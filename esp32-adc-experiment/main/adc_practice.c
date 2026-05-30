#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"


#define ADC_CHANNEL ADC_CHANNEL_4
#define ADC_ATTEN ADC_ATTEN_DB_12

//function for calibration
static bool  adc_calibrate(adc_unit_t unit  , adc_atten_t atten, adc_cali_handle_t *out_handle){
	adc_cali_handle_t handle = NULL;
	esp_err_t ret = ESP_FAIL;
	bool calibrated = false;


	#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED

	adc_cali_line_fitting_config_t cali_config  = {
		.unit_id =  unit,
		.atten = atten,
		.bitwidth =ADC_BITWIDTH_12,
	};
	
	ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
	if(ret==ESP_OK){
		calibrated = true;
	}
	#endif

	*out_handle = handle;
	return calibrated;
}



void app_main(void)
{ //-------------adc1 iniT--------------
adc_oneshot_unit_handle_t adc1_handle;
adc_oneshot_unit_init_cfg_t init_config1 = {
	.unit_id = ADC_UNIT_1,
};

ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1,&adc1_handle));


//---------ADC1 CONFIG------
adc_oneshot_chan_cfg_t config ={
	.atten = ADC_ATTEN_DB_12,
	.bitwidth = ADC_BITWIDTH_DEFAULT,
};
ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle,ADC_CHANNEL,&config));

//adc calibrabation init
 adc_cali_handle_t  adc1_channel4_handle= NULL;
 
//adc calibartion 
bool succesful_calibration = adc_calibrate(ADC_UNIT_1,ADC_ATTEN,&adc1_channel4_handle);


	int raw =0;
	int voltage =0;
	while(1){
		//raw reading 
	adc_oneshot_read(adc1_handle,ADC_CHANNEL ,&raw);

		//output voltage with calibration
		if(succesful_calibration){
			ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_channel4_handle,raw,&voltage));

		}
		else{
			printf("unsuccessful calibartion\n");
		}


	
	printf("raw: %d ,  volatge: %d  \n",raw,voltage);
	vTaskDelay(pdMS_TO_TICKS(1000));



}


//deinitilisation of calibartion handle
if(succesful_calibration){
//ESP_LOGI(TAG, "delete %s calibration scheme", "Line Fitting");
ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(adc1_channel4_handle));
}


}
