#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#define SIZEOFMAP 400000
#define MAXSIZOFKEY 100
#define arrsize(x) sizeof(x) / sizeof(x[0])
#define PRINT 0
typedef struct hashMap{
	
	char **key;
	int  *value;
	//char* read;    //REPRESENT RETURNED ELEMET  
	uint8_t is[SIZEOFMAP];
	
}HASHMAP;
typedef struct element{
	char *key;
	int  *value;
}ELEMENT;

void allocHashMap(HASHMAP *hashmap)
{
	
	//hashmap = (HASHMAP*)calloc(1000*1000*256,sizeof(HASHMAP));
	hashmap->key = (char**)malloc(SIZEOFMAP*sizeof(char*));
	hashmap->value = (int*)calloc(SIZEOFMAP, sizeof(int));
		
}

uint32_t hashFunc(HASHMAP *hashmap, char *str, uint8_t control){
	
        unsigned long hash = 58434;
        int c;
        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */	
        return (hash) % (SIZEOFMAP);

	
}

void insertElementInHashMap(HASHMAP *hashmap, char *word){ // diferent key every time
	
	
	//assert(arrsize(key) < MAXSIZOFKEY && "MAX SIZE OF KEY\n");
	uint32_t keyIndex =  hashFunc(hashmap,word,0x01);
	//printf("%u\n",keyIndex);
 if(hashmap->key[keyIndex]==NULL)
	        {
                hashmap->key[keyIndex] = (char*)malloc(sizeof(char*)*sizeof(word));
	        	hashmap->is[keyIndex] = 0x01; 
			}
			
	assert(hashmap->key[keyIndex] != NULL && "ALLOCATION FAILED!!!\n");
	strcpy(hashmap->key[keyIndex], word);
	hashmap->value[keyIndex]++;
	//printf("%d\n",hashmap->value[keyIndex]);
#if PRINT
	printf("INSERTED: %s ON POSITION: %u\n", hashmap->key[keyIndex],keyIndex);
	system("pause");
#endif	
}

uint32_t readElementInHashMap(HASHMAP *hashmap,char *word){
	
	size_t wordSize = arrsize(word);
	uint32_t keyIndex =  hashFunc(hashmap,word,0x00);
	//system("pause");

#if PRINT
		printf("%s %d\n",hashmap->key[keyIndex],hashmap->value[keyIndex]);
#endif
	  	return keyIndex;
	  
}

void delateElementInHashMap(HASHMAP *hashmap,char *word){
	
	size_t wordSize = arrsize(word);
	size_t keyIndex = hashFunc(hashmap,word,0x01);
	while(strcmp(hashmap->key[keyIndex],word)){
		keyIndex++;
	}
	
	free(hashmap->key[keyIndex]); 	
	hashmap->key[keyIndex] = NULL; 
}


