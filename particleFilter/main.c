#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "rssi.h"

#define WARNING(...)        fprintf(stdout, __VA_ARGS__)
#define ERROR_BREAK(...)    fprintf(stderr, __VA_ARGS__); exit(-1)
#define LOG(...)            fprintf(stdout, __VA_ARGS__)
#define CLAMP(X, LOW, HIGH) {if((X) < (LOW)) (X) = (LOW); if((X) > (HIGH)) (X) = (HIGH);}
#define ASSERT(msg) {fprintf(stderr, "assert in:\n\tFILE %s\n\tLINE %d\n\tmsg: %s" , __FILE__, __LINE__, msg); exit(-1);}
#define DROP(var) {(void)var;}


#define DA_SIZE 64
#define da_append(da, item)                                                            \
    do {                                                                                 \
        if ((da)->count >= (da)->capacity) {                                               \
            (da)->capacity = (da)->capacity == 0 ? DA_SIZE : (da)->capacity*2;               \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));         \
            assert((da)->items != NULL && "Realloc fail !!!");                               \
        }                                                                                \
        (da)->items[(da)->count++] = (item);                                               \
    } while (0)


struct kahan { double sum, err; };

static struct kahan kahan(struct kahan k, double x){
    double y = x - k.err;
    double t = k.sum + y;
    k.err = t - k.sum - y;
    k.sum = t;
    return k;
}

static uint64_t sfc64(uint64_t s[4]){
    uint64_t r = s[0] + s[1] + s[3]++;
    s[0] = (s[1] >> 11) ^ s[1];
    s[1] = (s[2] <<  3) + s[2];
    s[2] = r + (s[2]<<24 | s[2] >>40);
    return r;
}

static double rand_GAUS(uint64_t s[4]){
    uint64_t u = sfc64(s);
    double x = __builtin_popcount(u>>32);
    x += (uint32_t)u * (1 / 4294967296.);
    x -= 16.5;
    x *= 0.3517262290563295;
    return x;
}

typedef struct {
    float d;   // distance (hipoteza)
    float w;   // weight
} Particle;

typedef struct {
    size_t count;
    size_t capacity;
    Particle* items;
} Particle_DA;

typedef struct {
    Particle_DA p;
    float A;       // RSSI @ 1m
    float n;       // path-loss exponent
    float sig_meas;// measurement noise
    float sig_pro; // process noise
    float d_min, d_max;
    uint64_t s[4]; // RNG state
} ParticleFilter;

static inline ParticleFilter* init_PF(int N, float A, float n, 
                                      float sig_meas, float sig_pro, float d_min, float d_max){
    ParticleFilter *pf = malloc(sizeof(ParticleFilter));
    pf->A = A;
    pf->n = n;
    pf->sig_meas = sig_meas;
    pf->sig_pro = sig_pro;
    pf->d_min = d_min;
    pf->d_max = d_max;
    pf->p.count = 0;
    pf->p.capacity = 0;
    pf->p.items = NULL;

    pf->s[0] = rand();
    pf->s[1] = rand();
    pf->s[2] = rand();
    pf->s[3] = rand();

    for(int i = 0; i < N; i++){
        Particle part;
        part.d = d_min + (float)rand()/RAND_MAX * (d_max - d_min);
        part.w = 1.0f / (float)N;
        da_append(&pf->p, part);
    }
    return pf;    	
}

// RSSI model: rssi = A - 10*n*log10(d)
static inline float model_rssi(ParticleFilter *pf, float d){
    if(d <= 0.01f) d = 0.01f;
    return pf->A - 10.0f * pf->n * log10f(d);
}

void step_PF(ParticleFilter *pf, float rssi_measured){
    float w_sum = 0.0f;
    for(size_t i=0; i<pf->p.count; i++){
        // prediction
        float noise = pf->sig_pro * rand_GAUS(pf->s);
        pf->p.items[i].d += noise;
        CLAMP(pf->p.items[i].d, pf->d_min, pf->d_max);
        float rssi_pred = model_rssi(pf, pf->p.items[i].d);
        float diff = rssi_measured - rssi_pred;
        float w = expf(-0.5f * (diff*diff)/(pf->sig_meas*pf->sig_meas));
        pf->p.items[i].w = w;
        w_sum += w;
    }
    
    for(size_t i=0; i<pf->p.count; i++){
        pf->p.items[i].w /= w_sum;
    }
   
    //
    Particle_DA newp = {0};
    float step = 1.0f / pf->p.count;
    float r = rand_GAUS(pf->s) * step;
    float c = pf->p.items[0].w;
    size_t i = 0;
    
    for(size_t m=0; m<pf->p.count; m++){
        float U = r + m*step;
        while(U > c && i < pf->p.count - 1){
            i++;
            c += pf->p.items[i].w;
        }
        da_append(&newp, pf->p.items[i]);
        newp.items[m].w = 1.0f/pf->p.count;
    }
    //exit(-1);
    free(pf->p.items);
    pf->p = newp;
}

static inline float estimate_PF(ParticleFilter *pf){
    float sum = 0.0f;
    for(size_t i=0; i<pf->p.count; i++){
        sum += pf->p.items[i].d * pf->p.items[i].w;
    }
    return sum;
}

typedef struct{
  size_t count; 
  size_t capacity;
  float  *items;
}Float_DA;


Float_DA rssi_stream = {0};
int main(){
    const int A = -69;
    const int N = 600;
    const int n = 2.08F;
    const float sig_pro = 0.05f;
    const float sig_mea = 10.0f;
    srand(time(0));
    LOG("\t\t\tParticle Filter\n");
    LOG("\n --------------------------------------------------- \n");
    ParticleFilter *pf = init_PF(N, A, n, sig_mea, sig_pro, 0.5, 20.0);
    const int distSize = sizeof(distanc) / sizeof(float);
    for(int i = 0; i < distSize; i++){
     const float rssi = model_rssi(pf, distanc[i]);
     da_append(&rssi_stream, rssi);            
    }
    
    FILE *f = fopen("temp.py", "w+");
    fprintf(f, "import numpy as np\n");
    fprintf(f, "import matplotlib.pyplot as plt\n");
    fprintf(f, "dist = [");
    for(size_t i = 0; i < rssi_stream.count; i++){
    	fprintf(f, "%f, ", distanc[i]);
    }
    fprintf(f, "]\n");
    fprintf(f, "distParticle = [ ");
    
    for(int t=0; t<distSize; t++){
//        step_PF(pf, rssi_stream.items[t]);
//        step_PF(pf, rssi_stream.items[t]);
//        step_PF(pf, rssi_stream.items[t]);
//        step_PF(pf, rssi_stream.items[t]);
        step_PF(pf, rssi_stream.items[t]);
        float est = estimate_PF(pf);
        fprintf(f, "%f, ", est);
        //fprintf(f, "Step %d: RSSI=%.1f -> Estimated distance=%.3f m\n", t, rssi_stream.items[t], est);
    }
    fprintf(f, "]\nplt.plot(dist)\nplt.plot(distParticle)\nplt.savefig(\"comp.png\")\nplt.show()");
    fclose(f);
    //sleep(10);
    system("python temp.py");
    //exit(-1);
    //int a = 0;
    //fread(&a, 1, sizeof(int), stdin);
    LOG("\n --------------------------------------------------- \n");
    return 0;
}

