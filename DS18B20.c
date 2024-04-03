#include "DS18B20.h"

TemperatureDetector Thermometer0, Thermometer1;

TemperatureDetector *Thermometer[2] ={
    &Thermometer0,
    &Thermometer1,
};

char Dt_read(uint8_t data){
	Thermometer[data]->online = 0;
	Thermometer[data]->fail++;

	if(onewire_reset() == 1){
		onewire_reset();
		onewire_send(0x55);
		for(unsigned char i = 0; i <= 7; i++)
			onewire_send(Thermometer[data]->ROM[i]);

		onewire_send(0xBE);
		for(unsigned char i = 0; i <= 8; i++)
			Thermometer[data]->SCRATCHPAD[i] = onewire_read();
	}else{
		return 0;
	}

	if(Thermometer[data]->SCRATCHPAD[0] == 0x00 &&
	    Thermometer[data]->SCRATCHPAD[6] == 0x00){
		return 1;
	}

	if(Thermometer[data]->SCRATCHPAD[0] == 0xFF &&
	    Thermometer[data]->SCRATCHPAD[6] == 0xFF){
		return 1;
	}

	uint8_t crc = 0;
	for(uint8_t i = 0; i < 8; i++){
		crc = onewire_crc_update(crc, Thermometer[data]->SCRATCHPAD[i]);
	}

	if(!(crc == Thermometer[data]->SCRATCHPAD[8])) return 1;

	Thermometer[data]->online = 1;
	Thermometer[data]->fail--;

	return 0;
}

void Dt_calc(uint8_t data){
	Thermometer[data]->mud = Thermometer[data]->SCRATCHPAD[0];
	Thermometer[data]->mud |= (Thermometer[data]->SCRATCHPAD[1] << 8);

	if(Thermometer[data]->mud & (0b1111 << 12)){
		Thermometer[data]->mud = ~Thermometer[data]->mud;
		Thermometer[data]->minus = 1;
	}else{
		Thermometer[data]->minus = 0;
	}

	Thermometer[data]->temperOne = Thermometer[data]->mud >> 4;

	Thermometer[data]->temperTen = Thermometer[data]->mud & ~(0xFFF0);
	Thermometer[data]->temperTen = Thermometer[data]->temperTen * 6.25;
}

void DtConvertAll(){
	if(onewire_reset() == 1){
		onewire_send(0xCC);
		onewire_send(0x44);
	}
}

char DtConfRegAll(uint8_t bt){
	if(onewire_reset() == 1){
		onewire_send(0xCC);
		onewire_send(0x4E);
		onewire_send(0x00);
		onewire_send(0x00);
		onewire_send(bt);
	}

	if(onewire_reset() == 1){
		onewire_send(0xCC);
		onewire_send(0x48);
	}
	_delay_ms(100);
	return 0;
}