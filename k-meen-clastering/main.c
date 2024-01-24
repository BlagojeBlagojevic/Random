#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
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

#define DIVISION 5
#define NCLUSTERS 10

#define WIDTH   (400/DIVISION)
#define HEIGHT  (400/DIVISION)

void Distribution(bool IsToRender[HEIGHT][WIDTH],int nClusters,int radius){
for (size_t i = 0; i < nClusters; i++)
{
  int x = rand()%(WIDTH - radius - 10);
  int y = rand()%(HEIGHT - radius - 10);
  for (size_t j = 0; j < (rand()%40 + 30); j++)
  {
    if(rand()%4 == 0)
      IsToRender[x + rand()%radius][y + rand()%radius] = true;
    else if(rand()%4 == 0)
      IsToRender[x - rand()%radius][y + rand()%radius] = true;
    else if(rand()%4 == 0)
      IsToRender[x + rand()%radius][y - rand()%radius] = true;
    else 
      IsToRender[x - rand()%radius][y - rand()%radius] = true;
  }
  
}
}





void updateClasters(SDL_Color ColorOfCluster[NCLUSTERS], 
                    SDL_Rect  centroids[NCLUSTERS],
                    SDL_Color ColorOfData[HEIGHT][WIDTH],
                    bool      IsToRender[HEIGHT][WIDTH]){
  
  
size_t witch_to = 0;
double min;
size_t Distancex[NCLUSTERS];
size_t Distancey[NCLUSTERS];
int   Nele[NCLUSTERS]; 
for (size_t i = 0; i < NCLUSTERS; i++)
{
  Distancex[i] = 0;
  Distancey[i] = 0;
  Nele[i] = 0;
}



for (size_t y = 0; y < HEIGHT; y++)
{
  for (size_t x = 0; x < WIDTH; x++)
  {
    if(IsToRender){
    for (size_t i = 0; i < NCLUSTERS; i++)
    {

      double distance = (x - centroids[i].x)*(x - centroids[i].x)+
                        (y - centroids[i].y)*(y - centroids[i].y);
      distance = sqrt(distance)*DIVISION;
      if (i == 0)
      {
        min = distance;
      
      }
      if (min > distance)
      {
          min = distance;
          witch_to = i;
         
      }
    }
          ColorOfData[y][x].r = ColorOfCluster[witch_to].r;
          ColorOfData[y][x].g = ColorOfCluster[witch_to].g;
          ColorOfData[y][x].b = ColorOfCluster[witch_to].b;
          ColorOfData[y][x].a = ColorOfCluster[witch_to].a;
          Distancex[witch_to]+=x;
          Distancey[witch_to]+=y;
          Nele[witch_to]++;
          
    }
  }
  
}
for (size_t i = 0; i < NCLUSTERS; i++)
{
  if (Distancex[i]==0 && Distancey[i] == 0)
  {
    centroids[i].x = rand()%WIDTH;
    centroids[i].y = rand()%HEIGHT;
  }
  
else{
  centroids[i].x = (int)(Distancex[i] / (Nele[i]+0.0001));
  centroids[i].y = (int)(Distancey[i] / (Nele[i]+0.0001));  
  printf("%d. x %d y %d\n",i,centroids[i].x,centroids[i].y);
  }
}

  
  
}
int main(void){

    SDL_Window    *window;
    SDL_Renderer  *renderer;
    SDL_CreateWindowAndRenderer(WIDTH*DIVISION,HEIGHT*DIVISION,SDL_RENDERER_ACCELERATED,&window,&renderer);
    //WIDTH  /= DIVISION;    //width,height / 5 of rectangle coud be render on screen
    //HEIGHT /= DIVISION; 
    SDL_Event     event;
    SDL_Color     ColorOfData[HEIGHT][WIDTH] ;
    SDL_Rect      Data[HEIGHT][WIDTH];
    bool          IsToRender[HEIGHT][WIDTH];
    size_t        count = 0; 
    SDL_Rect      centoids[NCLUSTERS];
    SDL_Color     ColorOfCluster[NCLUSTERS] ;

    srand(time(0));
    for (size_t i = 0; i < NCLUSTERS; i++)
    {
        ColorOfCluster[i].r = rand()%255;
        ColorOfCluster[i].g = rand()%255;
        ColorOfCluster[i].b = rand()%255;
        ColorOfCluster[i].a = rand()%255;
        centoids[i].h = 20;
        centoids[i].w = 20;
        centoids[i].x = rand()%WIDTH;
        centoids[i].y = rand()%HEIGHT;
    }
    
    for (size_t y = 0; y < HEIGHT; y++)
        {
            for (size_t x = 0; x < WIDTH; x++)
            {
                //ColorOfData[y][x] = 0X00000000;   
                IsToRender[y][x] = false;
                Data[y][x].h = DIVISION;
                Data[y][x].w = DIVISION;
                Data[y][x].x = x * 5;
                Data[y][x].y = y * 5;
                ColorOfData[y][x].a = 0XFF;
                ColorOfData[y][x].r = 0XFF;
                ColorOfData[y][x].g = 0XFF;
                ColorOfData[y][x].b = 0XFF;
                
            }
        }
        int r = rand()%30 + 5,cl = rand()%10+2;

       Distribution(IsToRender,cl,r);

        
size_t counter = 0;

//Main rendering loop
    while(1){
      count++;
       updateClasters(ColorOfCluster,centoids,ColorOfData,IsToRender);       
        if(SDL_PollEvent(&event)){
        
            
            if(event.type == SDL_QUIT){
                return 0; 
            }
        SDL_RenderClear(renderer);
        //centroids
  /*      
  for (size_t i = 0; i < NCLUSTERS; i++)
        {
            SDL_SetRenderDrawColor(renderer,
                                   ColorOfCluster[i].r,
                                   ColorOfCluster[i].g,
                                   ColorOfCluster[i].b,
                                   ColorOfCluster[i].a);
            //centoids[i].x*=10;
            //centoids[i].y*=10;
            
            SDL_RenderFillRect(renderer,&centoids[i]);
        }
  //*/    


        //data
        if(counter%10 == 0)
        for (size_t y = 0; y < HEIGHT; y++)
        {
            for (size_t x = 0; x < WIDTH; x++)
            {
                //printf("%u\n",ColorOfData[y][x]&0x000000FF);
                if(IsToRender[y][x] == true){
                SDL_SetRenderDrawColor(renderer,ColorOfData[y][x].r,ColorOfData[y][x].g,ColorOfData[y][x].b,ColorOfData[y][x].a);
                SDL_RenderFillRect(renderer,&Data[y][x]);
                }    
            }
            
        }
        
        SDL_SetRenderDrawColor(renderer,0,10,0,0);
        SDL_RenderPresent(renderer);

        
        }


    }

    printf("Nesto");
    return  0; 
}