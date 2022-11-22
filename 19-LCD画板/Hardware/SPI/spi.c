#include "spi.h"
 
void SPI2_Init(void)
{	 
    gpio_config();
    SPI2_Config();
}   


void gpio_config(void) 
{
  RCC->APB2ENR|=1<<3;  	//PORTBʱ��ʹ�� 	 
  RCC->APB1ENR|=1<<14;   	//SPI2ʱ��ʹ�� 

	/*���� MISO MOSI SCK*/
	GPIOB->CRH &= 0X000FFFFF; // io�����ȸ�λ������ʹ��
	GPIOB->CRH |= 0XB8B00000; //	PB13 --- sck ���츴�����
  GPIOB->ODR |= 1<<13;
	GPIOB->ODR |= 1<<15;
}

void SPI2_Config(void) 
{
    SPI2->CR1 &= 0X0000;
    SPI2->CR1 |= 0<<15; // ˫��˫��ģʽ
    SPI2->CR1 |= 0<<11; // ʹ��8λ����֡��ʽ���з���/����
    SPI2->CR1 |= 0<<10; // ȫ˫��(���ͺͽ���)
    SPI2->CR1 |= 1<<9; // ����������豸����
    SPI2->CR1 |= 1<<8; // ���Q����NSS���ŵĵĵ�ƽ��0����NSSһֱ����͵�ƽ����ע�⣡��
    SPI2->CR1 |= 0<<7; // �ȷ���MSB

    SPI2->CR1 &= ~(7<<3); // fPCLK/2
    SPI2->CR1 |= 1<<2; // ����Ϊ���豸
    SPI2->CR1 |= 3<<0; //����״̬ʱ�� SCK���ָߵ�ƽ + ���ݲ����ӵڶ���ʱ�ӱ��ؿ�ʼ
    SPI2->CR1 |= 1<<6; // ����SPI�豸
}

/*���ͼĴ����Ƿ�Ϊ��
  1�����ͻ���Ϊ��
*/
u8 isTxEmpty() 
{
  return SPI2->SR & 0x2;
}

/*���ռĴ����Ƿ�Ϊ��
  1�����ջ���ǿ�
*/
u8 isRxNotEmpty() 
{
  return SPI2->SR & 0x1;
}

//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 data)
{		
  u8 result;
	u16 retry;
	while(!isTxEmpty()) 	// ���Ͳ�Ϊ��ִ�з���
  {
		retry++;
		if(retry>=0XFFFE)
			return 0; 	//��ʱ�˳�
	}			
	SPI2->DR = data;
	retry=0;
	while(!isRxNotEmpty()) // ���ݽ�����Ϊ��һֱ�ȴ���������
	{
		retry++;
		if(retry>=0XFFFE)
			return 0; 	//��ʱ�˳�
	}
	result = SPI2->DR;
	
  return result;		    
}
