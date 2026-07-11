#define SDL_MAIN_HANDLED

#include "Boy/Environment.h"
#include "Demo2.h"
#include <SDL2/SDL.h>

int main(int argc, char* args[]) {

	Boy::Environment::instance()->init(
		Demo2::instance(),
        800, // width
        600, // height
		false, // fullscreen
		"Asteroids (v1.0)", // window title
		"demo1.dat", // persistence layer file name
		NULL); // no encryption

    Boy::Environment::instance()->startMainLoop();

	Demo2::destroy();
	Boy::Environment::instance()->destroy();

    return 0;
}
