#ifndef LCD_H_
#define LCD_H_

#include "main.h"

#define e1 PD_ODR |= (1 << LCD_E)  // установка линии E в 1
#define e0 PD_ODR &= ~(1 << LCD_E) // установка линии E в 0
#define rs1 PD_ODR |= (1 << LCD_RS) // установка линии RS в 1 (данные)
#define rs0 PD_ODR &= ~(1 << LCD_RS) // установка линии RS в 0 (команда)

extern uint8_t lcd_ram[4][20];

extern const char Cyrillic[23][8];

void sendHalfByte(uint8_t c);
void sendByte(uint8_t c, uint8_t mode);

void lcd_Init(void);
void lcd_clear();
void lcd_CGRAM(uint8_t *data);
void lcd_write();
void buff_clear();
void lcd_str(uint8_t x, uint8_t y, const char *str);
char CharHex0(uint8_t r0);
void CharHex(uint8_t x, uint8_t y, uint8_t r0);
void DisDec(uint8_t x, uint8_t y, uint8_t data);

void notify(uint8_t i);
uint8_t request(uint8_t ack);

#endif /* LCD_H_ */