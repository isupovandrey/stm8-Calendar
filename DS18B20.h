#ifndef DS18B20_H_
#define DS18B20_H_

#include "main.h"

typedef struct structTemperatureDetector{
	int mud;
	uint8_t temperOne;
	uint8_t temperTen;
	uint8_t fail;
	uint8_t minus;
	uint8_t online;
	uint8_t ROM[9];
	uint8_t SCRATCHPAD[9];
}TemperatureDetector;

extern TemperatureDetector Thermometer0, Thermometer1;
extern TemperatureDetector *Thermometer[2];

char Dt_read(uint8_t data);
void Dt_calc(uint8_t data);
void DtConvertAll();
char DtConfRegAll(uint8_t bt);

#endif /* DS18B20_H_ */