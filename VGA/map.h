#ifndef MAP_H
#define MAP_H
#include <stdlib.h>


#define MAP_X (u8)25
#define MAP_Y (u8)25


Sprite map[MAP_Y][MAP_X];

u8 mapTile[MAP_Y][MAP_X];
#include<time.h>
void generateMap() {
	
	srand(time(0));
	for(u8 y = 0; y < MAP_Y; y++) {
		for(u8 x = 0; x < MAP_X; x++) {
			mapTile[y][x] = rand()%(ROCK + 1);
			}
		}

	}

static inline void initMapSprites(void){
	
	for(u8 y = 0; y < MAP_Y; y++){
		const i16 w = WIDTH  / MAP_X;
		const i16 h = HEIGHT / MAP_Y;
		const i16 scale = 40;
		generateMap();
		for(u8 x = 0; x < MAP_X; x++){
			switch(mapTile[y][x]){
				case WATER:{
					map[y][x] = initSprite("0.jpg",w*scale, h*scale, x*scale*w, y*scale*h, WATER);
					break;
				}				
				case GRASS:{
					map[y][x] = initSprite("1.png",w*scale, h*scale, x*scale*w, y*scale*h, GRASS);
					break;
				}
				
				
				case DIRTH:{
					map[y][x] = initSprite("2.png",w*scale, h*scale, x*scale*w, y*scale*h, DIRTH);
					break;
				}
				
				case FOREST:{
					map[y][x] = initSprite("3.png",w*scale, h*scale, x*scale*w, y*scale*h, FOREST);
					break;
				}
				
				case ROCK:{
					map[y][x] = initSprite("4.png",w*scale, h*scale, x*scale*w, y*scale*h, ROCK);
					break;
				}
				
			}
			
		}
	}
	
	
	
}



static inline void renderMap(void){

	for(u8 y = 0; y < MAP_Y; y++){
		for(u8 x = 0; x < MAP_X; x++){
			renderSprite(map[y][x]);
		}}
	
	
}









#endif