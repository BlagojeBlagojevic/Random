#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// We'll have 128 tokens. Each token can be up to 32 characters long.
char token[128][32];

int lexer(char* input) {
  int ii = 0; // input index
  int ti = 0; // token index

  // Loop thru the whole string
  while(input[ii] != '\0')
    switch(input[ii]) {
    // Ignore whitespace and newlines
    case ' ':
    case '\n':
      ++ii;
      break;

    // Turn a left parenthesis into a token.
    case '(':
      token[ti][0] = '(';
      token[ti][1] = '\0';
      ++ii;
      ++ti;
      break;

    // Turn a right parenthesis into a token.
    case ')':
      token[ti][0] = ')';
      token[ti][1] = '\0';
      ++ii;
      ++ti;
      break;

    // Anything else is a symbol
    default:
      for(int i = 0;; ++i) {
	if(input[ii] != ' '  &&
	   input[ii] != ')'  &&
	   input[ii] != '('  &&
	   input[ii] != '\n' &&
	   input[ii] != '\0') {
	  token[ti][i] = input[ii++];
	}
        else {
	  token[ti][i] = '\0';
	  break;
	}
      }
      ++ti;
      break;
    }
  return ti;
}


int curtok;

char* nexttok() {
  return token[curtok++];
}

char* peektok() {
  return token[curtok];
}


typedef struct {
  void* car;
  void* cdr;
} Pair;

Pair text[256];
Pair* textptr;

Pair* cons(void* x, void* y) {
  textptr->car = x;
  textptr->cdr = y;
  return textptr++;
}

int ispair(void* x) {
  return x >= (void*)&text &&
         x <  (void*)&text[256];
}


void* read_list();
void* read_exp();
//void* read_

void* read(char* ln) {
  // Initialize the lexer and list memory.
  curtok = 0;
  textptr = text;

  lexer(ln);
  return read_exp();
}



void* read_exp() {
  char* tok = nexttok();
  if(tok[0] == '(')
    return read_list();
  else
    return tok;
}

void* read_list() {
  char* tok = peektok();
  if(tok[0] == ')') {
    nexttok();
    return NULL;
  }
  else {
    void* fst = read_exp();
    void* snd = read_list();
    return cons(fst, snd);
  }
}

void print(void* exp) {
  print_exp(exp);
  printf("\n");
}

void print_exp(void* exp) {
  if(ispair(exp)) {
    printf("(");
    print_list(exp);
  }
  else
    printf("%s", exp);
}

void print_list(Pair* list) {
  if(list->cdr == NULL) {
    print_exp(list->car);
    printf(")");
  }
  else {
    print_exp(list->car);
    printf(" ");
    print_list(list->cdr);
  }
}


void* eval(void* exp) {
  return exp;
}


int main(int argc, char** argv) {
  printf("Lisp REPL\n\n");
  printf(">> ");

  char buffer[256];
  while(fgets(buffer, 256, stdin)) {
    print(eval(read(buffer)));
    printf(">> ");
  }
  return 0;
}
