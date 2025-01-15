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
	
	NK_BOOL,
	NK_GRATER,
	NK_IF,

	NK_ADD,
	NK_MULT,
	NK_MOD, 

	NK_TRIPLE,
}Node_Kind;

typedef struct {
	Node *lhs, *rhs;
}Node_Binop;

typedef struct {
	Node *first, *second, *thirth;
}Node_Triple;

typedef struct {
    Node *cond;
    Node *then;
    Node *elze;
} Node_If;

typedef union {
	float number;
	bool isTrue;
	Node_Binop binop;
	Node_If iff;
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
	node->as.number = number;
	return node;
}
#define node_number(number) node_number_loc(__FILE__, __LINE__, number)

Node *node_boolean_loc(const char *file, int line, bool isTrue)
{
    Node *node = node_loc(file, line, NK_BOOL);
    node->as.isTrue = isTrue;
    return node;
}
#define node_boolean(isTrue) node_boolean_loc(__FILE__, __LINE__, isTrue)


Node *node_gt_loc(char *file, int line, Node* lhs, Node *rhs){
	Node *node = node_loc(file, line, NK_GRATER);
	node->as.binop.lhs = lhs;
	node->as.binop.rhs = rhs;
	return node;
}
#define node_gt(lhs, rhs) node_gt_loc(__FILE__, __LINE__, lhs, rhs)




Node *node_if_loc(const char *file, int line, Node *cond, Node *then, Node *elze)
{
    Node *node = node_loc(file, line, NK_IF);
    node->as.iff.cond = cond;
    node->as.iff.then = then;
    node->as.iff.elze = elze;
    return node;
}
#define node_if(cond, then, elze) node_if_loc(__FILE__, __LINE__, cond, then, elze)



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

Node *node_mod_loc(char *file, int line, Node* lhs, Node *rhs){
	Node *node = node_loc(file, line, NK_MOD);
	node->as.binop.lhs = lhs;
	node->as.binop.rhs = rhs;
	return node;
}
#define node_mod(lhs, rhs) node_mod_loc(__FILE__, __LINE__, lhs, rhs)



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
	//INPUT OPS
	case NK_X:
		printf("X");
		break;
	case NK_Y:
		printf("Y");
		break;
	case NK_NUMBER:
		printf("%f", node->as.number);
		break;
	//LOGIC AND CONDITION OPS MAYBE LOOPS
	case NK_BOOL:
		printf("%s", node->as.isTrue ? "True" : "False" );
		break;
	
	case NK_GRATER:
		printf("grater(");
		node_print(node->as.binop.lhs);
		printf(", ");
		node_print(node->as.binop.rhs);
		printf(")");
		break;

	case NK_IF:
        printf("if ");
        node_print(node->as.iff.cond);
        printf(" then ");
        node_print(node->as.iff.then);
        printf(" else ");
        node_print(node->as.iff.elze);
        break;
	


	//MATH OPS
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
	case NK_MOD:
		printf("mod(");
		node_print(node->as.binop.lhs);
		printf(", ");
		node_print(node->as.binop.rhs);
		printf(")");
		break;
	//Color
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
	//printf("\n");
}
#define node_print_ln(node) node_print(node); printf("\n");

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

bool expect_number(Node *expr){
	if(expr->kind != NK_NUMBER){
			printf("Error: %s %d kind must be a number\n", expr->file, expr->line);
			return false;
		}
	return true;
}

bool expect_triple(Node *expr){
	if(expr->kind != NK_TRIPLE){
			printf("Error: %s %d kind must be a triple\n", expr->file, expr->line);
			return false;
		}
	return true;
}
bool expect_boolean(Node* expr){
	if(expr->kind != NK_BOOL){
			printf("Error: %s %d kind must be a bool\n", expr->file, expr->line);
			return false;
		}
	return true;
}



Node* eval(Node *expr, float x, float y){
	switch (expr->kind){
	//INPUT 
	case NK_X:
		return node_number_loc(expr->file, expr->line, x);
	case NK_Y:
		return node_number_loc(expr->file, expr->line, y);
	case NK_NUMBER:
		return expr;
	
	//bool
	case NK_BOOL:
		return expr;
	
	case NK_GRATER: {
        Node *lhs = eval(expr->as.binop.lhs, x, y);
        if(!expect_number(lhs)) return NULL;
        //if (!expect_number(lhs)) return NULL;
        Node *rhs = eval(expr->as.binop.rhs, x, y);
        if(!expect_number(rhs)) return NULL;
        //i//f (!expect_number(rhs)) return NULL;
        return node_boolean_loc(expr->file, expr->line, lhs->as.number > rhs->as.number);
    }

	 case NK_IF: {
        Node *cond = eval(expr->as.iff.cond, x, y);
        if (!cond) return NULL;
        if (!expect_boolean(cond)) return NULL;
        Node *then = eval(expr->as.iff.then, x, y);
        if (!then) return NULL;
        Node *elze = eval(expr->as.iff.elze, x, y);
        if (!elze) return NULL;
        return cond->as.isTrue ? then : elze;
    }

	//math
	case NK_ADD:{
		Node *lhs = eval(expr->as.binop.lhs, x, y);	
		if(!expect_number(lhs)) return NULL; 
		Node *rhs = eval(expr->as.binop.rhs, x, y);
		if(!expect_number(rhs)) return NULL;
			return node_number_loc(expr->file, expr->line, lhs->as.number + rhs->as.number);
		}
	case NK_MULT:{
		Node *lhs = eval(expr->as.binop.lhs, x, y);
		if(!expect_number(lhs)) return NULL;
		Node *rhs = eval(expr->as.binop.rhs, x, y);
		if(!expect_number(rhs)) return NULL;
		        return node_num_loc(expr->file, expr->line, lhs->as.number > rhs->as.number);
		}
	case NK_MOD:{
		Node *lhs = eval(expr->as.binop.lhs, x, y);
		if(!expect_number(lhs)) return NULL;
		Node *rhs = eval(expr->as.binop.rhs, x, y);
		if(!expect_number(rhs)) return NULL;
		return node_number_loc(expr->file, expr->line, 
			   fmod(lhs->as.number, rhs->as.number));
		}
	


	//Color
	case NK_TRIPLE:
		Node *first  = eval(expr->as.triple.first,  x, y);
		Node *second = eval(expr->as.triple.second, x, y);
		Node *thirth = eval(expr->as.triple.thirth, x, y);
		return node_triple_loc(expr->file, expr->line, first, second, thirth);
	default:
		//break;
		printf("Error: %s %d unrecahabl\n", expr->file, expr->line);
		return NULL;
}
}
bool eval_func(Node *body, float x, float y, Color *c){
	Node *result = eval(body, x, y);
	if(result == NULL) return false;
	if(!expect_triple(result)) return false;
	if(!expect_number(result->as.triple.first))  return false;
	if(!expect_number(result->as.triple.second)) return false;
	if(!expect_number(result->as.triple.thirth)) return false;
	
	c->r = result->as.triple.first->as.number;
	c->g = result->as.triple.second->as.number;
	c->b = result->as.triple.thirth->as.number;
	return true;
}

//void render_pixels(Color (*f)(float x, float y)){
bool render_pixels(Node *node){
	for (size_t y = 0; y < HEIGHT; y++){
		float yn = y / (float)HEIGHT * 2.0f - 1; //(0, HEIGHT) -> (-1, 1)
		for (size_t x = 0; x < WIDTH; x++){
			float xn = x / (float)WIDTH * 2.0f - 1; //(0, WIDTH) -> (-1, 1)
			Color c;
			if(!eval_func(node, xn, yn, &c)) return false;//eval(node, xn, yn);
			//bool ok = eval_func(node, xn, yn, &c);
			//if(!ok) printf("false\n");
			size_t index = WIDTH * y + x;
			pixels[index].r = (uint8_t)((c.r + 1)/2.0f * 255); 
			pixels[index].b = (uint8_t)((c.b + 1)/2.0f * 255);
			pixels[index].g = (uint8_t)((c.g + 1)/2.0f * 255);
			pixels[index].a = 255;
		}
	}
	return true;
}


int main(int argc, char *argv){
	//printf("Nesto\n\n");
	//Node *node = arena_alloc(&node_arena, sizeof(Node)); 
	//Node *node = node_triple(node_x(), node_y(), node_number(1));
	//Node *node = node_triple(
//		node_add(node_x(), node_y()),
//		node_y(),
//		node_number(0.001)
//	);
Node *node = 
	   node_if(
             node_gt(
				node_mult(node_x(), node_y()), 
				node_number(0)
				),
             node_triple(
                 node_x(),
                 node_y(),
                 node_number(1)),
             node_triple(
                 node_mod(node_x(), node_y()),
                 node_mod(node_x(), node_y()),
                 node_mod(node_x(), node_y())));
	node_print_ln(node);
	//node_print_ln(eval(node, 1, 1.4));
	//bool ok = render_pixels(node_triple(node_x(), node_x(), node_x()));
	//return 0;
	bool ok = render_pixels(node);
	if(!ok) return -1;
	stbi_write_png("nesto.png", WIDTH, HEIGHT, 4, pixels, sizeof(RGBA) * WIDTH);
	//printf("\n");
}
