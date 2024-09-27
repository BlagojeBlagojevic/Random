#ifndef AUTOGRAD_H
#define AUTOGRAD_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<unistd.h>
#define LOG(...)  fprintf(stdout, __VA_ARGS__)
#define ERROR(...) fprintf(stdout, __VA_ARGS__); exit(-1)
#define SIZE_OF_ARR(x) sizeof(x) / sizeof(x[0])

//LOG VARS
#define LOG_VALUE 0

//CONSTS
#define MAX_SIZE_OF_NODE 1000
#define DEAFULT_GRADIENT 1.0f

#define MAX_GRADIENT  10.0f
#define MIN_GRADIENT -10.0f
#define RELU_ALPHA   -0.01f



typedef struct Value {
	float    value;
	float    gradient;
	struct Value  **children;
	int      number_of_children;
	void (*backward)(struct Value*);
	} Value;


void Value_Print(Value *v) {
	LOG("Value{\n\tvalue-> %f\n\tgradient-> %f\n\tnumber of children-> %d\n     }\n",
	    v->value, v->gradient, v->number_of_children);

	if(v->number_of_children && 0) {
		LOG("\n------------CHILDREN--------------\n");
		for(int i = 0; i < v->number_of_children; i++) {
			Value_Print(v->children[i]);
			}
		}
	}
void Values_Print(Value **v, int size) {
	for(int i = 0; i < size; i++) {
		LOG("\n------------------------------\n");
		Value_Print(v[i]);
		}
	}



Value* Value_Init(float x) {
	Value *v = calloc(1, sizeof(Value));
	if(v == NULL) {
		ERROR("Error in init of a value!!!\n\n");
		}
	v->value = x;
	v->gradient = 0.0f;
	v->children = NULL;
	v->number_of_children = 0;
	v->backward = NULL;
#if LOG_VALUE
	Value_Print(v);
#endif

	return v;
	}




Value** Value_Vector_Init(float *vector, int size_of_vector) {
	Value **v = calloc(size_of_vector, sizeof(Value*));
	if(v == NULL) {
		ERROR("Error in init of a value!!!\n\n");
		}
	for(int i = 0; i < size_of_vector; i++) {

		v[i] =  Value_Init(vector[i]);

		}
	return v;
	}
//init vector as null values
Value** Value_Vector_Init_Deafult(int size_of_vector) {
	Value **v = calloc(size_of_vector, sizeof(Value*));
	if(v == NULL) {
		ERROR("Error in init of a value!!!\n\n");
		}
	for(int i = 0; i < size_of_vector; i++) {

		v[i] = Value_Init(0.0f);
		}
	return v;
	}





//Topological sort is a graph traversal algo in witch
//we nead to visit all nodes of same distance("Number of level from end of the graph")
//C++ Topo sort https://www.geeksforgeeks.org/topological-sorting/

void Build_Topological(Value *v, Value **topo, int* topological_size, Value **visited, int *visited_size) {
	//Escape condition
	for(int i = 0; i < *visited_size; i++) {
		if(visited[i] == v) return;
		}
	//If we are not exit
	//We are placed inside of a visited graph
	visited[*visited_size] = v;
	(*visited_size)++;
	//Go Thrue all children in a graph and then construct top
	for(int i = 0; i < v->number_of_children; i++) {
		Build_Topological(v->children[i], topo, topological_size, visited, visited_size);
		}
	topo[*topological_size] = v;
	(*topological_size)++;


	}
//Function used for backpropagation starts from root node


void backward(Value *root) {
	//Maybe just one we see

	Value* topo[MAX_SIZE_OF_NODE]; //= Value_Vector_Init_Deafult(MAX_SIZE_OF_NODE);
	int topological_size = 0;
	Value* visited[MAX_SIZE_OF_NODE]; //= Value_Vector_Init_Deafult(MAX_SIZE_OF_NODE);
	int visited_size = 0;
	Build_Topological(root, topo, &topological_size, visited, &visited_size);
	root->gradient = DEAFULT_GRADIENT;
	for (int i = topological_size - 1; i >= 0; --i) {
		// printf("%.2f", topo[i]->val);
		// printf("\n");
		if (topo[i]->backward) {
			topo[i]->backward(topo[i]);
			}
		}

	//Values_Print(topo, topological_size);

	//Values_Print(visited, visited_size);
	//Value_Print(root);
	}






void Limit_Gradient(Value *v, float min, float max) {

	if(v->gradient > max) {
		v->gradient = max;
		}
	else if(v->gradient < min) {
		v->gradient = min;
		}
	else return;
	}


//---------------BACKPROP OPERATIONS---------------//
//We Take in consid a chain rule so always grad of prew
//Scalar + operation  v = a + b => dv/db = 1 ^ dv/da = 1
void Add_Backward(Value *v) {
	v->children[0]->gradient += v->gradient;
	v->children[1]->gradient += v->gradient;
	Limit_Gradient(v->children[0], MIN_GRADIENT, MAX_GRADIENT);
	Limit_Gradient(v->children[1], MIN_GRADIENT, MAX_GRADIENT);
	}
//Scalar - operation v = a - b => bv/db = -1 ^ dv/da = 1
void Sub_Backward(Value *v) {
	v->children[0]->gradient += v->gradient;
	v->children[1]->gradient += -v->gradient;
	Limit_Gradient(v->children[0], MIN_GRADIENT, MAX_GRADIENT);
	Limit_Gradient(v->children[1], MIN_GRADIENT, MAX_GRADIENT);
	}



//Scalar * operation v= a*b => dv/da = b ^ dv/db  = a
void Mul_Backward(Value *v) {
	v->children[0]->gradient += v->children[1]->value * v->gradient;
	v->children[1]->gradient += v->children[0]->value * v->gradient;
	Limit_Gradient(v->children[0], MIN_GRADIENT, MAX_GRADIENT);
	Limit_Gradient(v->children[1], MIN_GRADIENT, MAX_GRADIENT);
	}

//Scalar / operation v = a / b => dv / da = (1/b) ^ dv/db = a*(-1/(b*b))
void Div_Backward(Value *v) {

	v->children[0]->gradient += (1 / v->children[1]->value) * v->gradient;
	v->children[1]->gradient += (-v->children[0]->value
	                             / (v->children[1]->value * v->children[1]->value)) * v->gradient;

	Limit_Gradient(v->children[0], MIN_GRADIENT, MAX_GRADIENT);
	Limit_Gradient(v->children[1], MIN_GRADIENT, MAX_GRADIENT);
	}


//Scalar operation ^(exponent) y = a^b => dv/da =  b * a^(b-1) ^ dv/db = a^b * log(a) = v * log(a)
void Power_Backward(Value* v) {
	v->children[0]->gradient += (v->children[1]->value * pow(v->children[0]->value, v->children[1]->value - 1))
	                            * v->gradient;
	if (v->children[0]->value > 0) {  // Ensure base is positive before computing log
		v->children[1]->gradient += (log(v->children[0]->value) * pow(v->children[0]->value, v->children[1]->value))
		                            * v->gradient;
		}
	Limit_Gradient(v->children[0], MIN_GRADIENT, MAX_GRADIENT);
	Limit_Gradient(v->children[1], MIN_GRADIENT, MAX_GRADIENT);
	}


//Actiavation operation LeakyRelu v = LeakyRelue(a) => dv/da = 1 if(a > 0) dv/da = 1 else dv/da=-RELU_ALPHA
void LeakyRelu_Backward(Value *v) {
	if (v->children[0]->value > 0) {
		v->children[0]->gradient += v->gradient;
		}
	else {
		v->children[0]->gradient += v->gradient * RELU_ALPHA;
		}
	Limit_Gradient(v->children[0], MIN_GRADIENT, MAX_GRADIENT);

	}



//-------------------------------------------------//
//---------Graph Construction Operations-----------//

//This is forward operation this operations has to construct a graph
Value* Add(Value* a, Value* b) {
	Value* out = (Value*)calloc(1, sizeof(Value));
	out->value = a->value + b->value;
	out->gradient = 0;
	out->children = (Value**)calloc(2, sizeof(Value*));
	out->children[0] = a;
	out->children[1] = b;
	out->number_of_children = 2;
	out->backward = Add_Backward;
	return out;
	}

Value* Sub(Value* a, Value* b) {
	Value* out = (Value*)calloc(1, sizeof(Value));
	out->value = a->value - b->value;
	out->gradient = 0;
	out->children = (Value**)calloc(2, sizeof(Value*));
	out->children[0] = a;
	out->children[1] = b;
	out->number_of_children = 2;
	out->backward = Sub_Backward;
	return out;
	}


Value* Mul(Value* a, Value* b) {
	Value* out = (Value*)calloc(1, sizeof(Value));
	out->value = a->value * b->value;
	out->gradient = 0;
	out->children = (Value**)calloc(2, sizeof(Value*));
	out->children[0] = a;
	out->children[1] = b;
	out->number_of_children = 2;
	out->backward = Mul_Backward;
	return out;
	}

Value* Divide(Value* a, Value* b) {
	if(b->value == 0.0) {
		ERROR("Divison by zero\n\n");
		}

	Value* out = (Value*)calloc(1, sizeof(Value));
	out->value = a->value / b->value;
	out->gradient = 0;
	out->children = (Value**)calloc(2, sizeof(Value*));
	out->children[0] = a;
	out->children[1] = b;
	out->number_of_children = 2;
	out->backward = Div_Backward;
	return out;
	}

Value* Power(Value* a, Value* b) {
	Value* out = (Value*)calloc(1, sizeof(Value));
	out->value = pow(a->value, b->value);
	out->gradient = 0;
	out->children = (Value**)calloc(2, sizeof(Value*));
	out->children[0] = a;
	out->children[1] = b;
	out->number_of_children = 2;
	out->backward = Power_Backward;
	return out;
	}
Value* LeakyRelu(Value* a) {
	Value* out = (Value*)calloc(1, sizeof(Value));

	if (a->value > 0) {
		out->value = a->value;
		}
	else {
		out->value = RELU_ALPHA * a->value;
		}

	out->gradient = 0;
	out->children = (Value**)calloc(1, sizeof(Value*));
	out->children[0] = a;
	out->number_of_children = 1;
	out->backward = LeakyRelu_Backward;

	return out;
	}

//This is used for updating a weight in backprop step
void Weights_Update(Value* v, float learning_rate) {
	v->value -= learning_rate * v->gradient;
	}


//-------------------------------------------------//
//-------------- ERROR FUNCTIONS ------------------//

//                                                       ^
//Let s construct mse error function  MSE = 1/N(SUM(Yi - Yi)^2)
Value* Mse_Loss(Value** y_pred, Value** y_true, int size) {

	Value* loss = Value_Init(0.0f);
	for (int i = 0; i < size; i++) {
		Value* diff = Sub(y_pred[i], y_true[i]);
		Value* sq = Power(diff, Value_Init(2.0f));
		loss = Add(loss, sq);
		}
	loss = Divide(loss, Value_Init(size));

	return loss;
	}

//-------------------------------------------------//

void Value_Free(Value* v) {
	if (v->children) {
		free(v->children);
		}
	if(v)
		free(v);
	}



void Values_Vector_Free(Value** v, int size) {

	for(int i = 0; i < size; i++) {
		Value_Free(v[i]);
		}
	//free(v);
	//LOG("Free\n\n");

	}
//-------------------------------------------------//


#endif
