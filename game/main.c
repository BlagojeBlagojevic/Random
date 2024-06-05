#include<stdio.h>
#include "utils.h"
#include "utils_include.h"

#include "game.h"
#undef main   //DEFINE HADER WITCH IS USED FOR THIS

int main(){
	
	LOG("NESTO\n");
	initGame();
	
	gameLoop();

	return 0;	
}