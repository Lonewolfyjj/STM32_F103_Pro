#ifndef _USART_H_
#define _USART_H_
#include "sys.h"
#include "stdio.h"
#define USART_REC_LENGTH  200  	//�����������ֽ��� 200
extern unsigned char USART_RX_BUFFER[USART_REC_LENGTH]; //���ջ���,���USART_REC_LEN���ֽ�.

/*
 *����״̬	
 1:���յ�0x0d 
 2:���յ�0x0a �������
*/
extern unsigned short USART_RECVICE_STA;
extern unsigned short R_LEN;

void usart_init(unsigned int pclk2,unsigned int bound);
void usart_send(char *s);

#endif
