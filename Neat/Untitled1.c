#include<stdio.h>

int main()
{
	int arr[] = {1,25, 50, 75, 100};

	printf("%d",arr[1]);
	for(size_t i = 0; i < 5;i++){
    printf("%d, ", arr[i]);
    printf("\n");
    system("pause");
	//int max = a[0];
	
	for(size_t i = 0; i < 5;i++){
		for(size_t j = i+1; j < 5;j++){
			if(arr[j]>arr[i]){
				//max = a[j];
				int pom = arr[i];
				arr[i] = arr[j];
				arr[j] = arr[i]; 
			}
	}
	
}

for(size_t i = 0; i < 5;i++){
    printf("%d, ", arr[i]);
}
return 0;
}}
