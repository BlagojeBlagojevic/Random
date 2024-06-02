#ifndef RENDER_H
#define RENDER_H

//SDL_INCLUDS
#define SDL_MAIN_HANDLED
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

#include "../types.h"
#include "../linmath.h"
typedef struct render_state{
	
	SDL_Window   *window;
	SDL_Renderer *renderer;
	f32           width;
	f32           height;
	
	
}Render_State;

void render_init(void);
void render_begin(void);
void render_end(void);
void render_quad(vec2 size, vec2 size, vec4 color);


#endif
