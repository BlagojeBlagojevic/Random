#define MATRIX_IMPLEMETATION
//#define TYPE int
#include "neat.h"
#include<time.h>

#define NUMBER_OF_SPICES 100
#define NUMBER_OF_LAYER 2
#define NUMBER_OF_NEURON 5
#define NINPUTS 2
#define NOUT 1
//#define MUTATION_RATE 0.1
#define BETA 0.1
#define NUMBER_OF_SPICES_IN_CROSOWER 8

typedef struct {

	float fitnes;
	Mat input;
	Mat weigts[NUMBER_OF_LAYER];
	Mat bias[NUMBER_OF_LAYER];  
	Mat out[NUMBER_OF_LAYER];  
	Mat out_softmax;
	
}NEAT;
void neat_alloc(NEAT *n){
	
	  	//*n = (NEAT*)calloc(NUMBER_OF_SPICES, sizeof(NEAT*));
	  	for(size_t i = 0; i < NUMBER_OF_SPICES; i++){
	  		n[i].fitnes = 10;
	  		n[i].input       = matrix_alloc(1,NINPUTS);
	  		n[i].weigts[0]   = matrix_alloc(NINPUTS,NUMBER_OF_NEURON);
			  n[i].bias[0]	   = matrix_alloc(1,NUMBER_OF_NEURON);
 	      n[i].out[0]      = matrix_alloc(1,NUMBER_OF_NEURON);
     	  n[i].out_softmax = matrix_alloc(1,NOUT);
		
	  		
	  		for(size_t j = 1; j < NUMBER_OF_LAYER;j++){
            n[i].weigts[j]  = matrix_alloc(NUMBER_OF_NEURON,NUMBER_OF_NEURON);
	  				n[i].bias[j]    = matrix_alloc(1,NUMBER_OF_NEURON);
	  				n[i].out[j]     = matrix_alloc(1,NUMBER_OF_NEURON);
			  }
             //Widjeti kako
	  		//n[i].weigts[NUMBER_OF_LAYER - 1]  = matrix_alloc(NOUT,NUMBER_OF_NEURON);
		    //n[i].bias[NUMBER_OF_LAYER - 1]    = matrix_alloc(1,NOUT);
		    //n[i].out[NUMBER_OF_LAYER - 1]     = matrix_alloc(1,NOUT);
		  }
	
}
void neat_free(NEAT *n){
	
	for(size_t i = 0;i < NUMBER_OF_SPICES;i++){
	matrix_free(n[i].input);
	//system("pause");
	for(size_t j = 0;j < NUMBER_OF_LAYER;j++){
			matrix_free(n[i].weigts[j]);
		//	system("pause");
			matrix_free(n[i].bias[j]);
			//system("pause");
			matrix_free(n[i].out[j]);
			//system("pause");
		
	}	
	}
	
 
}


void neat_rand(NEAT *n, float low, float high){
	
	for(size_t i = 0;i < NUMBER_OF_SPICES; i++){
	 for(size_t j = 0; j < NUMBER_OF_LAYER;j++){
	 	     matrix_rand(n[i].bias[j],low,high);	
     		 matrix_rand(n[i].weigts[j],low,high);
	 }

	}
}

void neat_forward(NEAT *n){
	
	
	for(size_t i = 0;i < NUMBER_OF_SPICES; i++){
		//system("pause");
		matrix_feedforward(n[i].out,n[i].input,n[i].weigts,n[i].bias,NUMBER_OF_LAYER);
		matrix_copy(n[i].out_softmax,n[i].out[NUMBER_OF_LAYER-1]);
		matrix_softmax(n[i].out_softmax);
		//matrix_print_out(n[i].out_softmax,"out_SOFTMAX",NOUT);
		//matrix_print_out(n[i].out[NUMBER_OF_LAYER - 1],"OUT",NOUT);
		//system("pause");
	}
}

void neat_print(NEAT *n){
	
	MATRIX_PRINT(n[0].input);
	for(size_t j = 0; j < NUMBER_OF_SPICES;j++){
		system("pause");
		printf("\n");
		system("cls");
		printf("\t\t\t NEAT SPICES %d\n",j);
		printf("\n\n______________________________________________________________________________\n");
	  system("pause");
	  
		for(size_t i = 0; i < NUMBER_OF_LAYER;i++){
		
		printf("\ni = %d \n\nj = %d",i,j);
		MATRIX_PRINT(n[j].weigts[i]);
		printf("\ni = %d \n\nj = %d",i,j);
		MATRIX_PRINT(n[j].bias[i]);
		printf("\ni = %d \n\nj = %d",i,j);
		MATRIX_PRINT(n[j].out[i]);
		printf("\ni = %d \n\nj = %d",i,j);	
	}
	MATRIX_PRINT(n[j].out_softmax);
	printf("\nj = %d",j);
	}
}



NEAT n1[NUMBER_OF_SPICES];

void neat_crossover(NEAT *n){   //Best has most spots then the rest  selected 
	
	
  //system("pause");
	size_t min = -99999,min_index = 0;
	for(size_t i = 0;i < NUMBER_OF_SPICES;i++){
    if(n[i].fitnes >= min)
    {
    	min = n[i].fitnes;
    	min_index = i;
    	
		}
	}
	memcpy(&n1[0],&n[min_index],sizeof(n[min_index]));
	
	//neat_print(&n1[0]);
	//system("pause");
	
	
	for(size_t i = 0;i < (NUMBER_OF_SPICES - NUMBER_OF_SPICES_IN_CROSOWER);i++){
		memcpy(&n[i],&n1[0],sizeof(n1[0]));
		//system("pause");
	}	
	//neat_free(n1);
}





void neat_mutation(NEAT *n){
	for(size_t i = 0; i < NUMBER_OF_SPICES;i++){
	 for(size_t j = 0; j < NUMBER_OF_LAYER;j++){
	 	 matrix_mutation(n[i].weigts[j]);
	 	 matrix_mutation(n[i].bias[j]);
	 }
		
	}
}

void neat_reproduce(NEAT *n1,NEAT *n2){
	for(size_t i = 0;i < NUMBER_OF_LAYER;i++){
			matrix_reproduce(n1[0].weigts[i],n2[0].weigts[i]);
			matrix_reproduce(n1[0].bias[i],n2[0].bias[i]);
	}

}

void neat_crossover1(NEAT *n){   //Best has most spots then the rest  selected 
	//int sorted_indexis[NUMBER_OF_SPICES] = {0};
	
	size_t min_index = 0,min_index1 = 0;
	float min = 99999;
	for(size_t i = 0;i < NUMBER_OF_SPICES;i++){
		if(min  >= n[i].fitnes){
			//if(i!=0){
				min_index1 = min_index;
				min_index = i; 
				min = n[i].fitnes;
			//}
		}
	}


	
	if(min_index1==min_index)
	{
		min_index++;
	}
	neat_reproduce(&n[min_index],&n[min_index1]);
	
	for(size_t i = 0;i < NUMBER_OF_SPICES;i++){
		if(i!=min_index && i!=min_index){
			if(rand()%2)
			    neat_reproduce(&n[i],&n[min_index1]);
       else
			       neat_reproduce(&n[i],&n[min_index]);
		}
	}
		//neat_mutation(n);
		//CHECKING IS DECRCRISING
	
	
	
  if(n1[0].fitnes < n[0].fitnes){
		memcpy(&n[0],&n1[0],sizeof(n1[0]));
		
	}
	

	if(n1[1].fitnes < n[0].fitnes){
		memcpy(&n[1],&n1[1],sizeof(n1[1]));
	}
		memcpy(&n1[0],&n[0],sizeof(n[min_index]));
	  memcpy(&n1[1],&n[1],sizeof(n[min_index1]));
	//*/
}


void neat_crossover2(NEAT *n){
		 for(size_t i = 0; i < NUMBER_OF_SPICES;i++){
		 	int spices_number = rand()%NUMBER_OF_SPICES;
		 	int spices1_number = rand()%NUMBER_OF_SPICES;
		 	if((1 - n[i].fitnes) > rand_float());
		 	    neat_reproduce(&n[i],&n[spices_number]);
		 }

}

int main(){
	NEAT n[NUMBER_OF_SPICES];  //MACRO EXTENDED
	neat_alloc(n);
	neat_alloc(n1);
	srand(time(0));
	neat_rand(n,-3,3);
	Mat xor_model;
	size_t counter = 0;
	for(size_t i = 0; i < 1000;counter++,i++){
		//int a = rand()%2;
		//int b = rand()%2;
		//int out = a^b;
		//printf("%d xor %d = %d\n",a,b,out);
		for(size_t j = 0;j < NUMBER_OF_SPICES;j++)
		{
		
	    for(size_t y = 0;y < 2;y++){
	    	for(size_t x = 0;x < 2;x++){
	    	
	    	 n[j].input.elem[0] = (float)x;
				 n[j].input.elem[1] = (float)y;
				 int out = x^y;
				 	//printf("%f %f %f",(float)x,(float)y,(float)out);
	    		//system("pause");
	    	 	
	    	 float err = ((float)out - n[j].out[NUMBER_OF_LAYER - 1].elem[0]);
	    	 err*=err;
	    	 
         n[j].fitnes += err;
         neat_forward(n);
				}
				
        //n[j].fitnes = 100000 / (n[j].fitnes+0.001);				
			   n[j].fitnes = 1 - sqrt((n[j].fitnes/4));
			
			//system("pause");
			
			}
		}
		//neat_crossover(n);
		neat_crossover1(n);
		//neat_crossover2(n);
		neat_mutation(n);
		printf("GEN %d fitnes = %f\n ",counter,n[0].fitnes);
		
		if(n[0].fitnes < 0.01 && (i>10)){
			i = 10000;
			break;
		}
		for(size_t k = 0; k < NUMBER_OF_SPICES;k++)
		{
		n[k].fitnes = 0.0f;
	}
		
		//system("pause");
	}
	//MATRIX_PRINT(n[0].input);
	//MATRIX_PRINT(n[0].out[NUMBER_OF_LAYER - 1]);
	while(1){
		int a,b;
		printf("Unesite A,B : \n");
		scanf("%d %d",&a,&b);
		int out = a^b;
		float err = ((float)out - n[0].out[NUMBER_OF_LAYER - 1].elem[0]);
		for(size_t j = 0;j < NUMBER_OF_SPICES;j++)
		{
			n[j].input.elem[0] = (float)a;
			n[j].input.elem[1] = (float)b;
			
		}
		neat_forward(n);
		//neat_print(&n[0]);
		matrix_print_out(n[0].out[NUMBER_OF_LAYER - 1],"OUT ",NOUT-1);
		
	}
	MATRIX_PRINT(n[0].weigts[0]);
	MATRIX_PRINT(n[0].weigts[1]);
	//MATRIX_PRINT(n[0].bias[0]);
  //MATRIX_PRINT(n[0].bias[1]);
	//neat_print(n);
	
	return 0;
}

