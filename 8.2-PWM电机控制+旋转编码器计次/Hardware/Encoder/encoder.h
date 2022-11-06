#ifndef _ENCODER_H_
#define _ENCODER_H_
#include "sys.h"

void Init_GPIO(void);
void Timer2_Init(void);
void Init_NVIC(u8 interrupt_channel); 
int16_t getEncoder(void);
void Init_Encoder(void);
#endif
