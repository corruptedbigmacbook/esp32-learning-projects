#include <stdio.h>
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "tsl2561.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define I2C_MASTER_SCL_19 19
#define I2C_MASTER_SDA_18 18

#define TSL2561_ADDR 0x39

//-----------i2c master init ----------
esp_err_t i2c_master_init(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle){
	
	i2c_master_bus_config_t bus_config = {
		.i2c_port = -1,

		.sda_io_num = I2C_MASTER_SDA_18,
		.scl_io_num = I2C_MASTER_SCL_19,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup =true,
	};
	ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));

	i2c_device_config_t device_config ={
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = TSL2561_ADDR,
		.scl_speed_hz = 100000,
	};


	ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &device_config, dev_handle));		
	
	return ESP_OK;

}

//---------tsl2561 read write helper dunction ----------
//writing
static esp_err_t tsl2561_write(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint8_t data){

uint8_t write_buf[2] = {reg_addr|0x80, data};
return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), 1000);

		
}	


//write and read
static esp_err_t tsl2561_write_read(i2c_master_dev_handle_t dev_handle, uint8_t reg_addr, uint16_t *value){
	uint8_t data[2];
	uint8_t write_buf = reg_addr|0x20|0x80;
esp_err_t ret=  i2c_master_transmit_receive(dev_handle,&write_buf,1 , data, 2, 1000);
if(ret != ESP_OK){
	return ret;
}

*value = data[0] | (data[1] << 8);

return ESP_OK;
}


//CALCULATING LUX



void app_main(void)
{
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

ESP_ERROR_CHECK(i2c_master_init(&bus_handle, &dev_handle));

//tsl2561  init
//CONTROL REGISTER 
tsl2561_write(dev_handle,0x00,0x03); //power up


//TIMING REGISTER
tsl2561_write(dev_handle,0x01,0x12); //gain and integartion time
	vTaskDelay(pdMS_TO_TICKS(500));			     
while(1){
//reading from data registers
//DATA0 REGISTERS (my implementation is setting the word bit and reading 2 bytes sequentially starting from the first data0 register low -> high)
uint16_t channel0_value;
tsl2561_write_read(dev_handle,0x0C,&channel0_value);

//DATA1 REGISTERS (my implementation is setting the word bit and reading 2 bytes sequentially starting from the first data1 register low -> high )
uint16_t channel1_value;
tsl2561_write_read(dev_handle,0x0E,&channel1_value);

unsigned int lux = CalculateLux(1,2,channel0_value, channel1_value,0);
printf("lux: %d\n", lux);

vTaskDelay(pdMS_TO_TICKS(1000));
}

}
