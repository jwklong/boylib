#include "Keyboard.h"

#include <algorithm>
#include <assert.h>
#include "Environment.h"
#include "KeyboardListener.h"
#include "SDL2/SDL.h"

using namespace Boy;

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
}

void Keyboard::addListener(KeyboardListener *listener)
{
	assert(find(mListeners.begin(),mListeners.end(),listener) == mListeners.end());
	mListeners.push_back(listener);
}

void Keyboard::removeListener(KeyboardListener *listener)
{
	assert(find(mListeners.begin(),mListeners.end(),listener) != mListeners.end());
	mListeners.erase(find(mListeners.begin(),mListeners.end(),listener));
}

void Keyboard::fireKeyDownEvent(wchar_t unicode, Key key, Modifiers mods)
{
	// envDebugLog("Keyboard::fireKeyDownEvent(): unicode=%c key=0x%02x\n",unicode,key);
	if (mListeners.size()>0 && isEnabled())
	{
		int numListeners = (int)mListeners.size();
		for (int i=0 ; i<numListeners ; i++)
		{
			mListeners[i]->keyDown(unicode, key, mods);
		}
	}
}

void Keyboard::fireKeyUpEvent(wchar_t unicode, Key key, Modifiers mods)
{
	// envDebugLog("Keyboard::fireKeyUpEvent(): unicode=%c key=0x%02x\n",unicode,key);
	if (mListeners.size()>0 && isEnabled())
	{
		int numListeners = (int)mListeners.size();
		for (int i=0 ; i<numListeners ; i++)
		{
			mListeners[i]->keyUp(unicode, key, mods);
		}
	}
}

Keyboard::Key Keyboard::getKey(SDL_Keycode key)
{
    switch (key) {
		case SDLK_BACKSPACE: return KEY_BACKSPACE;
		case SDLK_TAB: return KEY_TAB;
		case SDLK_RETURN: return KEY_RETURN;
		case SDLK_LSHIFT: return KEY_SHIFT;
		case SDLK_RSHIFT: return KEY_SHIFT;
		case SDLK_LCTRL: return KEY_CONTROL;
		case SDLK_RCTRL: return KEY_CONTROL;
		case SDLK_PAUSE: return KEY_PAUSE;
		case SDLK_ESCAPE: return KEY_ESCAPE;
		case SDLK_END: return KEY_END;
		case SDLK_HOME: return KEY_HOME;
		case SDLK_LEFT: return KEY_LEFT;
		case SDLK_UP: return KEY_UP;
		case SDLK_RIGHT: return KEY_RIGHT;
		case SDLK_DOWN: return KEY_DOWN;
		case SDLK_INSERT: return KEY_INSERT;
		case SDLK_DELETE: return KEY_DELETE;
		case SDLK_F1: return KEY_F1;
		case SDLK_F2: return KEY_F2;
		case SDLK_F3: return KEY_F3;
		case SDLK_F4: return KEY_F4;
		case SDLK_F5: return KEY_F5;
		case SDLK_F6: return KEY_F6;
		case SDLK_F7: return KEY_F7;
		case SDLK_F8: return KEY_F8;
		case SDLK_F9: return KEY_F9;
		case SDLK_F10: return KEY_F10;
		case SDLK_F11: return KEY_F11;
		case SDLK_F12: return KEY_F12;
    }
    return KEY_UNKNOWN;
}

Keyboard::Modifiers Keyboard::getModifiers(Uint16 mod)
{
	int output = KEYMOD_NONE;
	if ((mod & KMOD_LALT) || (mod & KMOD_RALT)) output |= KEYMOD_ALT;
	if ((mod & KMOD_LSHIFT) || (mod & KMOD_RSHIFT)) output |= KEYMOD_SHIFT;
	if ((mod & KMOD_LCTRL) || (mod & KMOD_RCTRL)) output |= KEYMOD_CTRL;
	//if ((mod & KMOD_LMETA) || (mod & KMOD_RMETA)) output |= KEYMOD_META;
	if ((mod & KMOD_NUM)) output |= KEYMOD_NUMLOCK;
	if ((mod & KMOD_CAPS)) output |= KEYMOD_CAPSLOCK;
	return (Modifiers)output;
}