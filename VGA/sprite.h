#ifndef TEXTURE_H
#define TEHTURE_H
//#include<SDL2/SDL2_surface.h>
typedef struct sprite{

	SDL_Texture *texture; 
	SDL_Rect size, b;
	u8 type;
	
}Sprite;
//TYPE OF THE SPRITES
enum {
	PLAYER,
	WATER,
	GRASS,
	DIRTH,
	FOREST,
	ROCK,

	};


static inline Sprite initSprite(const char* name , u32 w, u32 h, u32 x , u32 y, u8 type){
	Sprite s;
	s.type = type;
	SDL_Surface *tempSurface = IMG_Load(name);
	if(tempSurface == NULL){
		ERROR_BREAK("SURFACE ERROR %s!!!\n", name);
		
	}
	s.texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
	if(s.texture == NULL){
		ERROR_BREAK("TEXTURE ERROR %s !!!\n", name);
	}
	LOG("FILE NAME %s !!!\n", name);
	SDL_FreeSurface(tempSurface);
	s.size.h = h;
	s.size.w = w;
	s.size.x = x;
	s.size.y = y;
	//s.b.x = s.b.y = s.b.w = s.b.h  = 100;
	return s;

}


static inline void renderSprite(Sprite s){

	SDL_RenderCopy(renderer, s.texture,NULL ,&s.size);
	
}


#endif