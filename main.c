#include "main.h"

uint8_t But_Buff = 0xFF;
uint8_t ADCMin = 0;
uint8_t BRMin = 0;

#define EEPROM_ADR_ADCMin 0
#define EEPROM_ADR_BRMin 1
#define EEPROM_ADR_ROM0 10
#define EEPROM_ADR_ROM1 18

const char *allMonths[12] ={
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

void prog();
void menu();
void editRTC();
void editLDR();
void editDDT();
void BRregular();
void EEPROM_READ_TORAM();
void notify(uint8_t i);
uint8_t request(uint8_t ack);
void StrTemp(uint8_t x, uint8_t y, uint8_t data);
inline void PWM_Init();
inline void ADC_init();
uint8_t ADC_convert();
char button(uint8_t but);
void weekdayRus(uint8_t day);
void test();

int main(void){
	FREQ_INIT();

	PC_ODR |= (1 << LCD_LED);

	PC_DDR |= (LCD_4bit << 4) | (1 << LCD_LED);
	PC_CR1 |= (LCD_4bit << 4) | (1 << LCD_LED);

	PD_DDR |= (1 << LCD_E) | (1 << LCD_RS);
	PD_CR1 |= (1 << LCD_E) | (1 << LCD_RS);

	PA_DDR &= ~(1 << KEY_UP) | (1 << KEY_DN) | (1 << KEY_OK);
	PA_CR1 |= (1 << KEY_UP) | (1 << KEY_DN) | (1 << KEY_OK);

	PWM_Init();
	ADC_init();
	lcd_Init();
	i2c_init();
	DtConfRegAll(0x7F);

	PWM_LED = 100;

	while(1){
		prog();
	}
}

void prog(){
	uint8_t point = 0;
	uint8_t check = 0;
	uint8_t softTimer = 0;
	EEPROM_READ_TORAM();

	while(1){
		buff_clear();
		RTC_Read();

		if(button(KEY_OK)) menu();

		weekdayRus(RTC.day - 1);

		#define clockX 15
		#define clockY 1
		if(RTC.hour > 9) lcd_ram[clockY][0 + clockX] = RTC.hour/10 + '0';
		lcd_ram[clockY][1 + clockX] = RTC.hour % 10 + '0';
		lcd_ram[clockY][2 + clockX] = ':';
		lcd_ram[clockY][3 + clockX] = RTC.min/10 + '0';
		lcd_ram[clockY][4 + clockX] = RTC.min % 10 + '0';

		uint8_t temp = 0;
		if(RTC.date > 9) lcd_ram[1][temp++] = (RTC.date/10) + '0';
		lcd_ram[1][temp++] = (RTC.date % 10) + '0';
		lcd_ram[1][temp++] = ' ';
		lcd_str(temp, 1, allMonths[RTC.month - 1]);

		softTimer++;
		if(softTimer > 0x4F){
			switch(check){
				case 0:
					DtConvertAll();
					break;
				case 1:
					Dt_read(0);
					break;
				case 2:
					Dt_read(1);
					break;
				case 3:
					Dt_calc(0);
					Dt_calc(1);
					break;
				case 4:
					BRregular();
					break;
			}

			check++;
			if(check > 4) check = 0;
			softTimer = 0;
		}



		if(Thermometer[0]->online) StrTemp(0, 3, 0);
		if(Thermometer[1]->online) StrTemp(15, 3, 1);

		lcd_write();
	}
}

void menu(){
	uint8_t point = 0;

	while(1){
		buff_clear();

		if(button(KEY_UP)){
			point++;
			if(point > 2) point = 0;
		}

		if(button(KEY_DN)){
			point--;
			if(point > 2) prog();
		}

		if(button(KEY_OK)){
			switch(point){
				case 0:
					editRTC();
					break;
				case 1:
					editLDR();
					break;
				case 2:
					editDDT();
					break;
			}
		}

		lcd_str(0, 0, "Edit:");
		lcd_str(1, 1, "RTC");
		lcd_str(1, 2, "LDR");
		lcd_str(1, 3, "DDT");

		lcd_ram[1 + point][0] = '>';

		lcd_write();
	}
}

void editRTC(){
	uint8_t point = 0;
	uint8_t check = 0;

	RTC_Read();

	sendByte(0x40, 0);
	lcd_CGRAM(Cyrillic[22]); // Верхний курсор \x08

	while(1){
		buff_clear();

		if(button(KEY_UP)){
			if(check){
				switch(point){
					case 0:
						RTC.hour++;
						if(RTC.hour > 23) RTC.hour = 0;
						break;
					case 1:
						RTC.min++;
						if(RTC.min > 59) RTC.min = 0;
						break;
					case 2:
						RTC.date++;
						if(RTC.date > 31) RTC.date = 1;
						break;
					case 3:
						RTC.month++;
						if(RTC.month > 12) RTC.month = 1;
						break;
					case 4:
						RTC.year++;
						if(RTC.year > 99) RTC.year = 0;
						break;
					case 5:
						RTC.day++;
						if(RTC.day > 7) RTC.day = 1;
						break;
				}
			}else{
				point++;
			}

			if(point > 5){
				RTC.sec = 0;
				RTC_Set();
				notify(0);
				prog();
			}
		}

		if(button(KEY_DN)){
			if(check){
				switch(point){
					case 0:
						RTC.hour--;
						if(RTC.hour > 23) RTC.hour = 23;
						break;
					case 1:
						RTC.min--;
						if(RTC.min > 59) RTC.min = 59;
						break;
					case 2:
						RTC.date--;
						if(RTC.date == 0) RTC.date = 31;
						break;
					case 3:
						RTC.month--;
						if(RTC.month == 0) RTC.month = 12;
						break;
					case 4:
						RTC.year--;
						if(RTC.year > 99) RTC.year = 99;
						break;
					case 5:
						RTC.day--;
						if(RTC.day == 0) RTC.day = 7;
						break;
				}
			}else{
				point--;
				if(point > 5){
					prog();
				}
			}
		}

		if(button(KEY_OK)){
			check ^= (1 << 0);
		}

		lcd_ram[0][0] = (RTC.hour/10) + '0';
		lcd_ram[0][1] = (RTC.hour % 10) + '0';
		lcd_ram[0][2] = ':';
		lcd_ram[0][3] = (RTC.min/10) + '0';
		lcd_ram[0][4] = (RTC.min % 10) + '0';

		lcd_ram[0][6] = (RTC.date/10) + '0';
		lcd_ram[0][7] = (RTC.date % 10) + '0';
		lcd_ram[0][8] = '.';
		lcd_ram[0][9] = (RTC.month/10) + '0';
		lcd_ram[0][10] = (RTC.month % 10) + '0';
		lcd_ram[0][11] = '.';
		lcd_ram[0][12] = (RTC.year/10) + '0';
		lcd_ram[0][13] = (RTC.year % 10) + '0';

		lcd_str(15, 0, "day:");
		lcd_ram[0][19] = (RTC.day) + '0';

		lcd_ram[1][point * 3] = check ? '^' : '\x08';
		lcd_ram[1][point * 3 + 1] = check ? '^' : '\x08';

		lcd_write();
	}
}

void editLDR(){
	uint8_t point = 0;
	uint8_t check = 0;
	uint8_t ADCresult = 0;

	EEPROM_READ_TORAM();

	while(1){
		buff_clear();

		if(!check){
			ADCresult = ADC_convert();
			BRregular();
		}

		if(button(KEY_UP)){
			if(check){
				switch(point){
					case 1:
						BRMin++;
						PWM_LED = BRMin;
					break;
				}
			}else{
				point++;
				if(point > 1) point = 0;
			}
		}

		if(button(KEY_DN)){
			if(check){
				switch(point){
					case 1:
						BRMin--;
						PWM_LED = BRMin;
					break;
				}
			}else{
				point--;
				if(point > 1){
					EEPROM_UNLOCK();
					EEPROM_DATA_WRITE(EEPROM_ADR_ADCMin, ADCMin);
					EEPROM_DATA_WRITE(EEPROM_ADR_BRMin, BRMin);
					EEPROM_LOCK();
					EEPROM_READ_TORAM();
					notify(0);
					prog();
				}
			}
		}

		if(button(KEY_OK)){
			check ^= (1 << 0);
			switch(point){
				case 0:
					ADCMin = ADCresult;
					check = 0;
				break;
			}
		}

		DisDec(17, 0, ADCresult);

		lcd_str(1, 1, "ADC min:");
		lcd_str(1, 2, "BR  min:");

		DisDec(10, 1, ADCMin);
		DisDec(10, 2, BRMin);

		lcd_ram[point + 1][0] = '>';

		lcd_write();
	}
}

void editDDT(){
	uint8_t point = 0;
	EEPROM_READ_TORAM();

EditDDTL:
	while(1){
		buff_clear();
		Dt_read(point);

		if(button(KEY_UP)){
			point++;
			if(point > 1) point = 0;
		}

		if(button(KEY_DN)){
			point--;
			if(point > 1){
				prog();
			}
		}

		if(button(KEY_OK)){
			buff_clear();
			lcd_str(0, 0, "Update?");

			if(request(1) == 0){
				onewire_enum_init();
				onewire_enum_next();

				while(1){
					buff_clear();

					if(button(KEY_UP)){
						if(!onewire_enum_next()){
							onewire_enum_init();
						}
					}

					if(button(KEY_DN)){
						EEPROM_UNLOCK();
						if(point == 0){
							for(uint8_t i = 0; i <= 7; i++)
								EEPROM_DATA_WRITE(
								    EEPROM_ADR_ROM0 + i, onewire_enum[i]);
						}

						if(point == 1){
							for(uint8_t i = 0; i <= 7; i++)
								EEPROM_DATA_WRITE(
								    EEPROM_ADR_ROM1 + i, onewire_enum[i]);
						}
						EEPROM_LOCK();
						EEPROM_READ_TORAM();
						notify(0);
						goto EditDDTL;
					}

					if(button(KEY_OK)) break;

					lcd_str(0, 0, "Update");
					lcd_str(0, 3, " Save   Exit      > ");

					uint8_t crc = 0;
					for(uint8_t i = 0; i < 7; i++){
						crc = onewire_crc_update(crc, onewire_enum[i]);
					}

					lcd_str(0, 2, "ROM");
					if(crc == onewire_enum[7]){
						for(uint8_t i = 0; i <= 7; i++)
							CharHex(4 + (i * 2), 2, onewire_enum[i]);
					}else{
						lcd_str(0, 2, "Error CRC!"); // Ошибка CRC
					}

					lcd_write();
				}
			}
		}

		if(point){
			lcd_str(0, 0, "Street");
		}else{
			lcd_str(0, 0, "Home");
		}

		if(Thermometer[point]->online){
			lcd_str(12, 0, "[Online]");
		}else{
			lcd_str(11, 0, "[Offline]");
		}

		if(Thermometer[point]->fail){
			lcd_str(0, 1, "Error:");
			DisDec(7, 1, Thermometer[point]->fail);
		}

		lcd_str(0, 3, "S");
		for(uint8_t i = 0; i <= 8; i++)
			CharHex(2 + (i * 2), 3, Thermometer[point]->SCRATCHPAD[i]);

		lcd_str(0, 2, "ROM");
		for(uint8_t i = 0; i <= 7; i++)
			CharHex(4 + (i * 2), 2, Thermometer[point]->ROM[i]);

		lcd_write();
	}
}

void BRregular(){
	if(ADC_convert() < ADCMin){
		PWM_LED = BRMin;
	}else{
		PWM_LED = 100;
	}
}

void EEPROM_READ_TORAM(){
	ADCMin = EEPROM_DATA(EEPROM_ADR_ADCMin);
	BRMin = EEPROM_DATA(EEPROM_ADR_BRMin);

	for(uint8_t i = 0; i < 8; i++)
		Thermometer[0]->ROM[i] = EEPROM_DATA(EEPROM_ADR_ROM0 + i);
	for(uint8_t i = 0; i < 8; i++)
		Thermometer[1]->ROM[i] = EEPROM_DATA(EEPROM_ADR_ROM1 + i);
}

void notify(uint8_t i){
	buff_clear();
	lcd_str(0, 0, "Save");

	switch(i){
		case 0:
			lcd_str(0, 0, "Save");
			break;
		case 1:
			lcd_str(0, 0, "Error");
			break;
	}

	lcd_write();
	_delay_ms(500);
}

uint8_t request(uint8_t ack){
	switch(ack){
		case 0:
			lcd_str(3, 3, "Yes         No");
			break;
		case 1:
			lcd_str(3, 3, "Yes  Exit   No");
			break;
		case 2:
			lcd_str(2, 3, "Save        Exit");
			break;
		case 3:
			lcd_str(2, 3, "Save  Exit  Exit");
			break;
	}

	lcd_write();
	while(1){
		if(button(KEY_DN)) return 0;
		if(button(KEY_UP)) return 1;
		if(button(KEY_OK)) return 2;
	}
}

void StrTemp(uint8_t x, uint8_t y, uint8_t data){
	x += 4;
	lcd_ram[y][x--] = (Thermometer[data]->temperTen/10) + '0';
	lcd_ram[y][x--] = '.';
	lcd_ram[y][x--] = (Thermometer[data]->temperOne % 10) + '0';
	if(Thermometer[data]->temperOne > 9)
		lcd_ram[y][x--] = (Thermometer[data]->temperOne/10) + '0';
	if(Thermometer[data]->minus) lcd_ram[y][x--] = '-';
}

inline void PWM_Init(){
	TIM1_CR1 |= (1 << 4); // Counter used as down-counter

	TIM1_ARRH = 0; // Максимальное значение
	TIM1_ARRL = 100;

	TIM1_PSCRH = 0; // Делитель
	TIM1_PSCRL = 2;

	TIM1_RCR = 0; // Continuous

	TIM1_CCMR3 = (0b110 << 4) | (1 << 3);      // PWM mode 1 CH3 active while CNT<CCR3
	TIM1_CCER2 = (1 << 0) | (1 << 1); // 0x01 - CH3 compare

	TIM1_CR1 |= (1 << 0); // Enable timer
	TIM1_BKR |= (1 << 7); // OC and OCN outputs are enabled if their respective enable bits are set (CC/E in Tim1_CCERi)
}

inline void ADC_init(){
	ADC1_CSR |= 5;
	ADC1_CR1 |= (0b010 << 4) | (1 << 0);
}

uint8_t ADC_convert(){
	uint8_t adcH;
	ADC1_CR1 |= (1 << 0);
	while(!(ADC1_CSR & (1 << 7)));
	adcH = ADC1_DRH;
	ADC1_CSR &= ~(1 << 7); // Clear EOC flag
	return adcH;
}

char button(uint8_t but){
	if(!(PA_IDR & (1 << but))){
		_delay_ms(1);
		if(But_Buff & (1 << but)){
			But_Buff &= ~(1 << but);
			return 1;
		}
	}else
		But_Buff |= (1 << but);
	return 0;
}

void weekdayRus(uint8_t day){
	switch(day){
		case 0:
		case 1:
		case 2:
		case 3:
			sendByte(0x40, 0);
			lcd_CGRAM(Cyrillic[8]);  // П  \x08
			lcd_CGRAM(Cyrillic[2]);  // Д  \001
			lcd_CGRAM(Cyrillic[7]);  // Л  \002
			lcd_CGRAM(Cyrillic[18]); // Ь  \003
			lcd_CGRAM(Cyrillic[5]);  // И  \004
			lcd_CGRAM(Cyrillic[13]); // Ч  \005
			lcd_CGRAM(Cyrillic[1]);  // Г  \006
			break;
		case 4:
		case 5:
		case 6:
			sendByte(0x40, 0);
			lcd_CGRAM(Cyrillic[8]);  // П  \x08
			lcd_CGRAM(Cyrillic[21]); // Я  \001
			lcd_CGRAM(Cyrillic[5]);  // И  \002
			lcd_CGRAM(Cyrillic[12]); // И  \003
			lcd_CGRAM(Cyrillic[10]); // У  \004
			lcd_CGRAM(Cyrillic[0]);  // Б  \005
			break;
	}

	switch(day){
		case 0:
			lcd_str(10 - (12/2), 0, "\x08OHE\001E\002\003H\004K");
			break;
		case 1:
			lcd_str(10 - (8/2), 0, "BTOPH\004K");
			break;
		case 2:
			lcd_str(10 - (6/2), 0, "CPE\001A");
			break;
		case 3:
			lcd_str(10 - (8/2), 0, "\005ETBEP\006");
			break;
		case 4:
			lcd_str(10 - (8/2), 0, "\x08\001TH\002\003A");
			break;
		case 5:
			lcd_str(10 - (8/2), 0, "C\004\005\005OTA");
			break;
		case 6:
			lcd_str(10 - (12/2), 0, "BOCKPECEH\002E");
			break;
	}
}

void test(){
	uint8_t point = 0;

	while(1){
		buff_clear();

		lcd_write();
	}
}