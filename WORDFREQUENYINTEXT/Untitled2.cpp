#include "hashmap.h"
#include<stdlib.h>
#define SIZEOFBUFFER 1000


int main()
{
    HASHMAP hashmap;
	allocHashMap(&hashmap);
	//system("pause");
	//char *key = "NESTO",*word = "jij01234532423434234234" ,*word1 ="1";
	//system("pause");
	uint8_t i = 0;
	int keyIndex = 0;
	FILE *f = fopen("tsh.txt","r");
	char buffer[SIZEOFBUFFER],character;
	size_t counter = 0;
	while(!feof(f)){
	
	fread(&character,sizeof(char),1,f);
	//printf("%c\n",buffer);
	assert(SIZEOFBUFFER > counter && "WORD TO BIG !!!");
	int conditionWord = (character==' ' || character == '.' || character == ',' 
					  || character == 10 || character == ':' || character == ';' || character == '\0');

	if(conditionWord){
		buffer[counter] = '\0';
		counter = 0;
		//printf("%s\n",buffer);
		do{
				fread(&character,sizeof(char),1,f);
				int conditionWord = (character==' ' || character == '.' || character == ',' 
					  || character == 10 || character == ':' || character == ';' || character == '\0');
			
		}while(!conditionWord);
		insertElementInHashMap(&hashmap,buffer);
		//system("pause");
	}
	//if(!conditionWord)
	buffer[counter++] = character;
	//printf("%s\n",buffer);
	

	}
	fclose(f);
	char s[100];
	//uint32_t keyIndex = 0;
	while(1){
		printf("\nRijec:  ");
		scanf("%s",&s);
		keyIndex = (int)readElementInHashMap(&hashmap,s);
		//printf("%d", keyIndex);
		
		
		printf("Rijec %s se ponavlja %d puta.\n",hashmap.key[keyIndex],hashmap.value[keyIndex]);
		//system("c");
	}
}

