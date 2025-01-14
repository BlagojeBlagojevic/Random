#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
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
	const char *file;
	int line;
}Node;

Node *node_loc(const char* file, int line, Node_Kind kind){
	
	Node *node = arena_alloc(&node_arena, sizeof(Node));
	node->kind = kind;
	node->file = file;
	node->line = line;
	return node;
}

Node *node_number_loc(const char *file, int line, float number){
	Node *node = node_loc(__FILE__, __LINE__, NK_NUMBER);
	return node;
}
#define node_number(number) node_number_loc(__FILE__, __LINE__, number)

Node *node_x_loc(const char *file, int line){
	Node *node = node_loc(file, line, NK_X);
	return node;
}
#define node_x() node_x_loc(__FILE__, __LINE__)


Node *node_y_loc(const char* file, int line){
	Node *node = node_loc(file, line, NK_Y);
	return node;
}
#define node_y() node_y_loc(__FILE__, __LINE__)

Node *node_add_loc(char *file, int line, Node* lhs, Node *rhs){
	Node *node = node_loc(file, line, NK_ADD);
	node->as.binop.lhs = lhs;
	node->as.binop.rhs = rhs;
	return node;
}
#define node_add(lhs, rhs) node_add_loc(__FILE__, __LINE__, lhs, rhs)


Node *node_mult_loc(char *file, int line, Node* lhs, Node *rhs){
	Node *node = node_loc(file, line, NK_MULT);
	node->as.binop.lhs = lhs;
	node->as.binop.rhs = rhs;
	return node;
}
#define node_mult(lhs, rhs) node_mult_loc(__FILE__, __LINE__, lhs, rhs)


Node *node_triple_loc(char* file, int line, Node* a, Node *b, Node *c){
	Node *node = node_loc(file, line, NK_TRIPLE);
	node->as.triple.first  = a;
	node->as.triple.second = b;
	node->as.triple.thirth = c;
	return node;
}
#define node_triple(a, b, c) node_triple_loc(__FILE__, __LINE__, a, b, c)


void node_print(Node *node){
	switch (node->kind)
	{
	case NK_X:
		printf("X");
		break;
	case NK_Y:
		printf("Y");
		break;
	case NK_NUMBER:
		printf("%f", node->as.number);
		break;
	case NK_ADD:
		printf("add(");
		node_print(node->as.binop.lhs);
		printf(", ");
		node_print(node->as.binop.rhs);
		printf(")");
		break;
	case NK_MULT:
		printf("mult(");
		node_print(node->as.binop.lhs);
		printf(", ");
		node_print(node->as.binop.rhs);
		printf(")");
		break;
	case NK_TRIPLE:
		printf("(");
		node_print(node->as.triple.first);
		printf(", ");
		node_print(node->as.triple.second);
		printf(", ");
		node_print(node->as.triple.thirth);
		printf(")");
		break;
	default:
		break;
	}
	
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


Node* eval(Node *expr, float x, float y){
		switch (expr->kind)
	{
	case NK_X:
		return node_x_loc(expr->file, expr->line);
	case NK_Y:
		return node_y_loc(expr->file, expr->line);
	case NK_NUMBER:
		return expr;
	case NK_ADD:{
		Node *lhs = eval(expr->as.binop.lhs, x, y);
		if(lhs->kind != NK_NUMBER){
			printf("Error: %s %d lhs must be a num\n", expr->file, expr->line);
			return NULL;
		}
		Node *rhs = eval(expr->as.binop.rhs, x, y);
		if(rhs->kind != NK_NUMBER){
			printf("Error: %s %d rhs must be a num\n", expr->file, expr->line);
			return NULL;
		}
		return node_number_loc(expr->file, expr->line, lhs->as.number + rhs->as.number);
		}
	case NK_MULT:{
		Node *lhs = eval(expr->as.binop.lhs, x, y);
		if(lhs->kind != NK_NUMBER){
			printf("Error: %s %d lhs must be a num\n", expr->file, expr->line);
			return NULL;
		}
		Node *rhs = eval(expr->as.binop.rhs, x, y);
		if(rhs->kind != NK_NUMBER){
			printf("Error: %s %d rhs must be a num\n", expr->file, expr->line);
			return NULL;
		}
		return node_number_loc(expr->file, expr->line, lhs->as.number * rhs->as.number);
		}
	case NK_TRIPLE:
		Node *first  = eval(expr->as.triple.first, x, y);
		Node *second = eval(expr->as.triple.second, x, y);
		Node *thirth = eval(expr->as.triple.thirth, x, y);
		return node_triple_loc(expr->file, expr->line, first, second, thirth);
	default:
		//break;
		printf("Error: %s %d rhs unrecahabl\n", expr->file, expr->line);
}
}
bool eval_func(Node *body, float x, float y, Color *c){
	Node *result = eval(body, x, y);
	if(result == NULL) return false;
	if(result->kind != NK_TRIPLE){
		printf("Error: %s %d must return triple\n", result->file, result->line);
		return false;
	} 
	//c->r = result->as.triple.first;
	//c->g = result->as.triple.second;
	//c->b = result->as.triple.thirth;
	
}

//void render_pixels(Color (*f)(float x, float y)){
void render_pixels(Node *node){
	for (size_t y = 0; y < HEIGHT; y++){
		float yn = y / (float)HEIGHT * 2.0f - 1; //(0, HEIGHT) -> (-1, 1)
		for (size_t x = 0; x < WIDTH; x++){
			float xn = x / (float)WIDTH * 2.0f - 1; //(0, WIDTH) -> (-1, 1)
			Color c = gray_grad(xn, yn);//eval(node, xn, yn);
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
	//Node *node = arena_alloc(&node_arena, sizeof(Node)); 
	Node *node = node_triple(node_x(), node_y(), node_number(1));
	node_print(node);
	render_pixels(gib);
	stbi_write_png("nesto.png", WIDTH, HEIGHT, 4, pixels, sizeof(RGBA) * WIDTH);
	printf("\n");
}
