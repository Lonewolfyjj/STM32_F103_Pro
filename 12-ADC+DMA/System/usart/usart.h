#ifndef _USART_H_
#define _USART_H_
#include "sys.h"
#include "stdio.h"
#define USART_REC_LENGTH  200  	//定义最大接收字节数 200
extern unsigned char USART_RX_BUFFER[USART_REC_LENGTH]; //接收缓冲,最大USART_REC_LEN个字节.

/*
 *接收状态	
 1:接收到0x0d 
 2:接收到0x0a 接收完成
*/
extern unsigned short USART_RECVICE_STA;
extern unsigned short R_LEN;

void usart_init(unsigned int pclk2,unsigned int bound);
void usart_send(char *s);

#endif
