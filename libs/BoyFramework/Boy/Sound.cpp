#include "Sound.h"

#include <assert.h>
#include "Environment.h"
#include "ResourceLoader.h"
#include "SoundPlayer.h"

using namespace Boy;

Sound::Sound(ResourceLoader *loader, const std::string &path) : Resource(loader, path)
{
	mChunk = NULL;
	mFadeStartTime = -1;
	mFadeDuration = -1;
	mFadeVolume0 = -1;
	mFadeVolume1 = -1;
}

Sound::~Sound()
{
}

bool Sound::init()
{
	return mLoader->load(this);
}

void Sound::destroy()
{
    SoundPlayer *sp = Environment::instance()->getSoundPlayer();
    sp->stopSound(this);

    mChunk = NULL;
}

void Sound::reload()
{
	// do nothing, we don't want to reload sounds
}

void Sound::fade(float volume0, float volume1, float startTime, float duration)
{
    mFadeStartTime = startTime;
    mFadeDuration = duration;
    mFadeVolume0 = volume0;
    mFadeVolume1 = volume1;
}