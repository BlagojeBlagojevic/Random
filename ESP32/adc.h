// Creating adc 

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

//adc_oneshot_new_unit(&handle , config)   /Create instance of adc


// adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &raw); read value to raw as raw 
// Conver   LOW + (MAX_VALUE * raw / (2^numBits - 1) )
