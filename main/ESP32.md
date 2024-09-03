# ESP32

## esp_log.h

Enable using CONFIG_LOG_MASTER_LEVEL or 

```c
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

```

```c
esp_log_level_set("*", ESP_LOG_VERBOSE);

```
ESP_LOG"LEVEL"  ` ESP_LOGI(const char* tag, const char* other);`

## Timing control 

#include "freertos/timers.h"

` port_TICK_PERIOD_MS `

`esp_timer_get_time() micro sec`

```c
#include "freertos/timers.h"
#define MS(x) (x) / port_TICK_PERIOD_MS
vTaskDelay(MS(x));
```

port_TICK_PERIOD_MS

## Task Control


`#include "freertos.h/tasks.h"`
Create task
```c
 BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
                         const char * const pcName,
                         const configSTACK_DEPTH_TYPE uxStackDepth,
                         void *pvParameters,
                         UBaseType_t uxPriority,
                         TaskHandle_t *pxCreatedTask
                       );
```
Create task witch will execute on one core

```c
 BaseType_t xTaskCreatePinnedToCore( TaskFunction_t pvTaskCode,
                         const char * const pcName,
                         const configSTACK_DEPTH_TYPE uxStackDepth,
                         void *pvParameters,
                         UBaseType_t uxPriority,
                         TaskHandle_t *pxCreatedTask,
                         uint8_t witchCore
                       );
```
For handling if task is adel to be executed if contition is met we use Queue or global vars(volotaile)

`#include "freertos.h/queue.h"`

```c 
QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength,
                             UBaseType_t uxItemSize );
```

xQueueCreate returns QueueHandle_t 

probobly we will define global xQueue"SomeName"
```c
 BaseType_t xQueueSend(
                        QueueHandle_t xQueue,
                        const void * pvItemToQueue,
                        TickType_t xTicksToWait
                      );
```
xQueueSend put *pvItemToQueue in queue and will probably increase num of elements in queue ,xTicksToWait how mutch
procesor ticks we nead to wait `Must be converted to TickType_t` returns `pdTRUE` if succes else `pdFALSE`.

For handling if sometingh is on queue we use 
```c
BaseType_t xQueueReceive(
                          QueueHandle_t xQueue,
                          void *pvBuffer,
                          TickType_t xTicksToWait
);

```
If something is on it returns `pdTRUE` else `pdFALSE`  
`if(xQueueRecive(xQueue, &buffer, (TickType_t) 10) == pdTrue){//do something}`

If use queue from interupts use special recive, send functions.

For handling blocking of certian tasks we use semaphors. 

There are 2 types of semaphores binary and mutex.

Main difference between mutex and binary semaphore is in mutext if thread lock the critical section then it has to unlock critical section no other thread can unlock it, but in case of binary semaphore if one thread locks critical section using waits function then value of become "0".

To use semaphores we create global SemaphoreHandle_t xSemaphore;

To allocate binary semaphore use:
```c
SemaphoreHandle_t xSemaphoreCreateBinary( void );
```

To allocate mutex semaphore use:
```c
SemaphoreHandle_t xSemaphoreCreateMutex( void );
```

 `xSemaphoreTake()` locks sections of code to be executed untiled `xSemaphoreGive()`
 is called. For differance beatwen mutex and binary look above.

```c
 BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore,
                 TickType_t xTicksToWait );

```

```c
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );
```
Returns value `pdTRUE` , `pdFALSE` like above.

For suspending the task ve use `vTaskSuspend()`.
```c
void vTaskSuspend( TaskHandle_t xTaskToSuspend );
```
For deleting the task we use  `vTaskDelete()`.
```c
void vTaskDelete( TaskHandle_t xTaskToSuspend );

```
For more info on free RTOS https://www.freertos.org/

## GPIO 
In ESP32 near all pin could be changed to be what is desired(UART, I2C, I2S ,ADC).
Inside ESP32 ther is IOMUX and GPIOMUX matrix witch job is to reroute pins internaly (like one giant global 
multiplexer).
https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-reference/peripherals/gpio.html

We use alredy prebuild driver.
`#include "driver/gpio.h"`
Init of types of pins(INPUT, OUTPUT) is done using function 
`gpio_set_direction()`.

```c
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);
```


