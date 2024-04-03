#include "ds1307.h"

struct Timekeeper RTC;

uint8_t RTC_ConvertFromDec(uint8_t c) {
  uint8_t ch = ((c >> 4) * 10 + (0b00001111 & c));
  return ch;
}

uint8_t RTC_ConvertFromBinDec(uint8_t c) {
  uint8_t ch = ((c / 10) << 4) | (c % 10);
  return ch;
}

void RTC_Read() {
  i2c_start();
  i2c_send(RTC_Addr | WRITE);
  i2c_send(0x00);
  i2c_stop();

  i2c_start();
  i2c_send(RTC_Addr | READ);
  RTC.sec	= i2c_read(ACK);
  RTC.min	= i2c_read(ACK);
  RTC.hour	= i2c_read(ACK);
  RTC.day	= i2c_read(ACK);
  RTC.date	= i2c_read(ACK);
  RTC.month = i2c_read(ACK);
  RTC.year	= i2c_read(NAK);
  i2c_stop();

  RTC.sec	= RTC_ConvertFromDec(RTC.sec);
  RTC.min	= RTC_ConvertFromDec(RTC.min);
  RTC.hour	= RTC_ConvertFromDec(RTC.hour);
  RTC.day	= RTC_ConvertFromDec(RTC.day);
  RTC.date	= RTC_ConvertFromDec(RTC.date);
  RTC.month = RTC_ConvertFromDec(RTC.month);
  RTC.year	= RTC_ConvertFromDec(RTC.year);
}

void RTC_Set() {
  i2c_start();
  i2c_send(RTC_Addr | WRITE);
  i2c_send(0x00);
  i2c_send(RTC_ConvertFromBinDec(RTC.sec));
  i2c_send(RTC_ConvertFromBinDec(RTC.min));
  i2c_send(RTC_ConvertFromBinDec(RTC.hour));
  i2c_send(RTC_ConvertFromBinDec(RTC.day));
  i2c_send(RTC_ConvertFromBinDec(RTC.date));
  i2c_send(RTC_ConvertFromBinDec(RTC.month));
  i2c_send(RTC_ConvertFromBinDec(RTC.year));
  i2c_stop();
}
