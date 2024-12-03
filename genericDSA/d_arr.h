#include<stdio.h>
#include<stdlib.h>

#define ERROR(...)  {fprintf(stderr, __VA_ARGS__); exit(-1);}
#define DEAFULT_ALLOC_SIZE 1024

#define DA_ALLOC malloc
#define DA_REALLOC realloc

#define DA_STRUCT(type)\
	typedef struct{\
		size_t size;\
		size_t count;\
		type* elements;\
		}dArr_##type;

#define DA_CREATE(type)\
	static dArr_##type dArrCreate_##type(size_t size){\
		dArr_##type temp;\
		temp.size = size; temp.count = 0;\
		temp.elements = DA_ALLOC(sizeof(type) * size);\
		if(temp.elements == NULL) {ERROR("Allocating dArr failed !!!");}\
		return temp;\
		}

#define DA_INSERT(type)\
	static void dArrInsert_##type(type elem, dArr_##type *dArr){\
		if(dArr->size == dArr->count){\
			type *ptr = DA_REALLOC(dArr->elements, (dArr->size * 2)*sizeof(type));\
			dArr->size*=2;\
			if(ptr == NULL){ERROR("We have not able to realoc a arr !!!\n");}\
			else{dArr->elements = ptr;}\
			}\
		dArr->elements[dArr->count++] = elem;\
		}

//PROVIDE YOUR OWN IMPLEMENTATION format is a for exmple  "%d", "%d\n", dArr->elements[count]
#define DA_PRINT(type, ...)\
	static void dArrPrint_##type(dArr_##type *dArr){\
		for(size_t count = 0; count < dArr->count; count++){\
			printf(__VA_ARGS__);\
			}\
		}

#define DA_LOAD_FROM_FILE(type)\
	static dArr_##type dArrLoadFromFile_##type(const char *nameFile){\
	dArr_##type temp = dArrCreate_##type(1000000+1);\
		FILE *f = fopen(nameFile, "r");\
		if(f == NULL)\
			ERROR("Error in a file\n");\
		fread(temp.elements, sizeof(type), 1000000, f);\
		temp.count = 1000000;\
		fclose(f);\
		return temp;\
	}

#define DA_FREE()\
	static void dArrFree(void* ptr){if(ptr != NULL) free(ptr);}
