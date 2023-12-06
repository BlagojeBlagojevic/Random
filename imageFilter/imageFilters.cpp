
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMETATION
#include "stb_image_write.h"
#include<stdio.h>
#include<stdint.h>
#include<math.h>
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

#undef main
#define arrSize(x) sizeof(x) / sizeof(x[0]) 



/*
#include <complex.h>


void fft(float *in,size_t pivot,float _Complex *out,size_t n)  //coll
{
    float pi=3.141569;
    assert(n > 0);
    //printf("nesto");
    if(n==1)
    {
        out[0]=in[0];
        return; //braking assert error
    }
    fft(in,pivot*2,out,n/2);//even
    fft(in+pivot,pivot*2,n/2+out,n/2);//odd  
    for (size_t k = 0; k < n/2; k++)
    {
        float t=(float)k/n;
        float _Complex e = out[k];
        float _Complex v = cexp(-2*pi*I*t)*out[k+n/2];
        out[k]=e+v;
        out[k+n/2]=e-v; 
    }
   
    
    
}

*/

void imageFilter(uint8_t *img,int imgSize)
{
    for(size_t i = 0;i < imgSize; i++)
	if(img[i] > 100)
           img[i] = 255*sin(i);
       	else
       	   img[i] = 0;
}

int main()
{
	int height,width,chanels;
    uint8_t *img = stbi_load("baa.ppm",&width,&height,&chanels,0);  
    uint32_t imgSize = width*height*chanels;
    float *imgTr = (float*)calloc(imgSize,sizeof(float));

	if(img==NULL)
        {printf("ERROR LOADING"); return -1;}
	size_t sum = 0;
	for(size_t i = 0;i <  imgSize;i++,sum+=img[i]);
	printf("sum is: 0X%X  imgSize: %d chanel %d,",sum / imgSize,imgSize,chanels);
	sum/=imgSize;
	for(size_t i = 0;i < imgSize; i++)
        imgTr[i] = (float)img[i]/255.0f;
         
	// Renderern
	SDL_Window *window = NULL;
	SDL_Renderer *renderer;
	SDL_CreateWindowAndRenderer(width, height,SDL_RENDERER_ACCELERATED,&window,&renderer);
	//_Complex float compNum[40000];
	//fft(imgTr,1,compNum,40000);
	//for(size_t i = 0;i < 255; i++)
      // printf("real %f imag %f\n",creal(compNum[i]),cimag(compNum[i]));
	imageFilter(img,imgSize);
	while(1){
	SDL_RenderClear(renderer);
	size_t i = 0; 
	for(size_t y = 0; y < height;y++)
	for(size_t x = 0; x < width;x++)
	{
		
		SDL_SetRenderDrawColor(renderer,img[i++],img[i++],img[i++],0);
		SDL_RenderDrawPoint(renderer,x,y);
		//system("pause");
	}
	
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	SDL_RenderPresent(renderer);		
		SDL_Event event;
		if(SDL_PollEvent(&event)){
			
		}
		
	}
	

	return 0;
}
