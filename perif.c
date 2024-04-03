#include "perif.h"

void EEPROM_UNLOCK(){
	FLASH_DUKR = 0xAE; // unlock EEPROM
	FLASH_DUKR = 0x56;

	while(!(FLASH_IAPSR & (1 << 3))); // check protection off
}

void EEPROM_LOCK(){ FLASH_IAPSR &= ~(1 << 3); }

void EEPROM_DATA_WRITE(uint8_t addr, uint8_t data){ EEPROM_DATA(addr) = data; }

uint8_t EEPROM_DATA_READ(uint8_t addr){ return EEPROM_DATA(addr); }
