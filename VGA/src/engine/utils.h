#ifndef UTILS_H
#define UTILS_H
#include<stdio.h>

//ERROR UTILS
#define ERROR_EXIT(...)  fprintf(stderror, __VA_ARGS__)
#define ERROR_RETURN(R, ...) fprintf(stderror, __VA_ARGS__); return R;

#endif
