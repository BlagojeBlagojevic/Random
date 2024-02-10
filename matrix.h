#ifndef MATRIX_H
#define MATRIX_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdint.h>

#ifndef MATRIX_CALLOC
#include<stdlib.h> 
#define MATRIX_CALLOC calloc
#endif

#ifndef MATRIX_ASSERT
#include<assert.h> 
#define MATRIX_ASSERT assert
#endif

#define MATRIX_SHIFT(m,y,x) (m).elem[(y)*(m).cols + (x)]

#ifndef TYPE 
#define TYPE float
#endif

typedef struct {
	size_t rows;    //NUM OF   ROWS
	size_t cols;    //NUM OF   COLS
	size_t stride;  //WHER TO  SPLIT
	TYPE *elem;    //ELEMETS  IN
	} Mat;

//UTILITY FUNCTION
float rand_float(void);

//MATRIX OPERATION
Mat matrix_alloc(size_t rows, size_t cols);
void matrix_dot(Mat dest, Mat a, Mat b);
void matrix_sum(Mat dest, Mat a);
void matrix_rand(Mat dest,float low, float high);
void matrix_print(Mat m,const char *name);
#define MATRIX_PRINT(m) matrix_print(m, #m)

#endif  //MATRIX_H


#ifdef MATRIX_IMPLEMETATION
float rand_float(void){
	return (float)rand() / (float)RAND_MAX;
}

Mat matrix_alloc(size_t rows, size_t cols)
{
	Mat m;
	m.cols = cols;
	m.rows = rows; 
	m.elem = (TYPE *)MATRIX_CALLOC(rows*cols,sizeof(*m.elem));    //(*M.elem) derefrence Changing type of element
	MATRIX_ASSERT(m.elem!=NULL);
	return m;
}
void matrix_dot(Mat dest, Mat a, Mat b)  //a[1x2] b[2x3] c[1x3]
{
    MATRIX_ASSERT(a.cols == b.rows);
	MATRIX_ASSERT(dest.rows == a.rows);
	MATRIX_ASSERT(dest.cols == b.cols);
	for(size_t y = 0; y < dest.rows; y++){
		for(size_t x = 0; x < dest.rows; x++){
		    MATRIX_SHIFT(dest,y,x) = 0;
			for(size_t i = 0;i < a.cols; i++){
				MATRIX_SHIFT(dest,y,x) += MATRIX_SHIFT(a,y,i) * MATRIX_SHIFT(b,i,x);
			}
			
		}
	}
	
}
void matrix_sum(Mat dest, Mat a)
{
	MATRIX_ASSERT(dest.cols == a.cols);
	MATRIX_ASSERT(dest.rows == a.rows);
	for(size_t y = 0; y < dest.rows;y++){
		for(size_t x = 0; x < dest.rows;x++){
	     
		 MATRIX_SHIFT(dest,y,x) += MATRIX_SHIFT(a,y,x);  
		}
	}
	
}
void matrix_rand(Mat dest, float high,float low)
{
	for(size_t y = 0;y < dest.rows; y++){
		for(size_t x = 0;x < dest.cols; x++){
          MATRIX_SHIFT(dest,y,x) = (TYPE)((rand_float()*(high - low)) + low);   //IN RANGE
		}
}
}

void matrix_print(Mat m, const char *name)
{
	printf("\n\n______________________________________________________________________________\n");
	printf("%s",name);
	printf(" = \n[\n");
	for(size_t y = 0;y < m.rows; y++){
		for(size_t x = 0;x < m.cols; x++){
		   printf(" %.2f ", (float)MATRIX_SHIFT(m,y,x));
		}
		printf("\n\n");
	}
	printf("]\n");
	printf("______________________________________________________________________________\n");
	
}



#endif  //MATRIX_IMPLEMENTATION
