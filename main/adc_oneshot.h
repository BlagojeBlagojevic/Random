//0 2 4 CHANELS INPUT
// 
//   GPIO_NUM_        -> PORT NUMBER
//   esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)
//   esp_err_t gpio_reset_pin(gpio_num_t gpio_num)
//   esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)
//   esp_err_t gpio_intr_enable(gpio_num_t gpio_num)
//   esp_err_t gpio_intr_disable(gpio_num_t gpio_num)
//   esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)
//   int gpio_get_level(gpio_num_t gpio_num)
//   esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
//   esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)
//   esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
//   esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num
//   esp_err_t gpio_isr_register(void (*fn)(void*), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle)
//   esp_err_t gpio_pullup_en(gpio_num_t gpio_num)
//   esp_err_t gpio_pullup_dis(gpio_num_t gpio_num)
//   esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)
//	 esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num)
//   esp_err_t gpio_install_isr_service(int intr_alloc_flags)
//   void gpio_uninstall_isr_service(void)
//   esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)
//   gpio_isr_handler_remove (gpio_num_t gpio_num)
//   esp_err_t gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
//   esp_err_t gpio_hold_en(gpio_num_t gpio_num)
//   esp_err_t gpio_hold_dis(gpio_num_t gpio_num)
//   void gpio_deep_sleep_hold_en(void)
//   void gpio_deep_sleep_hold_dis(void)
//   void gpio_iomux_in(uint32_t gpio_num, uint32_t signal_idx)
//   void gpio_iomux_out(uint8_t gpio_num, int func, bool oen_inv
//   esp_err_t gpio_sleep_sel_en(gpio_num_t gpio_num)
//   esp_err_t gpio_sleep_sel_dis(gpio_num_t gpio_num)
//   esp_err_t gpio_sleep_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)
//   esp_err_t gpio_sleep_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)
//   esp_err_t gpio_dump_io_configuration(FILE *out_stream, uint64_t io_bit_mask)
//    








#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
//#include "driver/adc.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"


#define MS(x) (x) /  portTICK_PERIOD_MS

#define INPUT1 ADC_CHANNEL_0
#define INPUT2 ADC_CHANNEL_2
#define INPUT3 ADC_CHANNEL_4
static int val[3];

void app_main(void){
	adc_oneshot_unit_handle_t adc1_handle;
	adc_oneshot_unit_init_cfg_t init_config1 = {
    	.unit_id = ADC_UNIT_1,
    	.ulp_mode = ADC_ULP_MODE_DISABLE,
};

adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_12,
};

ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

	while(1){
		//val[0] = adc1_get_raw(INPUT1);
		int raw;
		adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &raw);
		printf("Analog value is: %f",raw * 5.0f / 4096.0f );
		vTaskDelay(MS(1000));
	}

} 
