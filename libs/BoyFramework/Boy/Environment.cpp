#include "Environment.h"

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <cctype>
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "GL/gl.h"
#include "Game.h"
#include "GamePad.h"
#include "Graphics.h"
#include "ResourceLoader.h"
#include "ResourceManager.h"
#include "SoundPlayer.h"
#include "PersistenceLayer.h"
#include "Storage.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Image.h"
#include "Sound.h"
#include "TriStrip.h"

using namespace Boy;

SDL_semaphore *gLoadingSemaphore;
int loadingProc(void *data)
{
	Game *game = (Game*)data;

	// create a locked semaphore for synchronizing with loading thread:
	gLoadingSemaphore = SDL_CreateSemaphore(0);

	// load the game:
	game->load();

	// unlock the loading semaphore:
	SDL_SemPost(gLoadingSemaphore);

	return 0;
}

static Environment sEnvObj;
Environment *Environment::gInstance = &sEnvObj;

Environment::Environment()
{
}

Environment::~Environment()
{
}

Environment *Environment::instance()
{
	assert(gInstance!=NULL);
	return gInstance;
}

void Environment::init(Boy::Game *game, 
					   int screenWidth, 
					   int screenHeight, 
					   bool fullscreen, 
					   const char *windowTitle, 
					   const BoyLib::UString &persFile)
{
	mGame = game;
	mGraphics = new Graphics(screenWidth, screenHeight);
	mStorage = new Storage();
	mResourceLoader = new ResourceLoader();
	mResourceManager = new ResourceManager(mResourceLoader);
	mSoundPlayer = new SoundPlayer();
	mPersistenceLayer = new PersistenceLayer(persFile);

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER) == -1) { 
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

	int mixerFlags = MIX_INIT_OGG;
	if ((Mix_Init(mixerFlags) & mixerFlags) != mixerFlags) {
		printf("Could not initialize SDL_mixer: %s\n", Mix_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		printf("Could not open audio for SDL_mixer: %s\n", Mix_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	int videoFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if (fullscreen) videoFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    mWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, videoFlags);
    if (mWindow == NULL)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(mWindow);
    if (gl_context == NULL)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_MakeCurrent(mWindow, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

	int imgFlags = IMG_INIT_PNG;
	if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        printf("Failed to init image support: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
	}

	setupGL();

	// initialize timing stuff:
	mMinStepSize = 0;
	mUpdateCount = 0;
	mLastUpdate = SDL_GetTicks();
	mT0 = SDL_GetTicks();
	mIntervalFrameCount = 0;
	mPauseCount = 0;

	// mouse
	mMouseInBounds = false;
	for (int i=0 ; i<MOUSE_COUNT_MAX ; i++)
	{
		mMice[i] = new Mouse(i);
		mMice[i]->setConnected(false);
		mIsLeftMouseButtonDown[i] = false;
	}
	mShowSystemMouse = true;
	mMice[0]->setConnected(true); // i have no idea how to make multiple mice work

	// keyboard
	mKeyboard = new Keyboard();
	mKeyboard->setConnected(true);

	// gamepad
	for (int i=0 ; i<GAMEPAD_COUNT_MAX ; i++)
	{
		mGamePads[i] = new GamePad(i);
	}

	mFullScreenToggleDisableCount = 0;
}

void Environment::destroy()
{
	mGame = NULL;

	for (int i=0 ; i<MOUSE_COUNT_MAX ; i++)
	{
		delete mMice[i]; mMice[i] = NULL;
	}
	delete mKeyboard; mKeyboard = NULL;
	for (int i=0 ; i<GAMEPAD_COUNT_MAX ; i++)
	{
		delete mGamePads[i]; mGamePads[i] = NULL;
	}

	delete mGraphics; mGraphics = NULL;
	delete mPersistenceLayer; mPersistenceLayer = NULL;
	delete mResourceManager; mResourceManager = NULL;
	delete mResourceLoader; mResourceLoader = NULL;
	delete mSoundPlayer; mSoundPlayer = NULL;
	delete mStorage; mStorage = NULL;

	SDL_Quit();
	mWindow = NULL;
}

void Environment::startMainLoop()
{
    //preload
    mGame->preInitLoad();
    mGame->init();
    gLoadingSemaphore = NULL;
    SDL_CreateThread(loadingProc, NULL, mGame);

    //timing
	mT0 = SDL_GetTicks();
	mIntervalStartTime = SDL_GetTicks();

    SDL_Event event;

    while (!mShutdownRequested) {
        //when main loop started
        Uint32 t0 = SDL_GetTicks();
		
		if (gLoadingSemaphore!=NULL && SDL_SemTryWait(gLoadingSemaphore)==0)
		{
			// get rid of the semaphore:
			SDL_DestroySemaphore(gLoadingSemaphore);
			gLoadingSemaphore = NULL;

			// handle loading completion:
			mGame->loadComplete();
		}

		int scrollDelta = 0;
        while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_CONTROLLERAXISMOTION: {
					float value = event.caxis.value / 32768.0f;
					int i = mInstanceIdToGamePadId[event.cdevice.which];
					switch (event.caxis.axis) { // analog y is inverted for some reason
						case SDL_CONTROLLER_AXIS_LEFTX: mGamePads[i]->setAnalogLX(value); break;
						case SDL_CONTROLLER_AXIS_LEFTY: mGamePads[i]->setAnalogLY(-value); break;
						case SDL_CONTROLLER_AXIS_RIGHTX: mGamePads[i]->setAnalogRX(value); break;
						case SDL_CONTROLLER_AXIS_RIGHTY: mGamePads[i]->setAnalogRY(-value); break;
						case SDL_CONTROLLER_AXIS_TRIGGERLEFT: mGamePads[i]->setTriggerL(value); break;
						case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: mGamePads[i]->setTriggerR(value); break;
					}
					break;
				}
				case SDL_CONTROLLERBUTTONUP:
				case SDL_CONTROLLERBUTTONDOWN: {
					bool down = event.type == SDL_CONTROLLERBUTTONDOWN;

					GamePad::Button button = GamePad::BUTTON_UNKNOWN;
					switch (event.cbutton.button) {
						case SDL_CONTROLLER_BUTTON_A: button = GamePad::BUTTON_0; break;
						case SDL_CONTROLLER_BUTTON_B: button = GamePad::BUTTON_1; break;
						case SDL_CONTROLLER_BUTTON_X: button = GamePad::BUTTON_2; break;
						case SDL_CONTROLLER_BUTTON_Y: button = GamePad::BUTTON_3; break;
						case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: button = GamePad::BUTTON_L_SHOULDER; break;
						case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button = GamePad::BUTTON_R_SHOULDER; break;
						case SDL_CONTROLLER_BUTTON_LEFTSTICK: button = GamePad::BUTTON_L_STICK; break;
						case SDL_CONTROLLER_BUTTON_RIGHTSTICK: button = GamePad::BUTTON_R_STICK; break;
						case SDL_CONTROLLER_BUTTON_START: button = GamePad::BUTTON_START; break;
						case SDL_CONTROLLER_BUTTON_BACK: button = GamePad::BUTTON_AUX; break;
						case SDL_CONTROLLER_BUTTON_DPAD_UP: button = GamePad::BUTTON_DPAD_UP; break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN: button = GamePad::BUTTON_DPAD_DOWN; break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT: button = GamePad::BUTTON_DPAD_LEFT; break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: button = GamePad::BUTTON_DPAD_RIGHT; break;
					}

					int i = mInstanceIdToGamePadId[event.cdevice.which];
					mGamePads[i]->setButtonDown(button, down);
					break;
				}
				case SDL_CONTROLLERDEVICEADDED: {
					SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which);
					SDL_JoystickID instanceID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
					for (int i=0 ; i<GAMEPAD_COUNT_MAX ; i++) {
						if (!mGamePads[i]->isConnected()) {
							mGamePads[i]->setConnected(true);
							mInstanceIdToGamePadId[instanceID] = i;
							break;
						}
					}
					break;
				}
				case SDL_CONTROLLERDEVICEREMOVED: {
					int i = mInstanceIdToGamePadId[event.cdevice.which];
					mInstanceIdToGamePadId.erase(event.cdevice.which);
					mGamePads[i]->setConnected(false);
					break;
				}
				case SDL_KEYUP:
				case SDL_KEYDOWN: {
					if (event.key.repeat) break;

					bool down = event.type == SDL_KEYDOWN;

					Keyboard::Key key = Keyboard::getKey(event.key.keysym.sym);
					Keyboard::Modifiers mod = Keyboard::getModifiers(event.key.keysym.mod);

					wchar_t unicode = key == Keyboard::KEY_UNKNOWN ? event.key.keysym.sym : 0;

					if (down) {
						if ((mod & Keyboard::KEYMOD_ALT) && (key == Keyboard::KEY_RETURN)) {
							toggleFullScreen();
							break;
						}

						mKeyboard->fireKeyDownEvent(unicode, key, mod);
					} else {
						mKeyboard->fireKeyUpEvent(unicode, key, mod);
					}

					break;
				}
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN: {
					bool down = event.type == SDL_MOUSEBUTTONDOWN;

					Mouse::Button button = Mouse::BUTTON_UNKNOWN;
					switch (event.button.button) {
						case SDL_BUTTON_LEFT: button = Mouse::BUTTON_LEFT; break;
						case SDL_BUTTON_MIDDLE: button = Mouse::BUTTON_MIDDLE; break;
						case SDL_BUTTON_RIGHT: button = Mouse::BUTTON_RIGHT; break;
					}

					if (down) {
						mMice[0]->fireDownEvent(button);
					} else {
						mMice[0]->fireUpEvent(button);
					}

					break;
				}
				case SDL_MOUSEMOTION:
					mMice[0]->fireMoveEvent(event.motion.x, event.motion.y);
					break;
				case SDL_MOUSEWHEEL:
					scrollDelta += event.wheel.y;
					break;
				case SDL_QUIT:
					mShutdownRequested = true;
					break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							setupGL();
							break;
						case SDL_WINDOWEVENT_ENTER:
							mMice[0]->fireEnterEvent();
							break;
						case SDL_WINDOWEVENT_LEAVE:
							mMice[0]->fireLeaveEvent();
							break;
					}
					break;
			}
        }
		if (scrollDelta != 0) {
			mMice[0]->fireWheelEvent(scrollDelta);
		}

        if (mPauseCount==0) {
			update();
		}

		draw();
    	SDL_GL_SwapWindow(mWindow);

        printTimingStats();

		// calculate number of milliseconds this frame required:
		Uint32 t = SDL_GetTicks() - t0;

		// figure out if we need to sleep before the next frame:
		int sleepTime = (int)mMinStepSize - (int)t;
		if (sleepTime>0)
		{
			// sleep until it's time to calculate the next frame:
			sleep(sleepTime);
		}
    }

    mGame->preShutdown();
}

void Environment::update()
{
	// poll the game pads:
	// pollGamePads();

	// tick the sound player:
	getSoundPlayer()->tick();

	// update:
	Uint32 t = SDL_GetTicks();
	mGame->update((t-mLastUpdate)/1000.0f);
	mLastUpdate = t;
	mUpdateCount++;
	mIntervalFrameCount++;
}

void Environment::draw()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	mGraphics->getTransform().reset();

	mGame->draw(mGraphics);
}

void Environment::stopMainLoop()
{
	mShutdownRequested = true;
}

bool Environment::isShuttingDown()
{
	return mShutdownRequested;
}

bool Environment::isFullScreen()
{
	if (mWindow==NULL)
	{
		return false;
	}
	
	Uint32 flags = SDL_GetWindowFlags(mWindow);
    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        return true;
    }
    return false;
}

void Environment::toggleFullScreen()
{
	if (mFullScreenToggleDisableCount>0)
	{
		return;
	}

	SDL_SetWindowFullscreen(mWindow, isFullScreen() ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);

	setupGL();

	mGame->fullscreenToggled(isFullScreen());
}

void Environment::setupGL()
{
	int sW = screenWidth();
	int sH = screenHeight();
	int wW;
	int wH;
	SDL_GetWindowSize(mWindow, &wW, &wH);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, sW, sH, 0.0, 1.0, -1.0);

	float screenAspect = (float)sW / (float)sH;
	float windowAspect = (float)wW / (float)wH;
	int vX = 0;
	int vY = 0;
	int vW = wW;
	int vH = wH;
	if (screenAspect < windowAspect)
	{
		vW = (int)(vH * screenAspect);
		vX = (wW - vW) / 2;
	} else {
		vH = (int)(vW / screenAspect);
		vY = (wH - vH) / 2;
	}
	glViewport(vX, vY, vW, vH);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    mGraphics->setClearColor(0xff000000);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	mGraphics->setDrawMode(Graphics::DRAWMODE_NORMAL);
	glEnable(GL_MULTISAMPLE);

	mResourceManager->reloadResources();
}

void Environment::enableFullScreenToggle()
{
	assert(mFullScreenToggleDisableCount>0);
	mFullScreenToggleDisableCount--;
}

void Environment::disableFullScreenToggle()
{
	mFullScreenToggleDisableCount++;
}

Mouse *Environment::getFirstMouse()
{
	return mMice[0];
}

int Environment::getMouseCount()
{
	return MOUSE_COUNT_MAX;
}

Mouse *Environment::getMouse(int mouseId)
{
	assert(mouseId<MOUSE_COUNT_MAX);

	return mMice[mouseId];
}

void Environment::showSystemMouse(bool show)
{
	mShowSystemMouse = show;
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void Environment::fireMouseAdded(int mouseId)
{
	if (mGame!=NULL)
	{
		mGame->handleMouseAdded(mouseId);
	}
}

void Environment::fireMouseRemoved(int mouseId)
{
	if (mGame!=NULL)
	{
		mGame->handleMouseRemoved(mouseId);
	}
}

int Environment::getKeyboardCount()
{
	return 1;
}

Keyboard *Environment::getKeyboard(int i)
{
	return mKeyboard;
}

int Environment::getGamePadCount()
{
	return GAMEPAD_COUNT_MAX;
}

GamePad *Environment::getGamePad(int i)
{
	assert(i<GAMEPAD_COUNT_MAX);
	return mGamePads[i];
}

void Environment::fireGamePadAdded(int gamePadId)
{
	if (mGame!=NULL)
	{
		mGame->handleGamePadAdded(gamePadId);
	}
}

void Environment::fireGamePadRemoved(int gamePadId)
{
	if (mGame!=NULL)
	{
		mGame->handleGamePadRemoved(gamePadId);
	}
}

void Environment::vibrateGamePad(int gamePadId, int left, int right)
{
	SDL_JoystickID instanceId = -1;
	for (const auto& [k, v] : mInstanceIdToGamePadId)
	{
		if (v == gamePadId)
		{
			instanceId = k;
			break;
		}
	}
	assert(instanceId != -1);

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(instanceId);
	SDL_GameControllerRumble(controller, left, right, 0xffffffff); // duration should be indefinite. vibration can be stopped by setting left and right to 0
}

float Environment::getTime()
{
    return (SDL_GetTicks() - mT0) / 1000.0f;
}

Graphics *Environment::getGraphics()
{
	return mGraphics;
}
PersistenceLayer *Environment::getPersistenceLayer()
{
	return mPersistenceLayer;
}
ResourceManager *Environment::getResourceManager()
{
	return mResourceManager;
}
ResourceLoader *Environment::getResourceLoader()
{
	return mResourceLoader;
}
SoundPlayer *Environment::getSoundPlayer()
{
	return mSoundPlayer;
}
Storage *Environment::getStorage()
{
	return mStorage;
}

void Environment::sleep(int milliseconds)
{
    SDL_Delay(milliseconds);
}

void Environment::printTimingStats()
{
	// if it's time to calculate framerates:
	Uint32 dt = SDL_GetTicks() - mIntervalStartTime;
	if (dt > 1000)
	{
		// calculate fps/ups:
		float fps = (float)mIntervalFrameCount * 1000.0f / dt;

		// reset counters:
		mIntervalStartTime = SDL_GetTicks();
		mIntervalFrameCount = 0;

		envDebugLog("fps=%3.0f\n",fps);
	}
}

int Environment::stricmp( const char *pStr1, const char *pStr2 )
{
    if (pStr1 == NULL && pStr2 == NULL) return 0;
    if (pStr1 == NULL) return -1;
    if (pStr2 == NULL) return 1;
    
    while (*pStr1 != '\0' && *pStr2 != '\0') 
    {
        int c1 = std::tolower(static_cast<unsigned char>(*pStr1));
        int c2 = std::tolower(static_cast<unsigned char>(*pStr2));

        if (c1 < c2) 
        {
            return -1;
        } 
        if (c1 > c2) 
        {
            return 1;
        }

        pStr1++;
        pStr2++;
    }

    if (*pStr1 < *pStr2)
    {
        return -1;
    }
    if (*pStr1 > *pStr2)
    {
        return 1;
    }

    return 0;
}

int Environment::screenWidth()
{
	assert(gInstance!=NULL);
	return gInstance->getGraphics()->getWidth();
}

int Environment::screenHeight()
{
	assert(gInstance!=NULL);
	return gInstance->getGraphics()->getHeight();
}

Image *Environment::getImage(const std::string &id)
{
	Image *img = Environment::instance()->getResourceManager()->getImage(id);
	assert(img!=NULL);
	return img;
}

Image *Environment::getImage(const char *id, Image *defaultImg)
{
	if (id==NULL)
	{
		return NULL;
	}

	return getImage(std::string(id), defaultImg);
}

Image *Environment::getImage(const std::string &id, Image *defaultImg)
{
	Image *img = instance()->getResourceManager()->getImage(id);
	return img==NULL ? defaultImg : img;
}

void Environment::playSound(const char *id, bool loop, float volume)
{
	assert(id!=NULL);
	Sound *snd = instance()->getResourceManager()->getSound(id);
	playSound(snd,loop,volume);
}

void Environment::playSound(Sound *sound, bool loop, float volume)
{
	instance()->getSoundPlayer()->playSound(sound,volume,loop);
}

TriStrip *Environment::createTriStrip(int numVerts)
{
	TriStrip *strip = new TriStrip(numVerts);
	return strip;
}

void Environment::debugLog(const char *fmt, ...)
{
#if defined (_DEBUG)
	// write to console:
	printf("[t=%0.2f] ",getTime());
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);

	if (mLogFile!=NULL)
	{
		fprintf(mLogFile, "[t=%0.2f] ",getTime());
		va_list ap2;
		va_start(ap2, fmt);
		vfprintf(mLogFile, fmt, ap2);
		va_end(ap2);
		fflush(mLogFile);
	}
#endif
}