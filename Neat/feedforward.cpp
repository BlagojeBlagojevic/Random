#define NEAT_IMPLEMETATION
//#define TYPE int
#include "neat.h"
#include<time.h>

#define NUMBER_OF_SPICES 10
#define WIDTH 5
#define HEIGHT 5 




int main(){
	//NEAT n[NUMBER_OF_SPICES];  //MACRO EXTENDED
	//neat_alloc(n);
	//neat_rand(n,0,1);
	
	//Mat weigts = matrix_alloc(2,2);
	
	Mat weigts[2];
	Mat bias[2];
	Mat out[2];
	
	Mat input = matrix_alloc(1,10);
	weigts[0] = matrix_alloc(10,2);
	bias[0]   = matrix_alloc(1,2);
	out[0]    = matrix_alloc(1,2);
	weigts[1] = matrix_alloc(2,2);
	bias[1]   = matrix_alloc(1,2);
	out[1]    = matrix_alloc(1,1);
	
	//MATRIX_PRINT(out[0]);
	srand(time(0));
	//matrix_rand(out[0],0,1);
	matrix_rand(bias[0],0,1);
	matrix_rand(weigts[0],0,1);

	matrix_rand(bias[1],0,1);
	matrix_rand(weigts[1],0,1);
	matrix_rand(input,0,1);
	input.elem[0] = 1;
	input.elem[1] = 1;
	matrix_feedforward(out,input,weigts,bias,2);
	MATRIX_PRINT(weigts[0]);
	MATRIX_PRINT(bias[0]);
	MATRIX_PRINT(out[0]);
	MATRIX_PRINT(weigts[1]);
	MATRIX_PRINT(bias[1]);
	MATRIX_PRINT(out[1]);
	
	return 0;
}
