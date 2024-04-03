#ifndef SOFTI2C_H_
#define SOFTI2C_H_

#include "main.h"

#define READ 1
#define WRITE 0

#define ACK 1
#define NAK 0

#define I2C_DELAY delay(1)

#define SDA_1 PB_DDR &= ~(1 << SDA)
#define SDA_0 PB_DDR |= (1 << SDA)
#define SCL_1 PB_DDR &= ~(1 << SCL)
#define SCL_0 PB_DDR |= (1 << SCL)
#define SDA_IN PB_IDR

void i2c_init(void);
void i2c_start();
void i2c_restart();
void i2c_stop();
uint8_t i2c_send(uint8_t data);
uint8_t i2c_read(uint8_t ack);

#endif /* SOFTI2C_H_ */