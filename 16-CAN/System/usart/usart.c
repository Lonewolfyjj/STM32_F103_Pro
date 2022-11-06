#include "usart.h"
#include "reg.h"
#include "sys.h"
#include "typedef.h"	

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//printf�������ָ��fputc����fputc���������
//����ʹ�ô���1(USART1)���printf��Ϣ
int fputc(int ch, FILE *f)
{      
	while((USART_SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART_DR = (u8) ch;      	//дDR,����1����������
	return ch;
}
#endif 

unsigned char USART_RX_BUFFER[USART_REC_LENGTH];
unsigned short USART_RECVICE_STA = 0;
unsigned short R_LEN = 0;

void usart_init(u32 pclk2,u32 bound) 
{
	float temp;
	unsigned short int mantissa;
	unsigned short int fraction;	
  
	// �����ʹ̶����㣬�ο�25.3.4
	temp = (float)(pclk2*1000000 / (16*bound));
  mantissa = temp; //�õ���������
	fraction = (temp - mantissa)*16; // �õ�С������
	mantissa<<=4;
	mantissa+=fraction;
	
  RCC1->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC1->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	
	//A IO�˿�����
	GPIOA_CRH &= ~(0XFF<<4); // io��λ
	GPIOA_CRH |= 0x8b<<4; // PA10 ����������  PA9 �����������50Mhz ???
	
	RCC1->APB2RSTR |= 1<<14; // ��λ����1
  RCC1->APB2RSTR &= ~(1<<14);	// ֹͣ��λ����1
	
	USART_BRR = mantissa;   // 0-3 С��λ  4-15 ����λ 
	USART_CR1 |= 1<<13; // ����1 ʹ��
	USART_CR1 |= 0xc;   // ����ʹ�ܽ���ʹ��    
	USART_CR1 |= 1<<5;  //���ջ������ǿ��ж�ʹ��
	
	MY_NVIC_Init(3 , 3, USART1_IRQn, 2);//��2��������ȼ� 
}

void usart_send(char *s) {
	while(*s)
	{
		while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
    USART_DR = (u8) *s++;      	//дDR,����1����������
	}
}

void USART1_IRQHandler(void) 
{
	unsigned char res;	

	if(USART_SR & (1<<5))	//���յ�����
	{	 
		res = USART_DR; 
		if(USART_RECVICE_STA != 2) //����δ���
		{
			if(USART_RECVICE_STA == 1)//���յ���0x0d
			{
				if(res != 0x0a)
					USART_RECVICE_STA = 0; //���մ���,���¿�ʼ
				else
					USART_RECVICE_STA = 2;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(res == 0x0d)
					USART_RECVICE_STA = 1;
				else
				{
					USART_RX_BUFFER[R_LEN] = res;
					R_LEN++;
					if(R_LEN > (USART_REC_LENGTH - 1))
						USART_RECVICE_STA = 0;  //�������ݴ���,���¿�ʼ����	  
				}
			}
		}  		 									     
	}
}
