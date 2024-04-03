#ifndef OTHER_H_
#define OTHER_H_

#include "main.h"


#define F_SUM 182UL

#if F_CPU == 16000000UL
   #define FREQ_INIT() CLK_CKDIVR = (0b11<<3) | (0b000<<0)
   #define _delay_ms(t) delay(t * F_SUM)
#elif F_CPU == 8000000UL
   #define FREQ_INIT() CLK_CKDIVR = (0b11<<3) | (0b001<<0)
   #define _delay_ms(t) delay(t * (F_SUM/2))
#elif F_CPU == 4000000UL
   #define FREQ_INIT() CLK_CKDIVR = (0b11<<3) | (0b010<<0)
   #define _delay_ms(t) delay(t * (F_SUM/4))
#elif F_CPU == 2000000UL
   #define FREQ_INIT() CLK_CKDIVR = (0b11<<3) | (0b011<<0)  
   #define _delay_ms(t) delay(t * (F_SUM/8))
#elif F_CPU == 1000000UL
   #define FREQ_INIT() CLK_CKDIVR = (0b11<<3) | (0b100<<0)  
   #define _delay_ms(t) delay(t * (F_SUM/16))   
#endif


inline void delay(uint32_t t){
   while(t--);
}



#endif /* OTHER_H_ */