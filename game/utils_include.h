#ifndef UTILSINCLUDE_H
#define UTILSINCLUDE_H
//MAINLY WILL CONTAIN SDL2 INCLUDES AND OF ALL OTHER LIBS
#include "utils.h"
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


//SIZE OF THE WINDOW WILL BE CONTAINED HEAR

#define WIDTH  800
#define HEIGHT 640

//ALSO RENDERER AND WINDOW OBJECTS WILL BE HEAR

SDL_Window   *window;
SDL_Renderer *renderer;
u8           isClose;






#endif