#pragma once

#include <string>
#include <map>
#include <stdio.h>
#include "BoyLib/UString.h"
#include "SDL2/SDL.h"

#define MOUSE_COUNT_MAX 4
#define GAMEPAD_COUNT_MAX 4

namespace Boy
{
	class Game;
	class GamePad;
	class Graphics;
	class Image;
	class Keyboard;
	class Mouse;
	class PersistenceLayer;
	class ResourceLoader;
	class ResourceManager;
	class Sound;
	class SoundPlayer;
	class Storage;
	class TriStrip;

	class Environment
	{
	public:
		Environment();
		virtual ~Environment();

		// static access method
		static Environment *instance();

		// init and destroy
		void init(Game *game, int screenWidth, int screenHeight, bool fullscreen, 
			const char *windowTitle, const BoyLib::UString &persFile);
		void destroy();

		// main loop related
		void startMainLoop();
		void stopMainLoop();
		bool isShuttingDown();
        void showError(const std::string &message);

		bool isFullScreen();
		void toggleFullScreen();
		void enableFullScreenToggle();
		void disableFullScreenToggle();

		int getMouseCount();
		Mouse *getMouse(int mouseId);
		Mouse *getFirstMouse();
		void showSystemMouse(bool show);
		void fireMouseAdded(int mouseId);
		void fireMouseRemoved(int mouseId);

		int getKeyboardCount();
		Keyboard *getKeyboard(int i);

		int getGamePadCount();
		GamePad *getGamePad(int i);
		void fireGamePadAdded(int gamePadId);
		void fireGamePadRemoved(int gamePadId);
		void vibrateGamePad(int gamePadId, int leftVal, int rightVal);

		float getTime();

		Graphics *getGraphics();
		PersistenceLayer *getPersistenceLayer();
		ResourceManager *getResourceManager();
		ResourceLoader *getResourceLoader();
		SoundPlayer *getSoundPlayer();
		Storage	*getStorage();

        void sleep(int milliseconds);
		int stricmp( const char *pStr1, const char *pStr2 );

		//shortcut methods
		static int screenWidth();
		static int screenHeight();
		static Image *getImage(const std::string &id);
		static Image *getImage(const std::string &id, Image *defaultImg);
		static Image *getImage(const char *id, Image *defaultImg);
		static void playSound(const char *id, bool loop=false, float volume=1.0f);
		static void	playSound(Sound *sound, bool loop=false, float volume=1.0f);
		static TriStrip *createTriStrip(int numVerts);

		void debugLog(const char *fmt, ...);
	protected:
		void update();
		void draw();
		void setupGL();

		void checkMouseInBounds();

        void printTimingStats();

    public:
		static Environment *gInstance;

	protected:

        SDL_Window *mWindow;

		Game *mGame;
		Graphics *mGraphics;
		PersistenceLayer *mPersistenceLayer;
		ResourceManager *mResourceManager;
		ResourceLoader *mResourceLoader;
		SoundPlayer *mSoundPlayer;
		Storage *mStorage;

		bool mShutdownRequested;
		int mFullScreenToggleDisableCount;

		// timing related:
		Uint32 mT0;
		Uint32 mPauseTime;
		Uint32 mPauseDuration;
		int mPauseCount;
		Uint32 mMaxFrameRate; // maximum allowed frame rate
		Uint32 mMinStepSize; // minimum delay between frames (in ms)
		Uint32 mUpdateCount;
		Uint32 mLastUpdate;

        Uint32 mIntervalFrameCount;
        Uint32 mIntervalStartTime;

		Mouse *mMice[MOUSE_COUNT_MAX];
		bool mShowSystemMouse;
		bool mMouseInBounds;
		bool mIsLeftMouseButtonDown[MOUSE_COUNT_MAX];

		Keyboard *mKeyboard;

		GamePad *mGamePads[GAMEPAD_COUNT_MAX];
		std::map<SDL_JoystickID,int> mInstanceIdToGamePadId;

		FILE *mLogFile;
	};
}

//shorthand for debuglog
#define envDebugLog(format, args...) Boy::Environment::instance()->debugLog( format , ## args )
