#pragma once

#include <string>
#include <map>
#include <stdio.h>
#include "BoyLib/UString.h"
#include "SDL2/SDL.h"
#include "Mouse.h"
#include "Keyboard.h"

#define MOUSE_COUNT_MAX 4

namespace Boy
{
	class Game;
	class Graphics;
	class PersistenceLayer;
	class ResourceManager;
	class ResourceLoader;
	class SoundPlayer;
	class Storage;
	class Image;
	class Sound;
	class TriStrip;

	class Environment
	{
	public:
		Environment();
		virtual ~Environment();

		// static access method:
		static Environment *instance();

		// init and destroy
		void init(Game *game, int screenWidth, int screenHeight, bool fullscreen, 
			const char *windowTitle, const BoyLib::UString &persFile);
		void destroy();

		void startMainLoop();
		void stopMainLoop();
		bool isShuttingDown();

		
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

		FILE *mLogFile;
	};
}

//shorthand for debuglog
#define envDebugLog(format, args...) Boy::Environment::instance()->debugLog( format , ## args )