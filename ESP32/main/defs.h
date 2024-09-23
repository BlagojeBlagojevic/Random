#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//WE WILL USE sdkconfig to setup our esp32 periferals
#include "sdkconfig.h"

//DELAY IN MS DEFINITION
#define DELAY(x)  vTaskDelay((x) / portTICK_PERIOD_MS);



//ESP LIBS WE DEPEND ON
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_err.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "esp_eth_enc28j60.h"
#include "driver/spi_master.h"


#define ERROR(...) ESP_LOGE(tag, __VA_ARGS__); ESP_ERROR_CHECK(ESP_FAIL);
#define MAX_SIZE_OF_STRINGS 100   //LIMIT VALUE OF STRINGS
#define LIMIT_STRING(x) if(x > MAX_SIZE_OF_STRINGS) {x = MAX_SIZE_OF_STRINGS;}
//TYPES DEFINES

#include<stdint.h>
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;
 

//THIS PART IS LIGHT WEIGHT IMPLEMENTATION OF SEMAPHORES FOR BLOCKING OF A TASK

typedef enum{
  WIFI = 0,
  SERVER,
  ETHERNET,
  GET, 
  POST,
}State;

#define NUM_OF_STATES 5
volatile u8 _STATE[NUM_OF_STATES];
#define BLOCK_TASK(x, time)  while(!_STATE[x]){DELAY((time));}
#define SET_STATE(x)        _STATE[x] =  true
#define CLEAR_STATE(x)      _STATE[x] =  false
#define GET_STATE(x)        _STATE[x]


#endif