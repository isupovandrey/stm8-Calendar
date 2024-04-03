#ifndef DS1307_H_
#define DS1307_H_

#include "main.h"

#define RTC_Addr 0xD0

struct Timekeeper{
	uint8_t sec, min, hour, day, date, month, year;
};

extern struct Timekeeper RTC;

uint8_t RTC_ConvertFromDec(uint8_t c);
uint8_t RTC_ConvertFromBinDec(uint8_t c);
void RTC_Read();
void RTC_Set();

#endif /* DS1307_H_ */