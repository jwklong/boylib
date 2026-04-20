#include "SoundPlayer.h"

#include <algorithm>
#include <assert.h>
#include "Environment.h"
#include "SDL2/SDL_mixer.h"
#include "ResourceManager.h"
#include "Sound.h"

using namespace Boy;

SoundPlayer::SoundPlayer()
{
}

SoundPlayer::~SoundPlayer()
{
}

void SoundPlayer::playSound(Sound *sound, float volume, bool loop)
{
    setVolume(sound, volume);
    Mix_PlayChannel(-1, sound->getChunk(), loop ? -1 : 0);
}

void SoundPlayer::playSoundChain(std::vector<Sound*> &sounds, bool loopLastSound)
{
    SoundChain *sc = new SoundChain();
    sc->loopLastSound = loopLastSound;
    sc->sounds = sounds;
    mSoundChains.push_back(sc);

    playSound(sc->sounds[0]);
}

void SoundPlayer::stopSound(Sound *sound)
{
    if (isPlaying(sound))
    {
        Mix_HaltChannel(getChannel(sound));
    }
}

void SoundPlayer::stopAllSounds()
{
    Mix_HaltChannel(-1);
}

void SoundPlayer::setVolume(Sound *sound, float volume)
{
    Mix_VolumeChunk(sound->getChunk(), (int)(volume*128));
}

float SoundPlayer::getVolume(Sound *sound)
{
    return (float)Mix_VolumeChunk(sound->getChunk(), -1) / 128.0f;
}

void SoundPlayer::setMasterVolume(float volume)
{
    Mix_Volume(-1, (int)(volume*128));
}

float SoundPlayer::getMasterVolume()
{
    return (float)Mix_Volume(-1, -1) / 128.0f;
}

void SoundPlayer::fadeIn(Sound *sound, float duration, bool loop)
{
	float now = Environment::instance()->getTime();
	sound->fade(0,1,now,duration);
	playSound(sound,0,loop);
    mFadingSounds.push_back(sound);
}

/**
 * Fades out a sound over a given duration.
 * If the sound is currently playing, it will be faded out to silence.
 * If the sound is not currently playing, this function does nothing.
 * @param sound The sound to be faded out.
 * @param duration The duration of the fade out in seconds.
 */
void SoundPlayer::fadeOut(Sound *sound, float duration)
{
    if (isPlaying(sound))
    {
        float now = Environment::instance()->getTime();
        sound->fade(getVolume(sound),0,now,duration);
        mFadingSounds.push_back(sound);
    }
}

int SoundPlayer::getChannel(Sound *sound)
{
    if (sound->getChunk() == NULL) {
        return -1;
    }
    int numChannels = Mix_AllocateChannels(-1);
    for (int i = 0; i < numChannels; i++) {
        if (Mix_Playing(i) && Mix_GetChunk(i) == sound->getChunk()) {
            return i;
        }
    }
    return -1;
}

bool SoundPlayer::isPlaying(Sound *sound)
{
    return getChannel(sound) != -1;
}

void SoundPlayer::tick()
{
    // iterate over all sound chains:
	int numChains = (int)mSoundChains.size();
	for (int i=numChains-1 ; i>=0 ; i--)
	{
		// get the chain:
		SoundChain *chain = mSoundChains[i];

		// if the first sound in the chain is done playing:
		if (!isPlaying(chain->sounds[0]))
		{
			// remove it:
			chain->sounds.erase(chain->sounds.begin());

			// if there are no more sounds in this chain:
			if (chain->sounds.size()==0)
			{
				// get rid of it:
				mSoundChains.erase(mSoundChains.begin()+i);
				delete chain;
			}
			else
			{
				// get the next sound:
				Sound *sound = chain->sounds[0];

				// if this is the last sound and we should loop it:
				if (chain->sounds.size()==1 && chain->loopLastSound)
				{
					// loop the sound:
					playSound(sound,1,true);

					// get rid of this sound chain:
					mSoundChains.erase(mSoundChains.begin()+i);
					delete chain;
				}
				else
				{
					// play the next sound:
					playSound(sound);
				}
			}
		}
	}

	// iterate over all fading sounds:
	int numFades = (int)mFadingSounds.size();
	for (int i=numFades-1 ; i>=0 ; i--)
	{
		Sound *snd = mFadingSounds[i];

		float currentTime = Environment::instance()->getTime();
		float fadeStartTime = snd->getFadeStartTime();
		float fadeDuration = snd->getFadeDuration();
		float v0 = snd->getFadeVolume0();
		float v1 = snd->getFadeVolume1();
		float progress = (currentTime - fadeStartTime) / fadeDuration;
		// if we're still fading:
		if (progress<1)
		{
			// fade:
			float volume = v0 + (v1-v0) * progress;
			if (getChannel(snd)!=-1)
			{
                setVolume(snd, volume);
			}
		}
		else
		{
			// we're done fading this sound:
			mFadingSounds.erase(mFadingSounds.begin()+i);

			// set final volume:
			if (getChannel(snd)!=-1)
			{
                setVolume(snd, v1);
			}

			// if we're fading out:
			if (v1==0)
			{
				// stop the sound:
				stopSound(snd);
			}
		}
	}
}