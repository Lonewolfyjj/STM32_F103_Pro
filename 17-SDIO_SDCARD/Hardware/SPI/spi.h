#ifndef _SPI_H_
#define _SPI_H_
#include "sys.h"
			  	    													  
void SPI2_Init(void);			 //初始化SPI2口

u8 SPI2_ReadWriteByte(u8 data);//SPI2总线读写一个字节

void gpio_config(void) ;

void SPI2_Config(void) ;

#endif
