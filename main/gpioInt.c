
#include<stdio.h>

static int stateOfLeds[] = {false, false, false};
int state = false;

void nesto(void *args){
    
   //printf("Nesto\n");
    state = true;
    stateOfLeds[0] = !stateOfLeds[0];
    gpio_intr_disable(GPIO_NUM_0);
    //printf("Nesto\n");

}

void setup(){
 


      
      

}


void loop(){
 
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
    gpio_set_intr_type(GPIO_NUM_0, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(GPIO_NUM_0);
    gpio_isr_register(nesto, (void* )NULL, GPIO_NUM_0, NULL);
   
  while(1){
    if(state){

      gpio_intr_enable(GPIO_NUM_0);
      
      printf("enable1!!\n");
      state = 0;
    }
    printf("%d\n",stateOfLeds[0]);
    vTaskDelay(10000);
  }
 
}
