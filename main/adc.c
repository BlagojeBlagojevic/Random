
#include <stdio.h>
//#includ <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
//#include "hal/adc_types.h"
//#include "esp_adc/adc_oneshot.h"


#define MS(x) (x) /  portTICK_PERIOD_MS

#define INPUT1 ADC1_CHANNEL_0
#define INPUT2 ADC1_CHANNEL_2
#define INPUT3 ADC1_CHANNEL_4
static int val[3];

void setup() {
 
		printf("Nesto\n");

	adc1_config_width(ADC_WIDTH_MAX);
	//adc2_config_width(ADC_WIDTH_12);

	
}

void loop() {
    val[0] = adc1_get_raw(INPUT1);
    float val_f = 0 + (val[0] / 4096.0f * 5.0f);
	
    printf("Analog value is: %f\n",val_f);

		vTaskDelay(MS(1000));
}
