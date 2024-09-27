#ifndef MLP_H
#define MLP_H
#include "autograd.h"
//-------------- MLP IMPLEMENTATION ---------------//
#define SEED 1234
float rand_value_in_range(float min, float max) {
	return min + 2 * ((float)rand() /(float)RAND_MAX * max);
	}

//We will define Neuron as:
typedef struct Neuron {
	Value **weights;
	Value *bias;
	int number_of_inputs;
	int is_neuron_non_lin;
	//MAYBE NUM FOR OPERATIONS WHEN WE ADD MORE
	} Neuron;

//------------------- Neuron -----------------------//
//Init Neuron:
Neuron* Neuron_Init(int number_of_inputs, int is_neuron_non_lin) {
	//srand(SEED);
	Neuron* n = calloc(1, sizeof(Neuron));
	n->weights = calloc(number_of_inputs, sizeof(Value*));
	for(int i = 0; i < number_of_inputs; i++) {
		float temp_value = rand_value_in_range(-1, 1);
		n->weights[i] = Value_Init(temp_value);
		}

	//n->bias = Make_Value(rand_value_in_range(-1, 1));
	n->bias = Value_Init(0.0f);
	n->number_of_inputs = number_of_inputs;
	n->is_neuron_non_lin = is_neuron_non_lin;
	return n;
	}

//We will forward thru neurons and expect vector of values as output
//Our output will be a sum Neuron output is product of a value weigts and a previous value

Value* Neuron_Forward(Neuron* n, Value** v) {
	Value *sum = Value_Init(0.0f);
	for(int i = 0; i < n->number_of_inputs; i++) {
		Value* product = Mul(n->weights[i], v[i]);
		sum = Add(sum, product);
		}
	sum = Add(sum, n->bias);
	if(n->is_neuron_non_lin) {
		sum = LeakyRelu(sum);
		}


	return sum;

	}

void Neuron_Free(Neuron* neuron) {
	for (int i = 0; i < neuron->number_of_inputs; i++) {
		Value_Free(neuron->weights[i]);
		}
	free(neuron->weights);
	Value_Free(neuron->bias);
	free(neuron);
	}




//-------------------------------------------------//

//------------------- Layer -----------------------//
//We will define a layer as folowing:
typedef struct Layer {
	Neuron **neurons;
	int number_of_outputs;
	} Layer;
//As we can see is that is a Layer struct array of vectors(of NEURONS) so we init it like following:
Layer* Layer_Init(int number_of_inputs, int number_of_outputs, int is_layer_non_lin) {
	Layer* layer 	 = (Layer*)calloc(1, sizeof(Layer));
	layer->neurons = (Neuron**)calloc(number_of_outputs,  sizeof(Neuron*));
	for (int i = 0; i < number_of_outputs; i++) {
		layer->neurons[i] = Neuron_Init(number_of_inputs, is_layer_non_lin);
		}
	layer->number_of_outputs = number_of_outputs;
	return layer;
	}

//Forward thru layers will return a values of all neuron (This is nes for backprop)
Value** Layer_Forward(Layer* layer, Value **v) {
	Value** out = (Value**)calloc(layer->number_of_outputs, sizeof(Value*));
	for (int i = 0; i < layer->number_of_outputs; i++) {
		out[i] = Neuron_Forward(layer->neurons[i], v);
		}
	return out;

	}

void Layer_Free(Layer* layer) {
	for (int i = 0; i < layer->number_of_outputs; i++) {
		Neuron_Free(layer->neurons[i]);
		}
	free(layer->neurons);
	free(layer);
	}


//-------------------------------------------------//
//--------------------- MLP -----------------------//
//Mlp is a multiple layer and how many layers we have:
typedef struct MLP {
	Layer **layers;
	int  number_of_layers;
	} MLP;

MLP* MLP_Init(int* number_of_neurons_in_layer, int number_of_layer) {
	MLP* mlp = (MLP*)calloc(1, sizeof(MLP));
	mlp->layers = (Layer**)malloc((number_of_layer - 1) * sizeof(Layer*));
	for (int i = 0; i < number_of_layer - 1; i++) {
		int is_layer_non_lin = (i != number_of_layer - 2);  // nonlinearity for all layers except the last one
		LOG("Layer %d = %d\n", i, is_layer_non_lin);system("pause");
		mlp->layers[i] = Layer_Init(number_of_neurons_in_layer[i], number_of_neurons_in_layer[i+1],
			 is_layer_non_lin);
		}
	mlp->number_of_layers = number_of_layer - 1;
	return mlp;
	}
//Forward thru layers:
Value** MLP_Forward(MLP* mlp, Value** v) {
	for(int i = 0; i < mlp->number_of_layers; i++) {
		v = Layer_Forward(mlp->layers[i], v);
		}
	return v;
	}


//Print a structure of a mlp network
void Print_PARAMS(MLP* mlp) {
	LOG("\n------------ MLP ---------------\n");

	for (int i = 0; i < mlp->number_of_layers; i++) {
		Layer* layer = mlp->layers[i];
		LOG("\n--------- LAYER %d ------------\n", i);
		//LOG("\n--------------------------------\n");
		for (int j = 0; j < layer->number_of_outputs; j++) {
			Neuron* neuron = layer->neurons[j];
			Values_Print(neuron->weights, neuron->number_of_inputs);

			}
		}

	LOG("\n--------------------------------\n");
	LOG("\n\n");
	}

Value* MLP_Train(MLP* mlp, Value** x, Value** y_true, float lr) {

	// Forward pass
	Value** y_pred = MLP_Forward(mlp, x);

	// Compute loss
	Value* loss = Mse_Loss(y_pred, y_true, 2);


	// Update weights and biases using gradient descent
	for (int i = 0; i < mlp->number_of_layers; i++) {
		Layer* layer = mlp->layers[i];
		for (int j = 0; j < layer->number_of_outputs; j++) {
			Neuron* neuron = layer->neurons[j];
			Weights_Update(neuron->bias, lr);
			for (int k = 0; k < neuron->number_of_inputs; k++) {
				Weights_Update(neuron->weights[k], lr);
				}
			}
		}

	return loss;

	// free_value(loss);
	}

void MLP_free(MLP* mlp) {
	for (int i = 0; i < mlp->number_of_layers; i++) {
		Layer_Free(mlp->layers[i]);
		}
	free(mlp->layers);
	free(mlp);
	}


//-------------------------------------------------//
#endif
