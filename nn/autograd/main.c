#include "mlp.h"
#include <time.h>
float* One_Hot_Encode(float y_true) {
	float* encoded = (float*)calloc(2, sizeof(float));
	if (y_true==1.0) {
		encoded[0] = 1.0f;
		encoded[1] = 0.0f;
		}
	else {
		encoded[0] = 0.0;
		encoded[1] = 1.0;
		}
	return encoded;
	}



int main() {
	//--------------- Const Values ---------------------//
	const int NUM_OF_INPUTS = 1;
	const int NUM_OF_OUTPUTS = 2;
	const int NUM_IN_TRAINING = 100;
	const int NUM_OF_EPOCH = 500;
	const float LR = 0.001f;
	const int NUM_OF_BACKPROP = 2;
	//-------------------------------------------------//
	int num_neuron_in_layers[] = {NUM_OF_INPUTS, 5, 10, 5, NUM_OF_OUTPUTS}; //WE input one number and then we nead to determin is number even or odd
	int s = sizeof(num_neuron_in_layers) / sizeof(int);
	MLP *mlp = MLP_Init(num_neuron_in_layers, SIZE_OF_ARR(num_neuron_in_layers));
	system("pause");
	//Print_PARAMS(mlp);
	//-------------- Generate Dataset -----------------//

	float* training_arr = calloc(NUM_IN_TRAINING, sizeof(float));
	float* is_even_or_odd = calloc(NUM_IN_TRAINING, sizeof(float));
	for(int i = 0; i < NUM_IN_TRAINING; i++) {
		*(training_arr + i) = (float)(rand()%100);
		if((int)*(training_arr + i)%2 == 0)
			*(is_even_or_odd + i) = 1;
		else
			*(is_even_or_odd + i) = 0;

		LOG("\n%f %f\n",*(training_arr + i), *(is_even_or_odd + i));
		}
	//-------------------------------------------------//

	//------------------- Training --------------------//
	srand(time(0));
	Value** input= Value_Vector_Init(training_arr, NUM_IN_TRAINING);
	//Values_Print(input, NUM_IN_TRAINING);
	system("pause");
	
	Value* total_loss = Value_Init(0.0);
	float epoch_loss = 0.0f;
	//Make correct output

	for(int epoch = 0; epoch < NUM_OF_EPOCH; epoch++) {
		for(int num_in_training = 0; num_in_training < NUM_IN_TRAINING; num_in_training++) {
			float pom[] = {training_arr[num_in_training]};
			Value** input= Value_Vector_Init(pom, 1);
			float* out = One_Hot_Encode(is_even_or_odd[num_in_training]);
			Value** out_corect =  Value_Vector_Init(out, NUM_OF_OUTPUTS);
			
			Value* loss = MLP_Train(mlp, input, out_corect, LR);
			total_loss = Add(total_loss ,loss);
			epoch_loss += total_loss->value;
			
			// BACKPROPAGATION
			if(num_in_training%2 == 0){
				
				total_loss->gradient=1.0f;
				backward(total_loss);
				//Value_Print(total_loss);
				
			// resetting total_loss for a new epoch.
				total_loss = Value_Init(0.0f);	
			}
			
			//free(out);
			
			}
			LOG("\n\nEPOCH %d LOSSS: %f", epoch, epoch_loss/25);
      epoch_loss=0.0;
		}
	//-------------------------------------------------//
	system("pause");
	MLP_free(mlp);
	free(training_arr);
	free(is_even_or_odd);

	}
