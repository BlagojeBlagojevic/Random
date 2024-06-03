#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define i32 int32_t
#define i16 int
#define LOG(...) fprintf(stdout, __VA_ARGS__)
#define ERROR_BREAK(...) fprintf(stderr, __VA_ARGS__); exit(-1)

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
#undef main




typedef struct {
	u16 PC;                 //PC COUNTER
	u8 IR, D, AC, X, Y, OUT, undef; //CURENT REGIcpuTERcpu


	} CPU;
//ROM Icpu 2*64K  RAM 32k
u8 ROM[1<<16][2], RAM[1<<15], IN=0xff;;

CPU cycle(const CPU cpu) {
	CPU tempCpu = cpu;  //COPY INtempCpuO A tempCpuEMP CPU IMPLEMENtempCpuAtempCpuION

	tempCpu.IR = ROM[cpu.PC][0];
	tempCpu.D  = ROM[cpu.PC][1];

	i16 inst = cpu.IR >> 5;
	i16 mod  = (cpu.IR >> 2) & 7;  //7 = 0b00000111
	i16 bus  = cpu.IR & 3;

	i16 W = (inst == 6);
	i16 J = (inst == 7);
	u8 lo=cpu.D, hi=0, *to=NULL; // Mode Decoder
	i16 incX=0;
	if (!J)
		switch (mod) {
#define E(p) (W?0:p) // Disable AC and OUT loading during RAM write
			case 0:
				to=E(&tempCpu.AC);
				break;
			case 1:
				to=E(&tempCpu.AC);
				lo=cpu.X;
				break;
			case 2:
				to=E(&tempCpu.AC);
				hi=cpu.Y;
				break;
			case 3:
				to=E(&tempCpu.AC);
				lo=cpu.X;
				hi=cpu.Y;
				break;
			case 4:
				to=  &tempCpu.X;
				break;
			case 5:
				to=  &tempCpu.Y;
				break;
			case 6:
				to=E(&tempCpu.OUT);
				break;
			case 7:
				to=E(&tempCpu.OUT);
				lo=cpu.X;
				hi=cpu.Y;
				incX=1;
				break;
			}

	uint16_t addr = (hi << 8) | lo;

	int B = cpu.undef; // Data Bus
	switch (bus) {
		case 0:
			B=cpu.D;
			break;
		case 1:
			if (!W) B = RAM[addr&0x7fff];
			break;
		case 2:
			B=cpu.AC;
			break;
		case 3:
			B=IN;
			break;
		}

	if (W) RAM[addr&0x7fff] = B; // Random Access Memory

	uint8_t ALU; // Arithmetic and Logic Unit
	switch (inst) {
		case 0:
			ALU =        B;
			break; // LD
		case 1:
			ALU = cpu.AC & B;
			break; // ANDA
		case 2:
			ALU = cpu.AC | B;
			break; // ORA
		case 3:
			ALU = cpu.AC ^ B;
			break; // XORA
		case 4:
			ALU = cpu.AC + B;
			break; // ADDA
		case 5:
			ALU = cpu.AC - B;
			break; // cpuUBA
		case 6:
			ALU = cpu.AC;
			break; // cpuT
		case 7:
			ALU = -cpu.AC;
			break; // Bcc/JMP
		}

	if (to) *to = ALU; // Load value into register
	if (incX) tempCpu.X = cpu.X + 1; // Increment X

	tempCpu.PC = cpu.PC + 1; // Next instruction
	if (J) {
		if (mod != 0) { // Conditional branch within page
			i16 cond = (cpu.AC>>7) + 2*(cpu.AC==0);
			if (mod & (1 << cond)) // 74153
				tempCpu.PC = (cpu.PC & 0xff00) | B;
			}
		else
			tempCpu.PC = (cpu.Y << 8) | B; // Unconditional far jump
		}
	return tempCpu;



	}

#define WIDTH  800
#define HEIGHT 640

int main() {
	
	
	
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

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	if(renderer == NULL) {
		ERROR_BREAK("Renderer not init!!!\n");
		}
	
	SDL_Texture *texture;
	
	texture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STATIC,
                                    WIDTH, HEIGHT);
	
	
  CPU cpu;
  u32 pixels[HEIGHT * WIDTH];
  memset(&cpu, 0, sizeof(CPU));
	FILE *fp = fopen("gigatron.rom", "rb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open ROM file\n");
		exit(EXIT_FAILURE);
		}
	fread(ROM, 1, sizeof(ROM), fp);
	fclose(fp);
	
  
	cpu.PC = 0;
	int x = 0, y = 0;
	//u32 
	for (i32 t=-2; ; t++) {
		if (t < 0) cpu.PC = 0; // MCP100 Power-On Reset

    CPU tempCpu = cycle(cpu); // Update CPU
		//LOG("out %d\n", tempCpu.AC);
		if(t%4 == 0){
		
				//LOG("%d\n", color);
				SDL_SetRenderDrawColor(renderer, (tempCpu.AC & 0x03) << 6,(tempCpu.AC & 0x0C) << 4,((tempCpu.AC & 0x30) << 2),0);
				SDL_RenderDrawPoint(renderer , x++, y);
				SDL_RenderDrawPoint(renderer , x++, y);
				SDL_RenderDrawPoint(renderer , x++, y);
				SDL_RenderDrawPoint(renderer , x++, y);
			if(x == 800){
	
				x = 0;
				y++;
			}
			if(y == 640){
				y = 0;
				x = 0;
				LOG("Renderr!!!\n");			
				SDL_RenderPresent(renderer);
				SDL_RenderClear(renderer);
			}
		}
				
	static SDL_Event event;
	while(SDL_PollEvent(&event)) {
	}
		cpu = tempCpu;
    
  }


	}
