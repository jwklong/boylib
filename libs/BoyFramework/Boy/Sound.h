#pragma once

#include "Resource.h"
#include "SDL2/SDL_mixer.h"

namespace Boy
{
	class Sound : public Resource
	{
	public:

		Sound(ResourceLoader *loader, const std::string &path);
		virtual ~Sound();

		const std::string &getPath() { return mPath; }

		void setChunk (Mix_Chunk *chunk) { mChunk = chunk; }
        Mix_Chunk *getChunk() { return mChunk; }

		// fade stuff:
		void fade(float volume0, float volume1, float startTime, float duration);
		float getFadeStartTime() { return mFadeStartTime; }
		float getFadeDuration() { return mFadeDuration; }
		float getFadeVolume0() { return mFadeVolume0; }
		float getFadeVolume1() { return mFadeVolume1; }

		// overrides:
		virtual void reload();

	protected:

		virtual bool init();
		virtual void destroy();

	private:

        Mix_Chunk *mChunk;

		float mFadeStartTime;
		float mFadeDuration;
		float mFadeVolume0;
		float mFadeVolume1;
	};
};