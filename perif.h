#ifndef PERIF_H_
#define PERIF_H_

#include "main.h"

#define EEPROM_DATA(address) *(volatile char *)(0x4000 + address)

void EEPROM_UNLOCK();
void EEPROM_LOCK();
void EEPROM_DATA_WRITE(uint8_t addr, uint8_t data);
uint8_t EEPROM_DATA_READ(uint8_t addr);

#endif /* PERIF_H_ */