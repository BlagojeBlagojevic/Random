#ifndef GAME_H
#define GAME_H
#include "utils.h"
#include "utils_include.h"
#include "sprite.h"

Sprite player[4];

void initGame(void) {

	if(SDL_Init(SDL_INIT_EVERYTHING)) {
		ERROR_BREAK("Init Error!!!\n");
		}

	SDL_Window *window =  SDL_CreateWindow(
	                        "asd",
	                        SDL_WINDOWPOS_CENTERED,
	                        SDL_WINDOWPOS_CENTERED,
	                        WIDTH,
	                        HEIGHT,
	                        SDL_WINDOW_OPENGL
	                      );
	if(window == NULL) {
		ERROR_BREAK("Window not inited!!!\n");
		}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(renderer == NULL) {
		ERROR_BREAK("Renderer not init!!!\n");
		}
	isClose = FALSE;
	//INIT SPRITES
	//player[0] = initSprite("asset/player/0.bmp", PLAYER,TRUE, WIDTH/2, HEIGHT/2, )
	




	}

void inputHandler() {

	const u8* stateOfKeyboard = SDL_GetKeyboardState(NULL);
	LOG("HANDLING USER INPUT\n");
	}


static inline void eventHandler(void) {
	//STA GOD DAL STATIC ILI NE
	static SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {

			case SDL_QUIT: {
					LOG("Window Closed!!!\n");
					isClose = TRUE;
					break;
					}
			}
		}
	}
	

static void mainRenderer(){
	SDL_RenderClear(renderer);
	
	//OTHTE SHIT
	SDL_RenderPresent(renderer);
	
}



#define FPS (u64)60
#define TARGETTIME (u64)(1000 / FPS)
u64 curentTime;
u64 passedTime;

static inline void gameLoop(){
		
		while(!isClose) {
		curentTime = SDL_GetTicks64();
		inputHandler();
		eventHandler();
		mainRenderer();
		passedTime = SDL_GetTicks64() - curentTime;
		if(passedTime < TARGETTIME) {
			SDL_Delay((u32)(TARGETTIME - passedTime));
			LOG("passed time %u\n", ((u32)(TARGETTIME - passedTime)));
			}



		}
	
	
}

#endif