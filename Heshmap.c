#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#define SIZEOFMAP 100000
#define MAXSIZOFKEY 100
#define arrsize(x) sizeof(x) / sizeof(x[0])

typedef struct hashMap{
	
	char **key;
	int  *value;
	//uint8_t *is;
	
}HASHMAP;
void allocHashMap(HASHMAP *hashmap)
{
	
	//hashmap = (HASHMAP*)calloc(1000*1000*256,sizeof(HASHMAP));
	hashmap->key = (char**)malloc(SIZEOFMAP*sizeof(char*));
	hashmap->value = (int*)calloc(SIZEOFMAP, sizeof(int));
 	//hashmap->is = (uint8_t*)calloc(SIZEOFMAP,sizeof(uint8_t)); 
	
}

size_t hashFunc(HASHMAP *hashmap, char *str, uint8_t control){
	
        unsigned long hash = 5843;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
       
        //system("pause");
        while(hashmap->key[(hash) % (SIZEOFMAP)]!=NULL && control==1){
        	hash++;
			if(hash <= SIZEOFMAP-1)
        			hash=0;
        			
            
		}
        return (hash) % (SIZEOFMAP);

	
}

void insertElementInHashMap(HASHMAP *hashmap, char *word){ // diferent key every time
	
	
	//assert(arrsize(key) < MAXSIZOFKEY && "MAX SIZE OF KEY\n");
	int keyIndex =  hashFunc(hashmap,word,0x01);
	//printf("%u\n",keyIndex);
	hashmap->key[keyIndex] = (char*)malloc(sizeof(char*)*sizeof(word));
	assert(hashmap->key[keyIndex] != NULL && "ALLOCATION FAILED!!!\n");
	strcpy(hashmap->key[keyIndex], word);
	printf("INSERTED: %s ON POSITION: %u\n", hashmap->key[keyIndex],keyIndex);
	system("pause");
	
}

void readElementInHashMap(HASHMAP *hashmap,char *word){
	
	size_t wordSize = arrsize(word);
	size_t keyIndex = hashFunc(hashmap,word,0x00);
	while(hashmap->key != NULL && !strcmp(hashmap->key,word)){
		keyIndex++;
	}
		printf("%s %d\n",hashmap->key[keyIndex],hashmap->value[keyIndex]);
}

void delateElementInHashMap(HASHMAP *hashmap,char *word){
	
	size_t wordSize = arrsize(word);
	size_t keyIndex = hashFunc(hashmap,word,0x00);
	while(hashmap->key != NULL && !strcmp(hashmap->key,word)){
		keyIndex++;
	}
	free(hashmap->key[keyIndex]); 	
	hashmap->key[keyIndex] = NULL; 
}

int main(){
	
	
	HASHMAP hashmap;
	allocHashMap(&hashmap);
	//system("pause");
	char *key = "NESTO",*word = "jij01234532423434234234" ,*word1 ="1";
	//system("pause");
	uint8_t i = 0;
	insertElementInHashMap(&hashmap,key);
	insertElementInHashMap(&hashmap,word);
	insertElementInHashMap(&hashmap,key);
	insertElementInHashMap(&hashmap,word);	
	insertElementInHashMap(&hashmap,word);
	insertElementInHashMap(&hashmap,key);
	insertElementInHashMap(&hashmap,key);
	insertElementInHashMap(&hashmap,key);
	insertElementInHashMap(&hashmap,word);
	insertElementInHashMap(&hashmap,key);
	readElementInHashMap(&hashmap,word1);
	readElementInHashMap(&hashmap,word);
	delateElementInHashMap(&hashmap,word);
	readElementInHashMap(&hashmap,word);
	return 0;
}
