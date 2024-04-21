#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


static int del = 500;
QueueHandle_t data;
TaskHandle_t task[3];

#define MS(x) (x) / portTICK_PERIOD_MS

void SendToLed(void* params){


    
  static int a = -1;
  while(1){
    scanf("%d", &a);
    printf("Send to led ");
    if(a > 100){
      xQueueSend(data, &a, 10);
    }
    printf("SetupLED\n");
    vTaskDelay(MS(100));

  }

  


}

void SetupLed(void* params){
  static int buf = 0;
  while(1){
    printf("SetupLed!!!!\n");
    if(xQueueReceive(data, &buf, 10) == pdTRUE)
    if(buf > 100){
      printf("%d\n", buf);
      vTaskSuspend(task[0]);
      vTaskSuspend(task[1]);
      del = buf;
      buf = 0;
      vTaskResume(task[0]);
      vTaskResume(task[1]);
      xQueueReset(data);
      
    }
    vTaskDelay(100);
    
    


  }
  
}

void FlashLed(void* parms){

  while(1){
    
    gpio_set_level(GPIO_NUM_0,LOW);
    printf("Low\n");
    vTaskDelay(MS(del));
    gpio_set_level(GPIO_NUM_0,HIGH);
    printf("High\n");
    vTaskDelay(MS(del));

  }


}


void setup(){
  gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
  data = xQueueCreate(10, sizeof(int));
  xTaskCreatePinnedToCore(SendToLed, "SendLed", 2048,NULL,10, &task[0], 0);
  xTaskCreatePinnedToCore(FlashLed, "Flash LED", 2048, NULL, 20, &task[1], 0);
  xTaskCreatePinnedToCore(SetupLed, "Setup", 2048, NULL, 1, &task[2], 0);
}



void loop(){



}
