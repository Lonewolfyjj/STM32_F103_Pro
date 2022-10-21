#ifndef _LED_H_
#define _LED_H_
#include "reg.h"
#include "typedef.h"
#define GPIOB_ODR_Address 0x40010C0C
#define GPIOE_ODR_Address 0x4001180C

#define LED_RED	  BA(GPIOB_ODR_Address, 5)	//定义PB5IO接口
#define LED_BLUE	BA(GPIOE_ODR_Address, 5)	//定义Pe5IO接口

void led_init(void);

#endif


