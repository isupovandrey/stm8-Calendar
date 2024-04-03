#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include "main.h"

#define ONEWIRE_PORT PD_ODR
#define ONEWIRE_DDR PD_DDR
#define ONEWIRE_PIN PD_IDR
#define ONEWIRE_PIN_NUM 6

#define onewire_low ONEWIRE_DDR |= (1 << ONEWIRE_PIN_NUM)
#define onewire_high ONEWIRE_DDR &= ~(1 << ONEWIRE_PIN_NUM)
#define onewire_level (ONEWIRE_PIN & (1 << ONEWIRE_PIN_NUM))

// Переменные для хранения промежуточного результата поиска
extern uint8_t dev; // Найденных устройств
extern uint8_t onewire_enum[8]; // найденный восьмибайтовый адрес
extern uint8_t onewire_enum_fork_bit; // последний нулевой бит, где была
                                      // неоднозначность (нумеруя с единицы)

void onewire_init();
uint8_t onewire_reset();
void onewire_send_bit(uint8_t bit);
void onewire_send(uint8_t b);
uint8_t onewire_read_bit();
uint8_t onewire_read();
uint8_t onewire_crc_update(uint8_t crc, uint8_t b);
uint8_t onewire_skip();
uint8_t onewire_read_rom(uint8_t *buf);
uint8_t onewire_match(uint8_t *data);
void onewire_enum_init();
uint8_t *onewire_enum_next();

#endif /* ONEWIRE_H_ */
