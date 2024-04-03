#include "lcd.h"

uint8_t lcd_ram[4][20];

const char Cyrillic[23][8] ={
   {0x1F, 0x10, 0x10, 0x1E, 0x11, 0x11, 0x1E, 0x00}, // 0  Б
   {0x1F, 0x11, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}, // 1	Г
   {0x0F, 0x05, 0x05, 0x09, 0x11, 0x1F, 0x11, 0x00}, // 2	Д
   {0x15, 0x15, 0x15, 0x0E, 0x15, 0x15, 0x15, 0x00}, // 3	Ж
   {0x1E, 0x01, 0x01, 0x02, 0x01, 0x01, 0x1F, 0x00}, // 4	З
   {0x11, 0x11, 0x13, 0x15, 0x19, 0x11, 0x11, 0x00}, // 5	И
   {0x15, 0x11, 0x13, 0x15, 0x19, 0x11, 0x11, 0x00}, // 6	Й
   {0x0F, 0x05, 0x05, 0x05, 0x05, 0x15, 0x09, 0x00}, // 7	Л
   {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00}, // 8	П
   {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00}, // 9	Т
   {0x11, 0x11, 0x11, 0x0A, 0x04, 0x08, 0x10, 0x00}, // 10	У
   {0x04, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x04, 0x00}, // 11	Ф
   {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1f, 0x01}, // 12	Ц
   {0x11, 0x11, 0x11, 0x0F, 0x01, 0x01, 0x01, 0x00}, // 13	Ч
   {0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x1F, 0x00}, // 14	Ш
   {0x15, 0x15, 0x15, 0x15, 0x15, 0x1F, 0x01, 0x00}, // 15	Щ
   {0x18, 0x08, 0x08, 0x0E, 0x09, 0x09, 0x0E, 0x00}, // 16	Ъ
   {0x11, 0x11, 0x11, 0x19, 0x15, 0x15, 0x19, 0x00}, // 17	Ы
   {0x08, 0x08, 0x08, 0x0E, 0x09, 0x09, 0x0E, 0x00}, // 18	Ь
   {0x1E, 0x01, 0x01, 0x03, 0x01, 0x01, 0x1E, 0x00}, // 19	Э
   {0x12, 0x15, 0x15, 0x1D, 0x15, 0x15, 0x12, 0x00}, // 20	Ю
   {0x0F, 0x11, 0x11, 0x0F, 0x05, 0x09, 0x11, 0x00}, // 21	Я
   {0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 22 Верхний курсор
};

void sendHalfByte(uint8_t c){
	c <<= 4;
	e1; // включаем линию Е
	delay(5);
	PC_ODR &= 0b00001111; // стираем информацию на входах DB4-DB7, остальное не трогаем                 
	PC_ODR |= c;
	e0; // выключаем линию Е
	delay(5);
}

void sendByte(uint8_t c, uint8_t mode){
	if(mode == 0)
		rs0;
	else
		rs1;
	uint8_t hc = 0;
	hc = c >> 4;
	sendHalfByte(hc);
	sendHalfByte(c);
}

void lcd_Init(void){
	_delay_ms(50); // Ждем 15 мс (стр 45)
	sendHalfByte(0b00000011);
	_delay_ms(1);
	sendHalfByte(0b00000011);
	_delay_ms(1);
	sendHalfByte(0b00000011);
	_delay_ms(1);
	sendHalfByte(0b00000010);
	_delay_ms(1);
	sendByte(0b00101000, 0); // 4бит-режим (DL=0) и 2 линии (N=1)
	_delay_ms(1);
	sendByte(0b00001000 | (0b100 << 0), 0); // включаем изображение на дисплее (D=1), курсоры никакие не включаем (C=0, B=0)                                  
	_delay_ms(1);
	sendByte(0b00000100 | (0b10 << 0), 0); // курсор (хоть он у нас и невидимый) будет двигаться влево
	_delay_ms(1);
	lcd_clear();
}

void lcd_clear(){
	sendByte(0b00000001, 0);
	_delay_ms(2);
}

void lcd_CGRAM(uint8_t *data){
	for(uint8_t i = 0; i < 8; i++)
		sendByte(data[i], 1);
}

void lcd_write(){
	sendByte(0x80 | 0x00, 0);
	for(uint8_t i = 0; i < 20; i++)
		sendByte(lcd_ram[0][i], 1);
	sendByte(0x80 | 0x40, 0);
	for(uint8_t i = 0; i < 20; i++)
		sendByte(lcd_ram[1][i], 1);
	sendByte(0x80 | 0x14, 0);
	for(uint8_t i = 0; i < 20; i++)
		sendByte(lcd_ram[2][i], 1);
	sendByte(0x80 | 0x54, 0);
	for(uint8_t i = 0; i < 20; i++)
		sendByte(lcd_ram[3][i], 1);
}

void buff_clear(){
	for(uint8_t i = 0; i < 80; i++)
		lcd_ram[0][i] = 0x20;
}

void lcd_str(uint8_t x, uint8_t y, const char *str){
	while(*str)
		lcd_ram[y][x++] = *str++;
}

char CharHex0(uint8_t r0){
	if(r0 < 10)
		return r0 + 48;
	else
		return (r0 + 7) + 48;
}

void CharHex(uint8_t x, uint8_t y, uint8_t r0){
	lcd_ram[y][x++] = CharHex0(HIGH(r0));
	lcd_ram[y][x] = CharHex0(LOW(r0));
}

void DisDec(uint8_t x, uint8_t y, uint8_t data){
	lcd_ram[y][x++] = ((data / 10) / 10) + '0';
	lcd_ram[y][x++] = ((data / 10) % 10) + '0';
	lcd_ram[y][x++] = (data % 10) + '0';
}