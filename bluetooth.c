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
#include<assert.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<omp.h>
#undef main 
#define width  1500
#define height 800

#define ARRSIZE(x)  sizeof(x) / sizeof(x[0])

int Nsources = 0; 
int Nwall    = 0;


typedef struct SOURCE{
	
	SDL_Point position; // POSITION OF A SOURCE
	float n;            // REPRESENT ENVIRONMENT FACTOR 
	int A;              // RSSI VALUE ON 1m 
	int R;              // MAX RANGE BY CLASS OF DEVICE
	
}SOURCE;

typedef struct OBJECT{
	SDL_Point position; //Position of object
	int loss;           //Loss dbm if contact
	
	
}OBJECT;
int map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float CalculateRSSI( SOURCE *source,OBJECT *wall,int **color,int x, int y,int **RSSI)
{
	double distance = sqrt((x - source->position.x ) * (x - source->position.x ) +  (y - source->position.y) * (y - source->position.y )); 
	
	//decrease IS HAPENING IF ANGLE OF LINE IS THE SAME
		if(distance < source->R)
			return (10.0f * source->n*log10((distance)) + source->A)+RSSI[y][x];
		else
			return (10.0f * source->n*log10((distance)) + source->A);
}

void ChangeColor(SDL_Renderer *renderer, SOURCE *source,OBJECT *wall,int x, int y,int **color,int **RSSI)
{
	double distance = sqrt((x - source->position.x ) * (x - source->position.x ) +  (y - source->position.y) * (y - source->position.y )); 
	float RSSIa= CalculateRSSI(source,wall,color,x,y,RSSI);
	//printf("%f\n" ,RSSI);
	int maped = map(RSSIa,-50,-30,0,255);
	//printf("%d ",color[0][0]);
	if(RSSIa >= -50)
		if(color[y][x] < maped){
				color[y][x]=maped;
					SDL_SetRenderDrawColor(renderer,color[y][x]*exp(-0.02*distance),color[y][x]*exp(-0.05*distance),color[y][x]*exp(-0.05*distance),color[y][x]*exp(-0.05*distance));		
					//SDL_SetRenderDrawColor(renderer,color[y][x],0,0,255);
					SDL_RenderDrawPoint(renderer,x,y);
					
				
		    }
		//SDL_SetRenderDrawColor(renderer,0,0,0,0);



}

void WallChange(OBJECT *wall,SOURCE *source,int **RSSI)
{
	for(size_t k = 0; k < Nwall;k++)
	for(size_t i = 0; i < Nsources;i++){
		
		double distance = sqrt((wall[k].position.x - source[i].position.x ) * (wall[k].position.x - source[i].position.x ) 
								+  
								(wall[k].position.y - source[i].position.y) * (wall[k].position.y - source[i].position.y ));
		if(distance  <= source[i].R)
		{
		 
		//1 kvadrant
		if(source[i].position.y >= wall[k].position.y && source[i].position.x <= wall[k].position.x ){
			for(size_t j = 0; ;j++){
					RSSI[wall[k].position.y - j][wall[k].position.x + j]+=wall[k].loss;
					RSSI[wall[k].position.y - j][wall[k].position.x + j+1]+=wall[k].loss;
					RSSI[wall[k].position.y - j+1][wall[k].position.x + j]+=wall[k].loss;
					RSSI[wall[k].position.y - j+1][wall[k].position.x + j+1]+=wall[k].loss;
				//color[wall->position.y - j]wall->position.y + j]-=255;
				//system("pause");
				if(wall[k].position.y - j == 0)
					break;
				if(wall[k].position.x + j == height - 1)
					break;
				
				int dist =  sqrt(j*j*2);
				if(dist >= source[i].R)
					break;
				//RSSI;
			}
		}
		
		 //2 kvadrant
			if(source[i].position.y >= wall[k].position.y && source[i].position.x >= wall[k].position.x ){
			for(size_t j = 0; ;j++){
				RSSI[wall[k].position.y - j][wall[k].position.x - j]+=wall[k].loss;
				RSSI[wall[k].position.y - j][wall[k].position.x - j+1]+=wall[k].loss;
				RSSI[wall[k].position.y - j+1][wall[k].position.x - j]+=wall[k].loss;
				RSSI[wall[k].position.y - j+1][wall[k].position.x - j+1]+=wall[k].loss;
				//color[wall->position.y - j]wall->position.y + j]-=255;
				//system("pause");
				if(wall[k].position.y - j == 0)
					break;
				if(wall[k].position.x - j == height - 1)
					break;
				int dist =  sqrt(j*j*2);
				if(dist >= source[i].R)
					break;
				//RSSI;
			}
		}
		
		//3 kvadrant
		if(source[i].position.y <= wall[k].position.y && source[i].position.x >= wall[k].position.x ){
			for(size_t j = 0; ;j++){
				RSSI[wall[k].position.y + j][wall[k].position.x - j]+=wall[k].loss;
				RSSI[wall[k].position.y + j][wall[k].position.x - j+1]+=wall[k].loss;
				RSSI[wall[k].position.y + j+1][wall[k].position.x - j]+=wall[k].loss;
				RSSI[wall[k].position.y + j+1][wall[k].position.x - j+1]+=wall[k].loss;
				//color[wall->position.y - j]wall->position.y + j]-=255;
				//system("pause");
				if(wall[k].position.y + j == 0)
					break;
				if(wall[k].position.x - j == height - 1)
					break;
				int dist =  sqrt(j*j*2);
				if(dist >= source[i].R)
					break;
				//RSSI;
			}
		}
		
		
		//4 kvadrant
		if(source[i].position.y <= wall[k].position.y && source[i].position.x <= wall[k].position.x ){
			for(size_t j = 0; ;j++){
				RSSI[wall[k].position.y + j][wall[k].position.x + j]+=wall[k].loss;
				RSSI[wall[k].position.y + j][wall[k].position.x + j+1]+=wall[k].loss;
				RSSI[wall[k].position.y + j+1][wall[k].position.x + j]+=wall[k].loss;
				RSSI[wall[k].position.y + j+1][wall[k].position.x + j+1]+=wall[k].loss;
				//color[wall->position.y - j]wall->position.y + j]-=255;
				//system("pause");
				if(wall[k].position.y + j == 0)
					break;
				if(wall[k].position.x + j == height - 1)
					break;
				int dist =  sqrt(j*j*2);
				if(dist >= source[i].R)
					break;
				//RSSI;
			}
		}
		}
	}

}


void DrawRenderer(SDL_Renderer *renderer,SOURCE *source,OBJECT *wall)
{
	
	SDL_RenderClear(renderer);
		//RENDER SOURCE
	for(size_t i = 0; i <Nsources;i++){
		SDL_SetRenderDrawColor(renderer,255,0,0,0);
		SDL_Rect rect;
		rect.h = 10;
		rect.w = 10;
		rect.x = source[i].position.x;
		rect.y = source[i].position.y;
		
		
		SDL_RenderFillRect(renderer,&rect);
		//system("pause");
	}
	
	//RENDER OBSTACLE
	for(size_t i = 0; i < Nwall;i++)
	{
		SDL_SetRenderDrawColor(renderer,255,255,255,0);
		SDL_Rect rect;
		rect.h = 4;
		rect.w = 4;
	//	printf("%d %d",wall->position.x,wall->position.y);
		rect.x=wall[i].position.x;
		rect.y=wall[i].position.y;
		SDL_RenderFillRect(renderer,&rect);
		
	}
	//
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	SDL_RenderPresent(renderer);

	
	
}

void MainRenderer(SDL_Renderer *renderer,SOURCE *source,OBJECT *wall)  //Rendering on screen
{
	
	SDL_RenderClear(renderer);
	
	int **color = (int**)calloc(width ,sizeof(int*));

	
	for(size_t y = 0;y < height; y++)
		color[y] = (int*)calloc(width,sizeof(int));
		
		
		
	int **RSSI = (int**)calloc(width ,sizeof(int*));
;
	
	for(size_t y = 0;y < height; y++)
		RSSI[y] = (int*)calloc(width,sizeof(int));
	//system("pause");
	
	//ADJUST FOR WALL CHANGES
	WallChange(wall,source,RSSI);
	
	//RENDER SIGNAL DISTRIBUTION	
	for(size_t i = 0; i < Nsources;i++){
		// LIMIT TO SCREEN
		int start_x = (source[i].position.x - source[i].R);
		if(start_x < 0)
			start_x = 0;
		

		int stop_x = (source[i].position.x + source[i].R);
		if(stop_x > width)
			stop_x = width;
			
		int start_y = (source[i].position.y - source[i].R);
		if(start_y < 0)
			start_y = 0;
			
		int stop_y = (source[i].position.y + source[i].R);
		if(stop_y > height)
			stop_y = height;
		//
		
		// MAIN LOOPS
		for(size_t y = start_y; y < stop_y; y++){
			for(size_t x = start_x; x < stop_x; x++){
				
				//CHECK IF IS A WALL 
					int R_comp = (x - source[i].position.x)* (x - source[i].position.x) +  (y - source[i].position.y)*(y - source[i].position.y);
					if(R_comp <= (source[i].R*source[i].R))
					{
						//SDL_SetRenderDrawColor(renderer,255,255,255,255);
						//system("pause");
						ChangeColor(renderer,&source[i],wall,x,y,color,RSSI);
						
					}
					
				
			}
		}	
	}
	//
	for(size_t i = 0; i <Nsources;i++){
		SDL_SetRenderDrawColor(renderer,255,0,0,0);
		SDL_Rect rect;
		rect.h = 10;
		rect.w = 10;
		rect.x = source[i].position.x;
		rect.y = source[i].position.y;
		
		
		SDL_RenderFillRect(renderer,&rect);
		//system("pause");
	}
	
	//RENDER OBSTACLE
	for(size_t i = 0; i < Nwall;i++)
	{
		SDL_SetRenderDrawColor(renderer,255,255,255,0);
		SDL_Rect rect;
		rect.h = 4;
		rect.w = 4;
	//	printf("%d %d",wall->position.x,wall->position.y);
		rect.x=wall[i].position.x;
		rect.y=wall[i].position.y;
		SDL_RenderFillRect(renderer,&rect);
		
	}


	//
	//DrawRenderer(renderer,source,wall);
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	SDL_RenderPresent(renderer);
	
	
}



void pom()
{
	SOURCE source[Nsources];
	source[0].position.x = 300;
	source[0].position.y = 300;
	source[0].R = 300;
	source[0].n = -0.6;
	source[0].A = -30; 
	source[1].position.x = 700;
	source[1].position.y = 700;
	source[1].R = 300;
	source[1].n = -0.6;
	source[1].A = -30; 
	
	source[2].position.x = 700;
	source[2].position.y = 100;
	source[2].R = 300;
	source[2].n = -0.6;
	source[2].A = -30; 
	
	
	OBJECT wall[Nwall];
	wall[0].loss=-100;  // -1dbm
	wall[0].position.x = 750;
	wall[0].position.y = 750;
	
	wall[1].loss=-100;  // -1dbm
	wall[1].position.x = 350;
	wall[1].position.y = 350;
}

void ProcesEvent(SDL_Renderer *renderer, SOURCE *source, OBJECT *wall)
{
	SDL_Event event;
		while(SDL_PollEvent(&event)){
			if(event.button.button == SDL_BUTTON_LEFT)  //Left Draw WALL
			{
				int x,y;
				Nwall++;
				if(Nwall >= 1000)
					break;
				SDL_GetMouseState(&x,&y);
				wall[Nwall-1].position.x = x; 
				wall[Nwall-1].position.y = y;
				wall[Nwall-1].loss=-10;
				//printf("Nesto");
			}
			
			if(event.button.button == SDL_BUTTON_RIGHT)  //Right Draw source
			{
				int x,y;
				Nsources++;
				if(Nwall >= 1000)
					break;
				SDL_GetMouseState(&x,&y);
				source[Nsources-1].position.x = x; 
				source[Nsources-1].position.y = y;
				source[Nsources-1].R = 100;
				source[Nsources-1].n = -0.6;
				source[Nsources-1].A = -30;
				
				
				//printf("Nesto");
			}
			
			if(event.key.keysym.sym == SDLK_s){
				MainRenderer(renderer,source,wall);
				system("pause");
				exit(1);
				
			}
			
			
		};
}


int main()
{
	
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_CreateWindowAndRenderer(width,height,SDL_RENDERER_ACCELERATED,&window,&renderer);
	Nwall = 0;
	Nsources = 0;
	OBJECT *wall;
	SOURCE *source;
	
	//DIN alloc
	wall = (OBJECT*)calloc(10000,sizeof(OBJECT));
	source = (SOURCE*)calloc(10000,sizeof(SOURCE));
	//system("pause");
	//
	
	
	

	while(1)
	{
		ProcesEvent(renderer,source,wall);
		DrawRenderer(renderer,source,wall);
		//system("pause");		
		

		
		
	}
	
	return 0;
}
