#include "softI2C.h"

void i2c_init(void){
	PB_DDR &= ~(1 << SCL) | (1 << SDA);
	PB_CR1 |= (1 << SCL) | (1 << SDA);
	PB_CR2 |= (1 << SCL) | (1 << SDA);
}

void i2c_start(){
	SDA_0;
	I2C_DELAY;
	SCL_0;
}

void i2c_restart(){
	SDA_1;
	I2C_DELAY;
	SCL_1;
	I2C_DELAY;

	SDA_0;
	I2C_DELAY;
	SCL_0;
	I2C_DELAY;
}

void i2c_stop(){
	SDA_0;
	I2C_DELAY;
	SCL_1;
	I2C_DELAY;
	SDA_1;
}

uint8_t i2c_send(uint8_t data){
	uint8_t i = 8, ask;
	while(i--){

		if(data & (1 << i)){
			SDA_1;
		}else{
			SDA_0;
		}
		I2C_DELAY;
		SCL_1;
		I2C_DELAY;
		SCL_0;
	}
	SDA_1;
	I2C_DELAY;
	SCL_1;
	I2C_DELAY;
	ask = (SDA_IN & (1 << SDA));
	SCL_0; // спад
	return ask;
}

uint8_t i2c_read(uint8_t ack){
	uint8_t byte = 0, i = 8;

	while(i--){
		SCL_1;
		I2C_DELAY;
		if(SDA_IN & (1 << SDA)) byte |= (1 << i);
		SCL_0;
		I2C_DELAY;
	}

	if(ack){
		SDA_0;
	}else{
		SDA_1;
	}

	SCL_1;
	I2C_DELAY;
	SCL_0;
	I2C_DELAY;
	SDA_1;
	return byte;
}
