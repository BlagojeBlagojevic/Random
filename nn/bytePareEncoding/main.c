#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<unistd.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


#define DA_SIZE 256
#define da_append(da, item)                                                              \
    do {                                                                                 \
        if ((da)->count >= (da)->capacity) {                                             \
            (da)->capacity = (da)->capacity == 0 ? DA_SIZE : (da)->capacity*2;           \
            (da)->items = realloc((da)->items, (da)->capacity*sizeof(*(da)->items));     \
            assert((da)->items != NULL && "Realloc fail !!!");                           \
        }                                                                                \
                                                                                         \
        (da)->items[(da)->count++] = (item);                                             \
    } while (0)

#define swap(Type, a, b)\
    do{\
        Type t = (a);\
        (a) = (b);\
        (b) = t;\
    }while(0)    
    


typedef struct {
    uint32_t l, r;
}Pair;

typedef struct {
    Pair *items;
    size_t capacity;
    size_t count;
}Pairs;


typedef struct {
    Pair key;
    size_t value;
}KV;

typedef struct{
    KV *items;
    size_t capacity;
    size_t count;
}KVS;



typedef struct{
    uint32_t *items;
    size_t capacity;
    size_t count;
}Tokens;

void printTokens(Pairs bpeTable, Tokens tokens){
    for (size_t i = 0; i < tokens.count; ++i){
        uint32_t token = tokens.items[i]; 
        if(bpeTable.items[token].l == token){
            printf("%c", token);
        }else{
            printf("[%u]", token);
        }
        
    }
    printf("\n");
}
void exportGraph(Pairs bpeTable){
    FILE *f = fopen("t.dot", "w+");
    if(f == NULL){exit(-1);}
    fprintf(f, "digraph bpeTable {\n");
    for (uint32_t token = 0; token < bpeTable.count; token++){
        if(token != bpeTable.items[token].l){
            fprintf(f, " %u -> %u\n", token, bpeTable.items[token].l);
            fprintf(f, " %u -> %u\n", token, bpeTable.items[token].r);

        }
    }
    fprintf(f, "}\n");
    //sleep(1);
    fclose(f);
    system("dot t.dot -Tpng -o bpmTable.png");
}


#define SIZE_TEXT 1000
int main(){
    
    //const char* text = "Byte pair encoding (also known as BPE, or digram coding) is an algorithm, first";
    char *text = malloc(SIZE_TEXT);
    memset(text, 0, SIZE_TEXT);
    FILE *f = fopen("text.txt", "r");
    if(f == NULL) return -1;
    fread(text, sizeof(char), SIZE_TEXT, f);
    fclose(f);
    //int textSize = (int)strlen(text);
    int textSize = SIZE_TEXT;
    KV *freq = NULL;
    Tokens tokens_in = {0};
    Tokens tokens_out = {0};
    Pairs bpeTable = {0};
    for(uint32_t i = 0; i < 256; i++){
        da_append(&bpeTable, (Pair){.l = i});
    }
    for(int i = 0; i < textSize; i++){
        da_append(&tokens_in, (uint32_t)text[i]);
    }
for(;;){
    tokens_out.count = 0;
    hmfree(freq);
    for(size_t i = 0; i < tokens_in.count - 1; i++){
        Pair key = {
            .l = tokens_in.items[i], .r = tokens_in.items[i+1]
        };
        ptrdiff_t diff = hmgeti(freq, key);
        if(diff < 0) hmput(freq, key, 1);
        else {freq[diff].value+=1;}        
        //printf("(%c%c)\n", a, b);
    }
    ptrdiff_t maxIndex = 0;
    for(ptrdiff_t i = 1; i < hmlen(freq); i++){
        if(freq[i].value > freq[maxIndex].value){
            maxIndex = i;
        }
    }
    
    printf("MAX(%c, %c)=>%zu\n", (char)freq[maxIndex].key.l, (char)freq[maxIndex].key.r, freq[maxIndex].value);
    if(freq[maxIndex].value <= 1){break;}
    da_append(&bpeTable, freq[maxIndex].key);
    for (size_t i = 0; i < tokens_in.count;){
        if(i + 1 >= tokens_in.count){
            da_append(&tokens_out, tokens_in.items[i]);
            i+=1;
        }else{
                Pair pair = {.l = tokens_in.items[i], .r = tokens_in.items[i+1] };
                if(memcmp(&pair, &freq[maxIndex].key, sizeof(pair))==0){
                    da_append(&tokens_out, bpeTable.count - 1);
                    i+=2;
                    }else{
                        da_append(&tokens_out, tokens_in.items[i]);
                        i+=1;
                }
        }
    }
    //printTokens(bpeTable, tokens_in);
    //printTokens(bpeTable, tokens_out);   
    //printf("SIZE IN:%zu\n", tokens_in.count);
    //printf("SIZE OUT:%zu\n", tokens_out.count);
    swap(Tokens, tokens_in, tokens_out);
}

exportGraph(bpeTable);
    /*KVS sortetedPair = {0};
    for(ptrdiff_t i = 0; i < hmlen(freq); i++){
        da_append(&sortetedPair, freq[i]);
        printf("(%u, %u)=>%zu\n", freq[i].key.l, freq[i].key.r, freq[i].value);
    }*/
    

    //printf("%s", text);
    free(text);
    return 0;

}