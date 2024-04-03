#include "onewire.h"

// Переменные для хранения промежуточного результата поиска
uint8_t dev;             // Найденных устройств
uint8_t onewire_enum[8]; // найденный восьмибайтовый адрес
uint8_t onewire_enum_fork_bit; // последний нулевой бит, где была
                               // неоднозначность (нумеруя с единицы)

/*

__asm__("NOP"); //2us
delay(1); //28us
delay(8); //68us
delay(16); //118us
delay(80); //500us
*/

void onewire_init(){
	ONEWIRE_PORT &= ~(1 << 6);
	ONEWIRE_DDR &= ~(1 << 6);
}

/*
    Выдаёт импульс сброса (reset), ожидает ответный импульс присутствия.
    Если импульс присутствия получен, дожидается его завершения и возвращает 1,
   иначе возвращает 0
*/
uint8_t onewire_reset(){
	char dt;
	ONEWIRE_DDR |= 1 << ONEWIRE_PIN_NUM; // притягиваем шину
	delay(80); // задержка как минимум на 480 микросекунд
	ONEWIRE_DDR &= ~(1 << ONEWIRE_PIN_NUM); // отпускаем шину
	delay(8); // задержка как максимум на 60 микросекунд
	if((ONEWIRE_PIN & (1 << ONEWIRE_PIN_NUM)) ==
	    0) // проверяем, ответит ли устройство
	{
		dt = 1; // устройство есть
	}else
		dt = 0; // устройства нет
	delay(80); // задержка как минимум на 480 микросекунд, но хватит и 420, тк
	           // это с учетом времени прошедших команд
	return dt; // вернем результат
}

void onewire_send_bit(uint8_t bit){
	ONEWIRE_DDR |= 1 << ONEWIRE_PIN_NUM; // притягиваем шину
	__asm__("NOP"); // задержка как минимум на 2 микросекунды
	if(bit) ONEWIRE_DDR &= ~(1 << ONEWIRE_PIN_NUM); // отпускаем шину
	delay(8); // задержка как минимум на 60 микросекунд
	ONEWIRE_DDR &= ~(1 << ONEWIRE_PIN_NUM); // отпускаем шину
}

void onewire_send(uint8_t b){
	char i;
	for(i = 0; i < 8; i++) // посылаем отдельно каждый бит на устройство
	{
		if((b & (1 << i)) == 1 << i) // посылаем 1
			onewire_send_bit(1);
		else // посылаем 0
			onewire_send_bit(0);
	}
}

uint8_t onewire_read_bit(){
	char bt; // переменная хранения бита
	ONEWIRE_DDR |= 1 << ONEWIRE_PIN_NUM; // притягиваем шину
	__asm__("NOP"); // задержка как минимум на 2 микросекунды
	ONEWIRE_DDR &= ~(1 << ONEWIRE_PIN_NUM); // отпускаем шину
	delay(1);
	bt = (ONEWIRE_PIN & (1 << ONEWIRE_PIN_NUM)) >> ONEWIRE_PIN_NUM; // читаем
	                                                                // бит
	delay(2);
	return bt; // вернем результат
}

uint8_t onewire_read(){
	char c = 0;
	char i;
	for(i = 0; i < 8; i++)
		c |= onewire_read_bit() << i; // читаем бит
	return c;
}

/*
    Обновляет значение контольной суммы crc применением всех бит байта b.
    Возвращает обновлённое значение контрольной суммы
*/
uint8_t onewire_crc_update(uint8_t crc, uint8_t b){
	for(uint8_t p = 8; p; p--){
		crc = ((crc ^ b) & 1) ? (crc >> 1) ^ 0b10001100 : (crc >> 1);
		b >>= 1;
	}
	return crc;
}

/*
    Выполняет последовательность инициализации (reset + ожидает импульс
   присутствия) Если импульс присутствия получен, выполняет команду SKIP ROM
*/
uint8_t onewire_skip(){
	if(!onewire_reset()) return 0;
	onewire_send(0xCC);

	return 1;
}

uint8_t onewire_read_rom(uint8_t *buf){
	if(!onewire_reset()) return 0;
	onewire_send(0x33);

	for(uint8_t p = 0; p < 8; p++){
		*(buf++) = onewire_read();
	}

	return 1;
}

uint8_t onewire_match(uint8_t *data){
	if(!onewire_reset()) return 0;
	onewire_send(0x55);

	for(uint8_t p = 0; p < 8; p++){
		onewire_send(*(data++));
	}

	return 1;
}

void onewire_enum_init(){
	dev = 0;

	for(uint8_t p = 0; p < 8; p++){
		onewire_enum[p] = 0;
	}

	onewire_enum_fork_bit = 65;
}

/*
    Перечисляет устройства на шине 1-wire и получает очередной адрес.
    Возвращает указатель на буфер, содержащий восьмибайтовое значение адреса,
   либо NULL, если поиск завешён
*/
uint8_t *onewire_enum_next(){
	if(!onewire_enum_fork_bit)
		return 0; // Если на предыдущем шаге уже не было разногласий

	if(!onewire_reset()) return 0;

	uint8_t bp = 8;
	uint8_t *pprev = &onewire_enum[0];
	uint8_t prev = *pprev;
	uint8_t next = 0;

	uint8_t p = 1;
	onewire_send(0xF0);
	uint8_t newfork = 0;

	while(1){
		uint8_t not0 = onewire_read_bit();
		uint8_t not1 = onewire_read_bit();
		if(!not0){ // Если присутствует в адресах бит ноль
			if(!not1){ // Но также присустствует бит 1 (вилка)
				if(p < onewire_enum_fork_bit){ // Если мы левее прошлого
					                            // правого конфликтного бита,
					if(prev & 1){
						next |= 0x80; // то копируем значение бита из прошлого
						              // прохода
					}else{
						newfork = p; // если ноль, то запомним конфликтное место
					}
				}else if(p == onewire_enum_fork_bit){
					next |= 0x80; // если на этом месте в прошлый раз был правый
					              // конфликт с нулём, выведем 1
				}else{
					newfork = p; // правее - передаём ноль и запоминаем
					             // конфликтное место
				}
			}// в противном случае идём, выбирая ноль в адресе
		}else{
			if(!not1){ // Присутствует единица
				next |= 0x80;
			}else{ // Нет ни нулей ни единиц - ошибочная ситуация
				return 0;
			}
		}

		onewire_send_bit(next & 0x80);
		bp--;

		if(!bp){
			*pprev = next;
			if(p >= 64) break;
			next = 0;
			pprev++;
			prev = *pprev;
			bp = 8;
		}else{
			if(p >= 64) break;
			prev >>= 1;
			next >>= 1;
		}

		p++;
	}

	dev++;
	onewire_enum_fork_bit = newfork;
	return &onewire_enum[0];
}
