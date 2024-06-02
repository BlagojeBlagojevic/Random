#include "game.h"
#undef main

int main() {

	// Init
	initGame();

	//




	while(!isClose) {
		curentTime = SDL_GetTicks64();
		eventHandler();
		mainRenderer();
		passedTime = SDL_GetTicks64() - curentTime;
		if(passedTime < TARGETTIME) {
			SDL_Delay((u32)(TARGETTIME - passedTime));
			LOG("passed time %u\n", ((u32)(TARGETTIME - passedTime)));
			}



		}

	//free(window);
	SDL_Quit();
	return 0;
	}