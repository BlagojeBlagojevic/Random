#ifndef UTILS_H
#define UTILS_H
//TYPE UTILS
#include<stdint.h>

typedef  uint8_t  u8;
typedef  uint16_t u16;
typedef  uint32_t u32;
typedef  uint64_t u64;
typedef  int16_t  i16;
typedef  int32_t  i32;
typedef  int64_t  i64;
typedef  float    f32;
typedef  double   f64;

//BOOL
enum {FALSE, TRUE};


//ERROR HANDLING AND LOGING SYSTEM WILL DEPEND OF STANDDARD LIB
#include<stdio.h>

#define ERROR(...)        fprintf(stderr, __VA_ARGS__)
#define ERROR_BREAK(...)  fprintf(stderr, __VA_ARGS__); exit(-1)
#define LOG(...)     			fprintf(stdout, __VA_ARGS__) 



#include <string.h> 
//FOR MEMCPY 
//toDo WRITE MEMCPY

#endif