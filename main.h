#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 16000000UL

#include <stdint.h>
#include <string.h>

// LCD display
#define PWM_LED TIM1_CCR3L
#define LCD_LED 3    // portC PWM
#define LCD_4bit 0xF // portC

#define LCD_E 2  // portD
#define LCD_RS 3 // portD

// BUTTONS
#define KEY_UP 3 // portA
#define KEY_DN 1 // portA
#define KEY_OK 2 // portA

// I2C
#define SCL 4 // portB
#define SDA 5 // portB

// LDR
#define LDR 5 // portD

// 1-WIRE
#define ONEWIRE 6 // portD

#define HIGH(x) ((x) >> 4)
#define LOW(x) ((x) & 0x0F)

#include "DS18B20.h"
#include "ds1307.h"
#include "lcd.h"
#include "onewire.h"
#include "other.h"
#include "perif.h"
#include "softI2C.h"
#include "stm8s.h"

#endif /* MAIN_H_ */