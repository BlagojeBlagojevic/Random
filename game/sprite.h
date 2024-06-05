#ifndef SPRITE_H
#define SPRITE_H
#include  "utils.h"
#include  "utils_include.h"
//#include
//SPRITE TYPE
enum {

	BACKGROUND,
	PLAYER,
	ENEMY,


	};


//SPRITE DATA IF SPRITE HAS IT WE COUD HAVE MORE OF THIS
//KINDA TO NO USE NUMBERS IN STATE MACHINES (SWITCH STATMENTS)
enum {
		aas
	};


typedef struct {


	u16          type;
	SDL_Texture *texture;
	u8           isAnimated;
	u8           lastAnimation;
	u8           data[10];     //PROBOBLY REDEFINE
	SDL_Rect     bound;        //.w , .h SIZEOF SPRITE .x, .y POSITION OF SPRITE

	} Sprite;


Sprite initSprite(const char* name, u8 type, u8 isAnimated, i16 x, i16 y, i16 w, i16 h, u8 data[10]) {
	Sprite s;
	s.type = type;
	s.isAnimated = isAnimated;
	s.lastAnimation = 0;
	memcpy(s.data, data, 10*sizeof(u8)); //IF REDEFINE
	LOG("Load Data\n");

	s.bound.h = h;
	s.bound.w = w;
	s.bound.x = x;
	s.bound.y = y;
	LOG("Load Rect\n");

	SDL_Surface *tempSurface = IMG_Load(name);
	if(tempSurface == NULL) {
		ERROR_BREAK("ERROR LOADING SURFACE!!!");
		}
	s.texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	if(s.texture == NULL) {
		ERROR_BREAK("ERROR IN CREATING TEXTURE");
		}
	SDL_FreeSurface(tempSurface);

	return s;
	}


//RENDER SPRITE
static inline void renderSprite(Sprite s){

	SDL_RenderCopy(renderer, s.texture,NULL ,&s.bound);
	
}

#endif

