#ifndef MATRIX_H
#define MATRIX_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdint.h>
#include<string.h>

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

#ifndef MATRIX_ACTIVATION_DEFAULT_DISABLE
#define MATRIX_ACTIVATION_SIGMOID
#endif

#ifndef T
#define T 1
#endif
#include<omp.h>

typedef struct {
	size_t rows;    //NUM OF   ROWS
	size_t cols;    //NUM OF   COLS
	size_t stride;  //WHER TO  SPLIT
	TYPE  *__restrict__ elem;    //ELEMETS  IN
	} Mat;

//UTILITY FUNCTION
static inline float rand_float(void);
static inline float sigmoid(float x);
// In MATRIX_IMPLEMETATION block
static inline float sigmoid_derivative(float x);

//MATRIX OPERATION
Mat matrix_alloc(size_t rows, size_t cols);
static inline void  matrix_free(Mat m);
static inline void  matrix_init(Mat m, double *arr);

static inline void  matrix_dot(Mat dest, Mat a, Mat b);
static inline void  matrix_outer(Mat dest, Mat a, Mat b);
static inline void  matrix_distance(Mat dest, Mat a, Mat b);
static inline void  matrix_sum(Mat dest, Mat a);
static inline void  matrix_dec(Mat dest, Mat a);
static inline void  matrix_mul_scalar(Mat dest, float scalar);
static inline void  matrix_add_scalar(Mat dest, float scalar);
static inline void  matrix_rand(Mat dest,float low, float high);
static inline void  matrix_eye(Mat dest);
static inline void  matrix_activation(Mat m);
static inline void  matrix_activation_cols(Mat m,size_t num);
static inline void  matrix_feedforward(Mat *out,Mat input, Mat *weights, Mat *bias, size_t num_of_layer);
static inline void  matrix_backpropagate(Mat input, Mat target, Mat *activations, Mat *weights, Mat *biases, size_t num_of_layer);
static inline void  matrix_reproduce(Mat a, Mat b);
static inline void  matrix_softmax(Mat m);
static inline void  matrix_copy(Mat dest, Mat src);
static inline void  matrix_convolution(Mat dest,Mat a, Mat kernel);



static inline void  matrix_print(Mat m,const char *name);
static inline void  matrix_print_out(Mat m, const char *name, size_t out);

static inline float matrix_max_element(Mat m);
#define MATRIX_PRINT(m) matrix_print((m), #m)

#ifndef LR
#define LR 0.6
#endif

#ifndef MUTATION_RATE
#define MUTATION_RATE 0.9
#endif


static inline void matrix_mutation(Mat m);
static inline void matrix_mutation_enable(Mat m, float DM);
static inline void matrix_mutation_disable(Mat m,float EM);


//EARTH MOVER DISTANCE (waserstaine distances) optimal way to move a distribution to onother
//KINDA MIN WORK NES 
static inline float matrix_EMD(Mat dest, Mat p, Mat q, Mat cost);

//STB IMAGE STUFF
#ifdef STB_IMAGE_IMPLEMENTATION
static inline Mat init_matrix_image(const char* name);
#endif
#endif  //MATRIX_H


#ifdef MATRIX_IMPLEMETATION
static inline float rand_float(void) {
	return (float)rand() / (float)RAND_MAX;
	}

static inline float sigmoid_derivative(float x) {
    
    return x * (1.0f - x);
}

Mat matrix_alloc(size_t rows, size_t cols) {
	Mat m;
	m.cols = cols;
	m.rows = rows;
	m.elem = (TYPE *)MATRIX_CALLOC((rows)*(cols)+1,sizeof(*m.elem));    //(*M.elem) derefrence Changing type of element
	MATRIX_ASSERT(m.elem!=NULL);
	return m;
	}

static inline void matrix_free(Mat m) {

	free(m.elem);
	}

static inline void matrix_init(Mat m, double *arr) {
	size_t cE = 0;
	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.rows; x++) {
			MATRIX_SHIFT(m, y, x) = arr[cE++];
			}
		}
	}



static inline void matrix_dot(Mat dest, Mat a, Mat b) { //a[1x2] b[2x3] c[1x3]
	MATRIX_ASSERT(a.cols == b.rows);
	MATRIX_ASSERT(dest.rows == a.rows);
	MATRIX_ASSERT(dest.cols == b.cols);

	Mat b_T = matrix_alloc(b.cols, b.rows);
	//Transpose matrix b
	for(size_t y = 0; y < b_T.rows; y++) {
		for(size_t x = 0; x < b_T.cols; x++) {
			MATRIX_SHIFT(b_T, y, x) = MATRIX_SHIFT(b, x, y);
			}
		}
	//Multiplay matrix with transposed
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {
			MATRIX_SHIFT(dest, y, x) = 0;
			for(size_t i = 0; i < a.cols; i++) {
				MATRIX_SHIFT(dest, y, x) += MATRIX_SHIFT(a, y, i) * MATRIX_SHIFT(b_T, x, i);
				}
			}
		}
	matrix_free(b_T);
	}

static inline void matrix_outer(Mat dest, Mat a, Mat b) {
	MATRIX_ASSERT(a.rows == 1);
	MATRIX_ASSERT(b.rows == 1);
	MATRIX_ASSERT(dest.rows >= a.cols);
	MATRIX_ASSERT(dest.cols >= b.cols);

	for(size_t y = 0; y < a.cols; y++) {
		for(size_t x = 0; x < b.cols; x++) {
			MATRIX_SHIFT(dest, y, x) = b.elem[x] * a.elem[y];
			//printf("(%f %f) %f\n",  MATRIX_SHIFT(a, y, 0) ,MATRIX_SHIFT(b, x, 0),  MATRIX_SHIFT(dest, x, y));
			}
		}
	}


static inline void matrix_distance(Mat dest, Mat a, Mat b) {
	MATRIX_ASSERT(dest.rows == a.rows);
	MATRIX_ASSERT(dest.rows == b.rows);
	MATRIX_ASSERT(dest.cols == a.cols);
	MATRIX_ASSERT(dest.cols == b.cols);
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {
			//const float sA = MATRIX_SHIFT(a, y, x) * MATRIX_SHIFT(a, y, x);
			//const float sB = MATRIX_SHIFT(b, y, x) * MATRIX_SHIFT(b, y, x);
			const float di = (MATRIX_SHIFT(b, y, x) - MATRIX_SHIFT(a, y, x)) * (MATRIX_SHIFT(b, y, x) - MATRIX_SHIFT(a, y, x));
			MATRIX_SHIFT(dest, y, x) = di;
			}
		}
	}

static inline void matrix_sum(Mat dest, Mat a) {
	MATRIX_ASSERT(dest.cols == a.cols);
	MATRIX_ASSERT(dest.rows == a.rows);
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {

			MATRIX_SHIFT(dest,y,x) += MATRIX_SHIFT(a,y,x);
			}
		}

	}
static inline void matrix_dec(Mat dest, Mat a) {
	MATRIX_ASSERT(dest.cols == a.cols);
	MATRIX_ASSERT(dest.rows == a.rows);
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {

			MATRIX_SHIFT(dest,y,x) -= MATRIX_SHIFT(a,y,x);
			}
		}

	}
static inline void matrix_add_scalar(Mat dest, float scalar) {
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {

			MATRIX_SHIFT(dest,y,x) += scalar;
			}
		}
	}

static inline void matrix_mul_scalar(Mat dest, float scalar) {
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {

			MATRIX_SHIFT(dest,y,x) *= scalar;
			}
		}

	}


static inline void matrix_rand(Mat dest,float low, float high) {
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {
			MATRIX_SHIFT(dest,y,x) = (TYPE)((rand_float()*(high - low)) + low);   //IN RANGE
			}
		}
	}

static inline void matrix_eye(Mat dest) {
	for(size_t y = 0; y  < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {
			MATRIX_SHIFT(dest, y, x) = (x == y) ?  1 : 0;

			}
		}
	}

static inline void matrix_print(Mat m, const char *name) {
	//system("pause");
	printf("\n\n______________________________________________________________________________\n");
	printf("%s",name);
	printf(" = \n[\n");

	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			printf(" %.2f ", (float)MATRIX_SHIFT(m,y,x));
			}
		printf("\n\n");
		}
	printf("]\n");
	printf("______________________________________________________________________________\n");

	}
static inline void matrix_print_out(Mat m, const char *name, size_t out) {
	//system("pause");
	printf("\n\n______________________________________________________________________________\n");
	printf("%s",name);
	printf(" = \n[\n");

	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			printf(" %.2f ", (float)MATRIX_SHIFT(m,y,x));
			if(x==out) {
				break;
				}
			}
		printf("\n\n");
		}
	printf("]\n");
	printf("______________________________________________________________________________\n");

	}

static inline float matrix_max_element(Mat m) {
	float max = -1000000000;
	for(size_t x  = 0; x < m.cols*m.rows; x++) {
		if(m.elem[x] > max) {
			max = m.elem[x];
			}
		}
	return max;

	}




static inline float sigmoid(float x) {
	return 1.0f/(1 + expf(-x));
	}

static inline void matrix_activation_cols(Mat m,size_t num) {

	for(size_t y = 0; y < m.rows; y++) {
		MATRIX_SHIFT(m,y,num) = sigmoid(MATRIX_SHIFT(m,y,num));
		}


	}

static inline void matrix_activation(Mat m) {

	for(size_t y = 0; y < m.rows; y++)
		for(size_t x = 0; x < m.cols; x++) {
#ifdef MATRIX_ACTIVATION_SIGMOID
			MATRIX_SHIFT(m,y,x) = sigmoid(MATRIX_SHIFT(m,y,x));
#endif
#ifdef MATRIX_ACTIVATION_TANH
			MATRIX_SHIFT(m,y,x) = tanh(MATRIX_SHIFT(m,y,x));
#endif
#ifdef MATRIX_ACTIVATION_RELU
			if(MATRIX_SHIFT(m,y,x) < 0)  MATRIX_SHIFT(m,y,x)  = 0;
#endif
#ifdef MATRIX_ACTIVATION_LRELU
			if(MATRIX_SHIFT(m,y,x) < 0)  MATRIX_SHIFT(m,y,x)  = -0.01*MATRIX_SHIFT(m,y,x);
#endif
#ifdef MATRIX_ACTIVATION_BINARY
			if(MATRIX_SHIFT(m,y,x) > 0)
				MATRIX_SHIFT(m,y,x) = 1.0f;
			else
				MATRIX_SHIFT(m,y,x) = 0 ;
#endif

#ifdef MATRIX_ACTIVATION_SWISH
			MATRIX_SHIFT(m,y,x) *= sigmoid(MATRIX_SHIFT(m,y,x));
#endif

#ifdef MATRIX_ACTIVATION_RAND_VALUE
#ifdef MATRIX_ACTIVATION_RAND_RATE
			if(rand_float() < MATRIX_ACTIVATION_RAND_RATE)
				MATRIX_SHIFT(m,y,x) += 2*rand_float() * (float)MATRIX_ACTIVATION_RAND_VALUE - (float)MATRIX_ACTIVATION_RAND_VALUE;
#endif
#endif
			}


	}

//REWRITE IN ONE LOOP
static inline void matrix_feedforward(Mat *out,Mat input, Mat *weights, Mat *bias,size_t num_of_layer) {
	//system("pause");
	matrix_dot(out[0],input,weights[0]);
	matrix_sum(out[0],bias[0]);
	matrix_activation(out[0]);

	for(size_t i = 1; i < num_of_layer; i++) {
		//system("pause");
		matrix_dot(out[i],out[i-1],weights[i]);
		//system("pause");
		matrix_sum(out[i],bias[i]);
		//system("pause");
		matrix_activation(out[i]);
		}
	}
	
static inline void matrix_backpropagate(Mat input, Mat target, Mat *activations, Mat *weights, Mat *biases, size_t num_of_layer) {
   
    Mat *delta_weights = (Mat *)MATRIX_CALLOC(num_of_layer, sizeof(Mat));
    MATRIX_ASSERT(delta_weights != NULL);
    Mat *delta_biases = (Mat *)MATRIX_CALLOC(num_of_layer, sizeof(Mat));
    MATRIX_ASSERT(delta_biases != NULL);
    Mat *deltas = (Mat *)MATRIX_CALLOC(num_of_layer, sizeof(Mat)); 
    MATRIX_ASSERT(deltas != NULL);

    for (size_t i = 0; i < num_of_layer; ++i) {
        delta_weights[i] = matrix_alloc(weights[i].rows, weights[i].cols);
        delta_biases[i] = matrix_alloc(biases[i].rows, biases[i].cols);
        deltas[i] = matrix_alloc(biases[i].rows, biases[i].cols);
    }

   
    for (size_t y = 0; y < target.rows; ++y) {
        for (size_t x = 0; x < target.cols; ++x) {
            TYPE activated_output = MATRIX_SHIFT(activations[num_of_layer - 1], y, x);
            TYPE target_val = MATRIX_SHIFT(target, y, x);
            TYPE error = activated_output - target_val;
            TYPE deriv = sigmoid_derivative(activated_output); // SIGMOID ACT
            MATRIX_SHIFT(deltas[num_of_layer - 1], y, x) = error * deriv;
        }
    }

    for (int l = num_of_layer - 2; l >= 0; --l) {
        Mat weights_T = matrix_alloc(weights[l+1].cols, weights[l+1].rows);
        for(size_t y = 0; y < weights_T.rows; ++y) {
            for(size_t x = 0; x < weights_T.cols; ++x) {
                MATRIX_SHIFT(weights_T, y, x) = MATRIX_SHIFT(weights[l+1], x, y);
            }
        }

        Mat product_delta_weight_T = matrix_alloc(deltas[l].rows, weights_T.cols); // (1xN) * (N x M) = 1 x M
        matrix_dot(product_delta_weight_T, deltas[l+1], weights_T); 

        for (size_t y = 0; y < deltas[l].rows; ++y) {
            for (size_t x = 0; x < deltas[l].cols; ++x) {
                TYPE activated_val = MATRIX_SHIFT(activations[l], y, x);
                TYPE deriv = sigmoid_derivative(activated_val);

                MATRIX_SHIFT(deltas[l], y, x) = MATRIX_SHIFT(product_delta_weight_T, y, x) * deriv;
            }
        }
        matrix_free(weights_T);
        matrix_free(product_delta_weight_T);
    }

    // GRADIENTS
    for (size_t l = 0; l < num_of_layer; ++l) {
        
        Mat current_input_for_dot;
        if (l == 0) {
            current_input_for_dot = input; 
        } else {
            current_input_for_dot = activations[l-1]; 
        }

        Mat current_input_for_dot_T = matrix_alloc(current_input_for_dot.cols, current_input_for_dot.rows);
        for(size_t y = 0; y < current_input_for_dot_T.rows; ++y) {
            for(size_t x = 0; x < current_input_for_dot_T.cols; ++x) {
                MATRIX_SHIFT(current_input_for_dot_T, y, x) = MATRIX_SHIFT(current_input_for_dot, x, y);
            }
        }
        
        
        matrix_dot(delta_weights[l], current_input_for_dot_T, deltas[l]);

        matrix_copy(delta_biases[l], deltas[l]);

        // Update weights and biases
        matrix_mul_scalar(delta_weights[l], LR);

			  matrix_dec(weights[l], delta_weights[l]); // W = W - LR * dW. 

        matrix_mul_scalar(delta_biases[l], LR);       
        matrix_dec(biases[l], delta_biases[l]); // B = B - LR * dB

        matrix_free(current_input_for_dot_T);
    }


    
    for (size_t i = 0; i < num_of_layer; ++i) {
        matrix_free(delta_weights[i]);
        matrix_free(delta_biases[i]);
        matrix_free(deltas[i]);
    }
    free(delta_weights);
    free(delta_biases);
    free(deltas);
}	


//*/
static inline void matrix_softmax(Mat m) {
	double sum = 0.0f;
	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			sum+=exp((double)MATRIX_SHIFT(m,y,x) / (double) T);
			}
		}
	//printf("sum = %f",sum);
	//system("pause");
	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			MATRIX_SHIFT(m,y,x) = (double)exp((double)MATRIX_SHIFT(m,y,x) / (double)T)/(double)sum;
			}
		}
	}
static inline void matrix_copy(Mat dest, Mat src) {
	//MATRIX_ASSERT();
	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {
			MATRIX_SHIFT(dest,y,x) = MATRIX_SHIFT(src,y,x);
			}
		}
	}
static inline void matrix_convolution(Mat dest, Mat a, Mat kernel) {
	MATRIX_ASSERT(dest.cols <= a.cols);
	MATRIX_ASSERT(dest.rows <= a.rows);

	for (size_t y = 0; y < dest.cols; y++) {
		for (size_t x = 0; x < dest.rows; x++) {

			float sum = 0.0f;
			for (size_t i = 0; i < kernel.rows; i++) {
				for (size_t j = 0; j < kernel.cols; j++) {
					sum+=MATRIX_SHIFT(a,y+i,x+j)*MATRIX_SHIFT(kernel,i,j);

					}

				}
			MATRIX_SHIFT(dest,y, x) = sum;
			}

		}

	}


static inline void matrix_mutation(Mat m) {
	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			if(rand_float() < MUTATION_RATE && MATRIX_SHIFT(m,y,x)!=0) {
				if(rand()%2==0) {
					MATRIX_SHIFT(m,y,x)+=LR*rand_float();
					}
				else {
					MATRIX_SHIFT(m,y,x)-=LR*rand_float();
					}

				}
			}
		}
	}


static inline void matrix_mutation_disable(Mat m,float DM) {
	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			if(rand_float() < MUTATION_RATE) {
				if(rand_float() < DM) {

					MATRIX_SHIFT(m,y,x) = 0.0f;
					}
				}
			}
		}
	}


static inline void matrix_mutation_enable(Mat m, float EM) {
	for(size_t y = 0; y < m.rows; y++) {
		for(size_t x = 0; x < m.cols; x++) {
			if(rand_float() < MUTATION_RATE) {
				if(rand_float() < EM && 	MATRIX_SHIFT(m,y,x) == 0) {
					MATRIX_SHIFT(m,y,x) = rand_float()*2.0f - 1.0f;
					}
				}
			}
		}
	}



static inline void matrix_reproduce(Mat a, Mat b) {
	MATRIX_ASSERT(a.rows == b.rows);
	MATRIX_ASSERT(a.cols == b.cols);
	for(size_t y = 0; y < a.rows; y++) {
		for(size_t x = 0; x < a.cols; x++) {
			int desi = rand()%2;
			if(desi == 0) {
				MATRIX_SHIFT(a,y,x) = MATRIX_SHIFT(b,y,x);

				}
			}
		}
	matrix_mutation(a);
#ifdef DISABLE_MUTATION
	matrix_mutation_disable(a, DISABLE_MUTATION);
#endif
#ifdef ENABLE_MUTATION
	matrix_mutation_enable(a, ENABLE_MUTATION);
#endif

	}


static inline float matrix_EMD(Mat dest, Mat p, Mat q, Mat cost) {
	const size_t n = p.cols;
	const size_t m = q.cols;

	float *supply = malloc(n * sizeof(float));
	float *demand = malloc(m * sizeof(float));

	for (size_t i = 0; i < n; ++i)
		supply[i] = MATRIX_SHIFT(p, 0, i);
	for (size_t j = 0; j < m; ++j)
		demand[j] = MATRIX_SHIFT(q, 0, j);


	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < m; ++j)
			MATRIX_SHIFT(dest, i, j) = 0.0f;

	float totalCost = 0.0f;
	float totalFlow = 0.0f;

	while (1) {
		float minCost = 10000000000;
		int bestI = -1, bestJ = -1;

		for (size_t i = 0; i < n && i < cost.rows; ++i) {
			if (supply[i] <= 0.0f) continue;
			for (size_t j = 0; j < m && j < cost.cols; ++j) {
				if (demand[j] <= 0.0f) continue;

				float c = MATRIX_SHIFT(cost, i, j);
				if (c < minCost) {
					minCost = c;
					bestI = i;
					bestJ = j;
					}
				}
			}

		if (bestI == -1 || bestJ == -1)
			break;

		float amount = fminf(supply[bestI], demand[bestJ]);
		MATRIX_SHIFT(dest, bestI, bestJ) = amount;

		totalCost += amount * MATRIX_SHIFT(cost, bestI, bestJ);
		totalFlow += amount;

		supply[bestI] -= amount;
		demand[bestJ] -= amount;
		}

	free(supply);
	free(demand);

	return (totalFlow > 0.0f) ? (totalCost / totalFlow) : 0.0f;
	}





#ifdef STB_IMAGE_IMPLEMENTATION

static inline Mat init_matrix_image(const char* name) {

	int x, y, comp, cD = 0;
	uint8_t *img_data = stbi_load(name, &x, &y, &comp, 0);

	assert(img_data != NULL);
	Mat dest = matrix_alloc((size_t)y, (size_t)(x*(comp)));

	for(size_t y = 0; y < dest.rows; y++) {
		for(size_t x = 0; x < dest.cols; x++) {
			MATRIX_SHIFT(dest, y, x) = (float)img_data[cD++];
			}
		}
//	printf("CHANELS %d\n", comp);
	free(img_data);
	return dest;
	}


#endif


#endif  //MATRIX_IMPLEMENTATION
