#include <stdio.h>

#include "freertos/FreeRTOS.h"  //Glavni frertos api 
#include "freertos/task.h"      //Sve je kao task 

#include "driver/gpio.h"       //Draiveri za periferale su u drivers 
								//Imamo sledece periferale 
								//https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/index.html

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#define DELAY(x) (((x)) / portTICK_PERIOD_MS)

#define LED_PIN GPIO_NUM_2
#define HIGH 1
#define LOW 0

void app_main(void)
{

	char* outTaskName = pcTaskGetName(NULL); 
	printf("%s\n", outTaskName);
	//ESP_LOGI(outTaskName);

	
	gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
	gpio_reset_pin(LED_PIN);

	while(1){

		gpio_set_level(LED_PIN, HIGH);
		vTaskDelay(DELAY(1000));
		gpio_set_level(LED_PIN, LOW);
		vTaskDelay(DELAY(1000));
	}

}
