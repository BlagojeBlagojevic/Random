#include<SDL2/SDL.h>
#include<SDL2/SDL_hints.h>
#include<SDL2/SDL_error.h>
#include<SDL2/SDL_log.h>
#include<SDL2/SDL_video.h>
#include<SDL2/SDL_render.h>
#include<SDL2/SDL_pixels.h>
#include<SDL2/SDL_rect.h>
#include<SDL2/SDL_surface.h>
#include<SDL2/SDL_clipboard.h>
#include<SDL2/SDL_events.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDl_timer.h>
#include<SDL2/SDL_keyboard.h>
#include<SDL2/SDL_scancode.h>
#include<stdio.h>
#include<stdlib.h>

#include<stdint.h>
#define DELAY(x) SDL_Delay(x)

#define TRUE   1
#define FALSE  0
#define WIDTH  800
#define HEIGHT 600

#define ERROR(...)       fprintf(stderr, __VA_ARGS__)
#define ERROR_BREAK(...) fprintf(stderr, __VA_ARGS__); exit(-1)
#define LOG(...)         fprintf(stdout, __VA_ARGS__)
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint32_t  u64;
typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef float     f32;
typedef double    f64;
//GLOBAL GAME VARS
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
u8 isClose = FALSE;



#include "sprite.h"
Sprite player;


#include "map.h"



#include<stdlib.h>
u8 playerOnTile;
u8 isColided = FALSE;
static inline u8 check_collision( SDL_Rect A, SDL_Rect B )
{
   
    i16 leftA, leftB;
    i16 rightA, rightB;
    i16 topA, topB;
    i16 bottomA, bottomB;

    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    if( bottomA <= topB )
    {
        return FALSE;
    }

    if( topA >= bottomB )
    {
        return FALSE;
    }

    if( rightA <= leftB )
    {
        return FALSE;
    }

    if( leftA >= rightB )
    {
        return FALSE;
    }

  
    return TRUE;
}
i16 move_x = 0, move_y = 0;
void inputHandler() {

	const u8* stateOfKeyboard = SDL_GetKeyboardState(NULL);
	if(stateOfKeyboard[SDL_SCANCODE_W]) {
		for(u8 y = 0; y < MAP_Y; y++) {
			for(u8 x = 0; x < MAP_X; x++) {
				
				map[y][x].size.y = -10;
				}
			}
			move_y-=1;
			
		}
	if(stateOfKeyboard[SDL_SCANCODE_S]) {
		for(u8 y = 0; y < MAP_Y; y++) {
			for(u8 x = 0; x < MAP_X; x++) {
				
				map[y][x].size.y -= 10;
				}
			}
			move_y=1;
		}
	if(stateOfKeyboard[SDL_SCANCODE_A]) {
		for(u8 y = 0; y < MAP_Y; y++) {
			for(u8 x = 0; x < MAP_X; x++) {

				map[y][x].size.x += 10;
				}
			}
			move_x=-1;
		}
	if(stateOfKeyboard[SDL_SCANCODE_D]) {
		for(u8 y = 0; y < MAP_Y; y++) {
			for(u8 x = 0; x < MAP_X; x++) {
				map[y][x].size.x -= 10;
				
				}
			}
			move_x=1;
		}
		LOG("PLAYER ON TILE %d", playerOnTile);
		//system("pause");
	}




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

	//SDL_CreateTextureFromSurface()
	player = initSprite("white_king.bmp", 100, 100, WIDTH/2, HEIGHT/2, PLAYER);
	initMapSprites();
	
	//map    = initSprite("ocean.png", WIDTH, HEIGHT, 0, 0);
	isClose = FALSE;

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
			case SDL_KEYDOWN: {
					inputHandler();
					break;
					}
			default: {
					break;
					}
			}
		}

	}


//LETS BE INT FRAMES NO FLOAT
#define FPS (u64)60
#define TARGETTIME (u64)(1000 / FPS)
u64 curentTime;
u64 passedTime;

void mainRenderer(void) {

	static i32 i;
	SDL_RenderClear(renderer);
	//SDL_RenderCopy(renderer, playerTexture, NULL, &a);
	//player.size.x++;

	//renderSprite(map);
	
	//checkColision();
	{
		//LOG("isColidede!!!\n");
		isColided = FALSE;
		for(u8 y = 0; y < MAP_Y; y++) {
			for(u8 x = 0; x < MAP_X; x++) {
				if(check_collision(player.size, map[y][x].size) == TRUE){
					if(map[y][x].type == WATER || map[y][x].type == ROCK || map[y][x].type == FOREST)
					{
							isColided = TRUE;
							LOG("Colision");
							break;
							//SDL_Delay(100);	
					}
					
				}
				//map[y][x].size.x -= 10;
				}
			}
		}
		for(u8 y = 0; y < MAP_Y && isColided; y++) {
			for(u8 x = 0; x < MAP_X; x++) {
				map[y][x].size.x += move_x;
				map[y][x].size.y += move_y;
			}}

	isColided = FALSE;
	move_x = 0;
	move_y = 0;
	renderMap();
	renderSprite(player);
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//CALL DRAWING FUNCTION IN BEATWEN AND LOGIC


	SDL_RenderPresent(renderer);

	LOG("frame num %d\n", i++);
	}

