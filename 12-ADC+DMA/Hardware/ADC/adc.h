#ifndef _ADC_H_
#define _ADC_H_
#include "sys.h"

#define ADC_CH1  1  				//ͨ��1(������PA1)	
extern unsigned short int adcRes;
u16 getADC(void);
void init_adc(void);
u16 Get_Adc_Average(void);
#endif
