#ifndef _IIC_H_
#define _IIC_H_

#include "sys.h"

#define STM32_SLAVE_ADDRESS  0x78
#define EEPROM_ADDRESS 0xA0
uint8_t IIC_BUSY(void) ;
//主模式下 停止条件产生
void Send_STOP(void);
//主模式下 起始条件产生
void Send_START(void);

void IIC_ACK_Enable(u8 enable);

void send_write_byte(u8 addr,u8 dataValue) ;

u8 send_read_byte(u8 addr) ;
void IIC_Init(void);
#endif
