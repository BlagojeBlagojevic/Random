#include<stdio.h>
#include<stdlib.h>
// #define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include<stdint.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"
static Arena node_arena = {0};


#define WIDTH 400
#define HEIGHT 400


typedef struct {
	uint8_t r, g, b, a;
}RGBA;


static RGBA pixels[WIDTH * HEIGHT];

typedef struct Vector2{
	float x, y;
}Vector2;

typedef struct Vector3{
	float r, g, b;
}Color;



//typedef Vector3  Color;

typedef struct Node Node;

typedef enum{
	NK_X,
	NK_Y,
	NK_NUMBER,
	NK_ADD,
	NK_MULT,
	NK_TRIPLE,
}Node_Kind;

typedef struct {
	Node *lhs, *rhs;
}Node_Binop;

typedef struct {
	Node *first, *second, *thirth;
}Node_Triple;

typedef union {
	float number;
	Node_Binop binop;
	Node_Triple triple;
}Node_As;


typedef struct Node{
	Node_Kind kind;
	Node_As as;
}Node;

Node *node_number(float number){
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = NK_NUMBER;
	node->as.number = number;
	return node;
}

Node *node_x(void){
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = NK_X;
	return node;
}

Node *node_y(void){
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = NK_Y;
	return node;
}

Node *node_add(Node* lhs, Node *rhs){
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = NK_ADD;
	node->as.binop.lhs = lhs;
	node->as.binop.rhs = rhs;
	return node;
}

Node *node_mult(Node* lhs, Node *rhs){
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = NK_MULT;
	node->as.binop.lhs = lhs;
	node->as.binop.rhs = rhs;
	return node;
}

Node *node_triple(Node* a, Node *b, Node *c){
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = NK_TRIPLE;
	node->as.triple.first  = a;
	node->as.triple.second = b;
	node->as.triple.thirth = c;
	return node;
}

Color gray_grad(float x, float y){
	//return (Color){pos.x, pos.x,pos.x,};
	return (Color){x, x, x,};
}

Color gib(float x, float y){
	if(x * y > 0)
		return (Color){x, y, 1};
	float r = fmodf(x, y);
	return (Color){r, r, r};
}


void render_pixels(Color (*f)(float x, float y)){
	for (size_t y = 0; y < HEIGHT; y++){
		float yn = y / (float)HEIGHT * 2.0f - 1; //(0, HEIGHT) -> (-1, 1)
		for (size_t x = 0; x < WIDTH; x++){
			float xn = x / (float)WIDTH * 2.0f - 1; //(0, WIDTH) -> (-1, 1)
			Color c = f(xn, yn);
			size_t index = WIDTH * y + x;
			pixels[index].r = (uint8_t)((c.r + 1)/2.0f * 255); 
			pixels[index].b = (uint8_t)((c.b + 1)/2.0f * 255);
			pixels[index].g = (uint8_t)((c.g + 1)/2.0f * 255);
			pixels[index].a = 255;
		}
	}
}


int main(int argc, char *argv){
	//printf("Nesto\n\n");
	Node *node = arena_alloc(&node_arena, sizeof(Node)); 
	render_pixels(gib);
	stbi_write_png("nesto.png", WIDTH, HEIGHT, 4, pixels, sizeof(RGBA) * WIDTH);
}