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
In ESP32 near all pin could be changed to be what is desired(UART, I2C, I2S, ADC).
Inside ESP32 ther is IOMUX and GPIOMUX matrix witch job is to reroute pins internaly (like one giant global 
multiplexer).
https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-reference/peripherals/gpio.html

We use alredy prebuild driver.
`#include "driver/gpio.h"`
gpio_num_t is like GPIO_NUM_"NUMBER OF PIN".

Init of types of pins(INPUT, OUTPUT) is done using function 
`gpio_set_direction()`.

```c
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);
```

Wrap functon in ESP_ERROR_CHECK() for checking is seting of pin is posible (Probobly problem be RTC pins or if we set i2c, adc, ...).
gpio_mode_t `GPIO_MOD_OUTPUT, GPIO_MOD_INPUT` for seting direction of a pin.

If pin direction is set to be input we coulde get value of a pin.
`uint8_t gpio_get_level(gpio_num_t gpio_num)` 1 for HIGH 0 for LOW.
Probobly nead to `#define HIGH 1`, `#define LOW 0`.

If pin direction is set to be output then we coude set value on pin.
`esp_err_t gpio_set_level(gpio_num_t num, uint8_t value)`.

If we want to use interupts on GPIO pins we nead to set interrupt, and then to 
register callback(Function witch will run when interrupt is trigerd) ` gpio_set_intr_type()`.

```c
esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
```
gpio_int_type:
1.  GPIO_INTR_POSEDGE
2.  GPIO_INTR_NEGEDGE
3.  GPIO_INTR_ANYEDGE
4.  GPIO_INTR_LOW_LEVEL
5.  GPIO_INTR_HIGH_LEVEL



`gpio_isr_register()` is used to setup callback when interupt is happend.

```c
esp_err_t gpio_isr_register(void (*fn)(void*), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
```
`void (*fn)(void*)` is a shape of our function(just provide name of isr and isr must not return and except void* params).

Last step is to enable the interrupt `gpio_intr_enable()`.
```c
esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
```
All pins have pull up and pull down resistors.

## WIFI 

This is the headers for witch ESP32 wifi app depenth on

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
```
Main recomendation is to use EXAMPLE witch is provided in examples.
Lets write ESP32 wifi in sta mode (MOD IN WITCH ESP32 ACTS LIKE A CLIENT).

Wifi initialisation has 4 main phases:

1. `Wi-Fi/LwIP Init Phase`
2. `Wi-Fi Configuration Phase`
3. `Wi-Fi Start phase`
4. `Wi-Fi Conect phase`

### First phase
First phase consist of:

1. The main task calls `esp_netif_init()` to create an LwIP core task and initialize LwIP-related work.

2. The main task calls `esp_event_loop_create()` to create a system Event task and initialize an application event's callback function. In the scenario above, the application event's callback function does nothing but relaying the event to the application task.

3. The main task calls `esp_netif_create_default_wifi_ap()` or `esp_netif_create_default_wifi_sta()` to create default network interface instance binding station or AP with TCP/IP stack.

4. The main task calls `esp_wifi_init()` to create the Wi-Fi driver task and initialize the Wi-Fi driver.

5. The main task calls OS API to create the application task.

```c
// 1 - Wi-Fi/LwIP Init Phase
	ESP_ERROR_CHECK(esp_netif_init()); //TCP
	ESP_ERROR_CHECK(esp_event_loop_create_default());//event loop	
	ESP_ERROR_CHECK(esp_netif_create_default_wifi_sta());//WIFI setup as station
	ESP_ERROR_CHECK(wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT()); //default config
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));	
```

If nvs flash is enabled  `nvs_flash_init()` 
all wifi_configs are saved in flash memory and on next setup will be loaded from it. 

### Second phase 

Second phase consist of :

1. Adding event loops for wifi and loading wifi credential(Same as if flash is enabled).

 ```c
  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL); //ADD EVENT HANDLER
	 esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
 ```
 Code above add event loops for wifi events and ip events .
 
 1. Main wifi events:
   
     1.1 `WIFI_EVENT_STA_START` -> event for start of wifi as station.

     1.2 `WIFI_EVENT_STA_CONNECTED` -> event for handling if wifi is conected

     1.3 `WIFI_EVENT_STA_DISCONNECTED` -> event for handling if wifi is disconected
  
  
  3. Main ip event:

     2.1 `IP_EVENT_STA_GOT_IP`  -> event for handling if we got a ip addres.
   
 For all events consult https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html
 or provided example . 
 `static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)`
EXAMPLE OF BASIC EVENT HANDLER IS:
 ```c
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

```

### Third phase

Just consist of a starting wifi. `esp_wifi_start()`

### Fourth phase 

Just consist of a tring to conect to a wifi. `esp_wifi_connect()`


EXAMPLE OF WIFI INITIALISATION IS:

```c
//WIFI EVENT HANDLER MUST BE DEFINED
void wifi_init(){
    esp_netif_init();                    // TCP/IP initiation 					
    esp_event_loop_create_default();     // event loop 			                
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = ""}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}
```
DEFAULT EXAMPLE 
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
In ESP32 near all pin could be changed to be what is desired(UART, I2C, I2S, ADC).
Inside ESP32 ther is IOMUX and GPIOMUX matrix witch job is to reroute pins internaly (like one giant global 
multiplexer).
https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/api-reference/peripherals/gpio.html

We use alredy prebuild driver.
`#include "driver/gpio.h"`
gpio_num_t is like GPIO_NUM_"NUMBER OF PIN".

Init of types of pins(INPUT, OUTPUT) is done using function 
`gpio_set_direction()`.

```c
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);
```

Wrap functon in ESP_ERROR_CHECK() for checking is seting of pin is posible (Probobly problem be RTC pins or if we set i2c, adc, ...).
gpio_mode_t `GPIO_MOD_OUTPUT, GPIO_MOD_INPUT` for seting direction of a pin.

If pin direction is set to be input we coulde get value of a pin.
`uint8_t gpio_get_level(gpio_num_t gpio_num)` 1 for HIGH 0 for LOW.
Probobly nead to `#define HIGH 1`, `#define LOW 0`.

If pin direction is set to be output then we coude set value on pin.
`esp_err_t gpio_set_level(gpio_num_t num, uint8_t value)`.

If we want to use interupts on GPIO pins we nead to set interrupt, and then to 
register callback(Function witch will run when interrupt is trigerd) ` gpio_set_intr_type()`.

```c
esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
```
gpio_int_type:
1.  GPIO_INTR_POSEDGE
2.  GPIO_INTR_NEGEDGE
3.  GPIO_INTR_ANYEDGE
4.  GPIO_INTR_LOW_LEVEL
5.  GPIO_INTR_HIGH_LEVEL



`gpio_isr_register()` is used to setup callback when interupt is happend.

```c
esp_err_t gpio_isr_register(void (*fn)(void*), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
```
`void (*fn)(void*)` is a shape of our function(just provide name of isr and isr must not return and except void* params).

Last step is to enable the interrupt `gpio_intr_enable()`.
```c
esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
```
All pins have pull up and pull down resistors.

## WIFI 

This is the headers for witch ESP32 wifi app depenth on

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
```
Main recomendation is to use EXAMPLE witch is provided in examples.
Lets write ESP32 wifi in sta mode (MOD IN WITCH ESP32 ACTS LIKE A CLIENT).

Wifi initialisation has 4 main phases:

1. `Wi-Fi/LwIP Init Phase`
2. `Wi-Fi Configuration Phase`
3. `Wi-Fi Start phase`
4. `Wi-Fi Conect phase`

### First phase
First phase consist of:

1. The main task calls `esp_netif_init()` to create an LwIP core task and initialize LwIP-related work.

2. The main task calls `esp_event_loop_create()` to create a system Event task and initialize an application event's callback function. In the scenario above, the application event's callback function does nothing but relaying the event to the application task.

3. The main task calls `esp_netif_create_default_wifi_ap()` or `esp_netif_create_default_wifi_sta()` to create default network interface instance binding station or AP with TCP/IP stack.

4. The main task calls `esp_wifi_init()` to create the Wi-Fi driver task and initialize the Wi-Fi driver.

5. The main task calls OS API to create the application task.

```c
// 1 - Wi-Fi/LwIP Init Phase
	ESP_ERROR_CHECK(esp_netif_init()); //TCP
	ESP_ERROR_CHECK(esp_event_loop_create_default());//event loop	
	esp_netif_create_default_wifi_sta();//WIFI setup as station
	ESP_ERROR_CHECK(wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT()); //default config
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));	
```

If nvs flash is enabled  `nvs_flash_init()` 
all wifi_configs are saved in flash memory and on next setup will be loaded from it. 

### Second phase 

Second phase consist of :

1. Adding event loops for wifi and loading wifi credential(Same as if flash is enabled).

 ```c
  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL); //ADD EVENT HANDLER
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
 ```
 Code above add event loops for wifi events and ip events .
 
 1. Main wifi events:
   
     1.1 `WIFI_EVENT_STA_START` -> event for start of wifi as station.

     1.2 `WIFI_EVENT_STA_CONNECTED` -> event for handling if wifi is conected

     1.3 `WIFI_EVENT_STA_DISCONNECTED` -> event for handling if wifi is disconected
  
  
  3. Main ip event:

     2.1 `IP_EVENT_STA_GOT_IP`  -> event for handling if we got a ip addres.
   
 For all events consult https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html
 or provided example . 
 `static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)`
EXAMPLE OF BASIC EVENT HANDLER IS:
 ```c
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
	ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

 
```
Then we nead to define config for wifi

```c
wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = ""}};

```
Set mode `esp_wifi_set_mode(WIFI_MODE_STA);`
and then call config `esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);`
### Third phase

Just consist of a starting wifi. `esp_wifi_start()`

### Fourth phase 

Just consist of a tring to conect to a wifi. `esp_wifi_connect()`


EXAMPLE OF WIFI INITIALISATION IS:

```c
//WIFI EVENT HANDLER MUST BE DEFINED
void wifi_init(){
    nvs_flash_init();
    esp_netif_init();                    // TCP/IP initiation 					
    esp_event_loop_create_default();     // event loop 			                
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = ""}};
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}
```
ALL HANDLING EXAMPLE

```c
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

/** DEFINES **/
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define TCP_SUCCESS 1 << 0
#define TCP_FAILURE 1 << 1
#define MAX_FAILURES 10

/** GLOBALS **/

// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

// task tag
static const char *TAG = "WIFI";
/** FUNCTIONS **/

//event handler for wifi events
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		ESP_LOGI(TAG, "Connecting to AP...");
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		if (s_retry_num < MAX_FAILURES)
		{
			ESP_LOGI(TAG, "Reconnecting to AP...");
			esp_wifi_connect();
			s_retry_num++;
		} else {
			xEventGroupSetBits(wifi_event_group, WIFI_FAILURE);
		}
	}
}

//event handler for ip events
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
	if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_SUCCESS);
    }

}

// connect to wifi and return the result
esp_err_t connect_wifi()
{
	int status = WIFI_FAILURE;

	/** INITIALIZE ALL THE THINGS **/
	//initialize the esp network interface
	ESP_ERROR_CHECK(esp_netif_init());

	//initialize default esp event loop
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	//create wifi station in the wifi driver
	esp_netif_create_default_wifi_sta();

	//setup wifi station with the default wifi configuration
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /** EVENT LOOP CRAZINESS **/
	wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t got_ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ip_event_handler,
                                                        NULL,
                                                        &got_ip_event_instance));

    /** START THE WIFI DRIVER **/
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "ssid-for-me",
            .password = "super-secure-password",
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );

    // set the wifi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    // start the wifi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA initialization complete");

    /** NOW WE WAIT **/
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
            WIFI_SUCCESS | WIFI_FAILURE,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_SUCCESS) {
        ESP_LOGI(TAG, "Connected to ap");
        status = WIFI_SUCCESS;
    } else if (bits & WIFI_FAILURE) {
        ESP_LOGI(TAG, "Failed to connect to ap");
        status = WIFI_FAILURE;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        status = WIFI_FAILURE;
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    return status;
}

// connect to the server and return the result
void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    esp_err_t status = WIFI_FAILURE;

	//initialize storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // connect to wireless AP
	status = connect_wifi();
	if (WIFI_SUCCESS != status)
	{
		ESP_LOGI(TAG, "Failed to associate to AP, dying...");
		return;
	}
	
}

```
