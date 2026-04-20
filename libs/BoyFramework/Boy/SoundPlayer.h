#pragma once

#include <vector>

namespace Boy
{
	class Sound;

	struct SoundChain
	{
		std::vector<Sound*> sounds;
		bool loopLastSound;
	};

	class SoundPlayer
	{
	public:
		SoundPlayer();
		virtual ~SoundPlayer();

		// play/stop methods:
		void playSound(Sound *sound, float volume=1, bool loop=false);
		void playSoundChain(std::vector<Sound*> &sounds, bool loopLastSound=false);
		void stopSound(Sound *sound);
		void stopAllSounds();

		// volume control:
		void setVolume(Sound *sound, float volume);
        float getVolume(Sound *sound);
		void setMasterVolume(float volume);
		float getMasterVolume();

		// fading:
		void fadeIn(Sound *sound, float duration, bool loop);
		void fadeOut(Sound *sound, float duration);

		// ticking:
		void tick();

		// misc:
        int getChannel(Sound *sound);
		bool isPlaying(Sound *sound);

    private:

		std::vector<Sound*> mFadingSounds;
		std::vector<SoundChain*> mSoundChains;

	};
}
