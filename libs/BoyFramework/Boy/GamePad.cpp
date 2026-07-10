#include "GamePad.h"

#include <algorithm>
#include "BoyLib/BoyUtil.h"
#include "Environment.h"
#include "GamePadListener.h"
#include "Graphics.h"
#include "Image.h"
#include "MouseListener.h"

using namespace Boy;

GamePad::GamePad(int id)
{
	mId = id;
	for (int i=0 ; i<BUTTON_COUNT ; i++)
	{
		mIsButtonDown[i] = false;
	}
}

GamePad::~GamePad()
{
}

void GamePad::vibrate(int left, int right)
{
	Environment::instance()->vibrateGamePad(mId, left, right);
}

void GamePad::addListener(GamePadListener *listener)
{
	assert(find(mListeners.begin(),mListeners.end(),listener) == mListeners.end());
	mListeners.push_back(listener);
}

void GamePad::removeListener(GamePadListener *listener)
{
	assert(find(mListeners.begin(),mListeners.end(),listener) != mListeners.end());
	mListeners.erase(find(mListeners.begin(),mListeners.end(),listener));
}

void GamePad::fireDownEvent(Button button)
{
	mIsButtonDown[button] = true;
	if (mListeners.size()>0 && isEnabled())
	{
		int numListeners = (int)mListeners.size();
		for (int i=0 ; i<numListeners ; i++)
		{
			mListeners[i]->gamePadButtonDown(this,button);
		}
	}
}

void GamePad::fireUpEvent(Button button)
{
	mIsButtonDown[button] = false;
	if (mListeners.size()>0 && isEnabled())
	{
		int numListeners = (int)mListeners.size();
		for (int i=0 ; i<numListeners ; i++)
		{
			mListeners[i]->gamePadButtonUp(this,button);
		}
	}
}

void GamePad::setEnabled(bool enabled)
{
	Controller::setEnabled(enabled);
}

void GamePad::setConnected(bool connected) 
{ 
	// if there's no change, return:
	if (connected==isConnected())
	{
		return;
	}

	// connect the controller:
	Controller::setConnected(connected);

	// fire notification:
	if (connected)
	{
		envDebugLog("gamepad %d connected\n",mId);
		Environment::instance()->fireGamePadAdded(mId);
	}
	else
	{
		envDebugLog("gamepad %d disconnected\n",mId);
		Environment::instance()->fireGamePadRemoved(mId);
	}
}

float GamePad::getTriggerL()
{
	return mTriggerL;
}

float GamePad::getTriggerR()
{
	return mTriggerR;
}

void GamePad::setTriggers(float l, float r)
{
	mTriggerL = l;
	mTriggerR = r;
}

void GamePad::setButtonDown(Button button, bool down)
{
	if (mIsButtonDown[button]==down)
	{
		return;
	}

	mIsButtonDown[button] = down;
	if (down)
	{
		// envDebugLog("gamepad %d: button %d down\n",mId,button);
		fireDownEvent(button);
	}
	else
	{
		// envDebugLog("gamepad %d: button %d up\n",mId,button);
		fireUpEvent(button);
	}
}

bool GamePad::isButtonDown(Button button)
{
	assert(button<BUTTON_COUNT);
	return mIsButtonDown[button];
}

const BoyLib::Vector2 &GamePad::getAnalogL()
{
	return mAnalogL;
}

const BoyLib::Vector2 &GamePad::getAnalogR()
{
	return mAnalogR;
}

void GamePad::setAnalogL(float x, float y)
{
	mAnalogL.x = x;
	mAnalogL.y = y;
	// envDebugLog("gamepad %d: left stick: (%f, %f)\n",mId,mAnalogL.x,mAnalogL.y);
}

void GamePad::setAnalogR(float x, float y)
{
	mAnalogR.x = x;
	mAnalogR.y = y;
	// envDebugLog("gamepad %d: right stick: (%f, %f)\n",mId,mAnalogR.x,mAnalogR.y);
}