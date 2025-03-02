
#include<stdio.h>
#include<math.h>
#include<stdint.h>
#include<stdlib.h>
#define ARENA_IMPLEMENTATION 
//#define ARENA_DYNAMIC_ARRAY
#include "arena.h"

#define DEBUG_PAUSE(...) {printf(__VA_ARGS__);    int c = getchar();(void)c; };

#define ASSERT(...) {fprintf(stderr, __VA_ARGS__); exit(-1);}

#define ASSERT_TYPE(chain, numOfTypes) \
if(chain->type >= numOfTypes){ \
    ASSERT("We have a ilegal type in FILE->%s on LINE->%d\n", __FILE__, __LINE__); \
}

#define ASSERT_NULL(ptr) \
if(ptr == NULL){ \
    ASSERT("Null ptr in  FILE->%s on LINE->%d\n", __FILE__, __LINE__)\
};


#define CONFIG_MIN_GRANDIENT      -10.0f
#define CONFIG_MAX_GRADIENT        10.0f
#define CONFIG_RELU_ALPHA         -0.01f
#define CONFIG_DEAGULT_GRADIENT    1.0f
#define CONFIG_MAX_NODE            1000 

 

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

const char *nodeTypesStr[] = {
    "Value",
    "LRELU",
    "Add", 
    "Sub",
    "Mult", 
    "Div",
    "Power",
    "shoudNotBePrinted",
}; 

typedef enum{
   
    nodeValue,
    //ONE CHILDREN
    nodeLrelu,
    //TWO CHILDREN
    nodeAdd, 
    nodeSub,
    nodeMult, 
    nodeDiv,
    nodePower,

    numOfTypes,
}NodeTypes;



//Value struct 
typedef struct Node{
    f64 value;
    f64 gradient;
    u64 numberOfChildren;
    struct Node **children;
    //void (*backward)(struct Node*);
    NodeTypes type;
    
}Node;

typedef struct {
    void (*Backward)(Node *v);
}BackwardFunc;

void dumyNode(Node *v); //DUMY FUNCTION 
void lReluBackward(Node *v);

void addBackward(Node *v);
void subBackward(Node *v);
void multBackward(Node *v);
void divBackward(Node *v);
void powerBackward(Node *v);

BackwardFunc globalBackpropFunction[] = {
    {.Backward = &dumyNode},
    {.Backward = &lReluBackward},
    {.Backward = &addBackward},
    {.Backward = &subBackward},
    {.Backward = &multBackward},
    {.Backward = &divBackward},
    {.Backward = &powerBackward},
};



//Maybe type so then we call a function thru type
Node *nodeInit(f64 val, Arena *arena){
    Node *temp = arena_alloc(arena, sizeof(Node)*1+1);
    ASSERT_NULL(temp);
    temp->value = val;
    temp->gradient = 0.0f;
    temp->numberOfChildren = 0;
    temp->children = NULL;
    temp->type = nodeValue;
    return temp;
}

#define NODE_STR(chain) nodeTypesStr[chain->type]

void nodePrint(Node *chain){
    ASSERT_TYPE(chain, numOfTypes);
         
    printf("Node:{\n\t type->%s, value->%f gradient->%f", 
          NODE_STR(chain), chain->value, chain->gradient);
   
    if(chain->numberOfChildren != 0){
        //printf("\n------------CHILDREN--------------\n");
        printf("\n");
        for (size_t i = 0; i < chain->numberOfChildren; i++){
            nodePrint(chain->children[i]);
        }
       printf("}\n");
    }
    
}

Node *nodeOneOperands(Node *A, NodeTypes type, Arena *arena){
    Node *temp = arena_alloc(arena, sizeof(Node)+1);
    ASSERT_NULL(temp);
    temp->type  = type;
    temp->numberOfChildren = 1;
    temp->children = arena_alloc(arena, 1*sizeof(Node*)+1);
    ASSERT_NULL(temp->children);
    temp->children[0] = A;
    return temp;
}

#define NODE_LRELU(A, arena) nodeOneOperands(A, nodeLrelu, arena)

Node* nodeTwoOperands(Node *A, Node *B, NodeTypes type, Arena *arena){
    Node *temp = arena_alloc(arena, sizeof(Node)+1);
    ASSERT_NULL(temp);
    temp->type  = type;
    temp->numberOfChildren = 2;
    temp->children = arena_alloc(arena, 2*sizeof(Node*)+1);
    ASSERT_NULL(temp->children);
    temp->children[0] = A;
    temp->children[1] = B;
    return temp;
}

#define NODE_ADD(A, B, arena)    nodeTwoOperands(A, B, nodeAdd,   arena)
#define NODE_SUB(A, B, arena)    nodeTwoOperands(A, B, nodeSub,   arena)
#define NODE_MULT(A, B, arena)   nodeTwoOperands(A, B, nodeMult,  arena)
#define NODE_DIV(A, B, arena)    nodeTwoOperands(A, B, nodeDiv,   arena)
#define NODE_POWER(A, B, arena)  nodeTwoOperands(A, B, nodePower, arena)

//BACKPROP
void limitGradient(Node *v, float min, float max) {

	if(v->gradient > max) {
		v->gradient = max;
		}
	else if(v->gradient < min) {
		v->gradient = min;
		}
	else return;
	}


void dumyNode(Node *v){
    
    ASSERT("We have a ilegal Node in backprop FILE->%s on LINE->%d\n", __FILE__, __LINE__);
    //v = NULL;
    nodePrint(v);
}
//Actiavation operation LeakyRelu v = LeakyRelue(a) => dv/da = 1 if(a > 0) dv/da = 1 else dv/da=-RELU_ALPHA
void lReluBackward(Node *v) {
	if (v->children[0]->value > 0) {
		v->children[0]->gradient += v->gradient;
		}
	else {
		v->children[0]->gradient += v->gradient * CONFIG_RELU_ALPHA;
		}
	limitGradient(v->children[0], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	
	}

void addBackward(Node *v) {
	v->children[0]->gradient += v->gradient;
	v->children[1]->gradient += v->gradient;
	limitGradient(v->children[0], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	limitGradient(v->children[1], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	}
//Scalar - operation v = a - b => bv/db = -1 ^ dv/da = 1
void subBackward(Node *v) {
	v->children[0]->gradient += v->gradient;
	v->children[1]->gradient += -v->gradient;
	limitGradient(v->children[0], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	limitGradient(v->children[1], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	}



//Scalar * operation v= a*b => dv/da = b ^ dv/db  = a
void multBackward(Node *v) {
	v->children[0]->gradient += v->children[1]->value * v->gradient;
	v->children[1]->gradient += v->children[0]->value * v->gradient;
	limitGradient(v->children[0], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	limitGradient(v->children[1], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	}

//Scalar / operation v = a / b => dv / da = (1/b) ^ dv/db = a*(-1/(b*b))
void divBackward(Node *v) {

	v->children[0]->gradient += (1 / v->children[1]->value) * v->gradient;
	v->children[1]->gradient += 
    (-v->children[0]->value / (v->children[1]->value * v->children[1]->value)) * v->gradient;
    limitGradient(v->children[0], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
    limitGradient(v->children[1], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	}


//Scalar operation ^(exponent) y = a^b => dv/da =  b * a^(b-1) ^ dv/db = a^b * log(a) = v * log(a)
void powerBackward(Node* v) {
	v->children[0]->gradient += (v->children[1]->value * pow(v->children[0]->value, v->children[1]->value - 1))
	                            * v->gradient;
	if (v->children[0]->value > 0) {  // Ensure base is positive before computing log
		v->children[1]->gradient += 
        (log(v->children[0]->value) * pow(v->children[0]->value, v->children[1]->value)) * v->gradient;
		}
    limitGradient(v->children[0], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	limitGradient(v->children[1], CONFIG_MIN_GRANDIENT, CONFIG_MAX_GRADIENT);
	}




void buildTopological(Node *chain, Node **topo, 
    u64* topological_size, Node **visited, u64 *visited_size) {
	//Escape condition
	for(u64 i = 0; i < *visited_size; i++) {
		if(visited[i] == chain) return;
		}
	//If we are not exit
	//We are placed inside of a visited graph
	visited[*visited_size] = chain;
	(*visited_size)++;
	//Go Thrue all children in a graph and then construct top
	for(u64 i = 0; i < chain->numberOfChildren; i++) {
		buildTopological(chain->children[i], topo, topological_size, visited, visited_size);
		}
	topo[*topological_size] = chain;
	(*topological_size)++;

	}

void chainBackward(Node *chain, Arena *tempArena) {
	//Maybe just one we see

	Node **topo = arena_alloc(tempArena, CONFIG_MAX_NODE * sizeof(Node*));
	u64 topological_size = 0;
	Node **visited= arena_alloc(tempArena, CONFIG_MAX_NODE * sizeof(Node*));
	u64 visited_size = 0;
	buildTopological(chain, topo, &topological_size, visited, &visited_size);
    chain->gradient = CONFIG_DEAGULT_GRADIENT;
	for (int i = topological_size - 1; i >= 0; --i) {
		// printf("%.2f", topo[i]->val);
		// printf("\n");
        //nodePrint(topo[i]);
		if (topo[i]->type != nodeValue) {
			globalBackpropFunction[topo[i]->type].Backward(topo[i]);
			}
		}
      arena_reset(tempArena);
	}

typedef struct  {
    Node **w;
    Node  *b;
    u64  numIn;
    u8   actType; //Activation type for now we only have a lRELU or lin  
}Perceptron;

typedef struct  {
    Perceptron **neuron;
    u64 numberOfLayer;
}MultiLayerPerceptron;





Arena mainArena = {0};
Arena tempArena = {0};
int main(){
    Node *value = nodeInit(0.123, &mainArena);
    nodePrint(value);
    Node *A = nodeInit(1.0f, &mainArena);
    Node *B = nodeInit(0.5f, &mainArena);
    Node *chain = NODE_ADD(
                            NODE_MULT(A, B, &mainArena), NODE_MULT(B, A, &mainArena), 
                &mainArena);
   // nodePrint(chain);
   //DEBUG_PAUSE("\nStart:\n");
    //for (int i = 0; i < 10000; i++){
      //  chainBackward(chain, &tempArena);
       
    //}
    //DEBUG_PAUSE("\nend\n");
    nodePrint(chain);  
    chainBackward(chain, &tempArena);
    printf("After Backprop\n");
    nodePrint(chain);
    DEBUG_PAUSE("\nStoped");
    //printf("Hello World!!!\n");
    return 0;

}
