#include "iic.h"
#include "sys.h"
#include "usart.h"

void IIC_Init()
{
	RCC->APB2ENR |= 1<<3; //ʹ��GPIOBʱ��
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  //ʹ��I2C1ʱ��
	RCC->APB1ENR |= 1<<21;
	GPIOB->CRL |= 0xFF000000; // PB6 pb7���ù��ܿ�©���ģʽ 50MHZ

	// ����IIC ��ʱ��Ƶ��36M
	I2C1->CR2 |= 0x24<<0;

	I2C1->OAR1 |= 1<14;// ����ʼ�����������Ϊ��1����

	I2C1->CCR |= 1<15; // ����IIC
	I2C1->CCR &= ~(1<14); // Tlow/Thigh = 2
	I2C1->CCR |= 30; //��ģʽ�µĿ���ģʽ�ķ�Ƶϵ��,���ڲ���400Khz�ĸ���ͨ��Ƶ��

	I2C1->TRISE |= 91;

	I2C1->OAR1 |= STM32_SLAVE_ADDRESS; // �趨STM32��Ϊ�ӻ��ĵ�ַ 	

	IIC_ACK_Enable(1);
	I2C1->CR1 |= 1<<0; //ʹ��IIc
}

void send_write_byte(u8 addr,u8 dataValue)
{
	u16 temp;
	while(IIC_BUSY());
	Send_START();
	while((I2C1->SR1 & 0x01) == 0); //1 ��ʼ�����ѷ���
	
	I2C1->DR = ( EEPROM_ADDRESS); // ��ַ + д 
  while((I2C1->SR1 & 0x0002) == 0); // 1 �����ַ�������
  temp = I2C1->SR2; // �����SR2�Ĵ���
	
	I2C1->DR = addr; // дEEPROM ��ַ���� 
	while((I2C1->SR1 & 0x0080) == 0);//  1�����ݼĴ�����
	
  I2C1->DR |= dataValue; // дEEPROM ��ַ���� 
	while((I2C1->SR1 & 0x0080) == 0);//  1�����ݼĴ�����

  Send_STOP();
}

u8 send_read_byte(u8 addr)
{
	u8 dataValue,temp;

	while(IIC_BUSY());

	Send_START();
	while((I2C1->SR1 & 0x01) == 0); //1 ��ʼ�����ѷ���

	I2C1->DR = (EEPROM_ADDRESS); //�����豸��ַ+���ź�
	while((I2C1->SR1 & 0x0002) == 0); // 1 �����ַ�������
	temp = I2C1->SR2; // �����SR2�Ĵ���

	I2C1->DR = addr;//���ʹ洢��Ԫ��ַ
	while((I2C1->SR1 & 0x0080) == 0);//  1�����ݼĴ����ǿ�

	Send_START();
	while((I2C1->SR1 & 0x01) == 0); //1 ��ʼ�����ѷ���

	I2C1->DR = (EEPROM_ADDRESS | 0X01); //�����豸��ַ+���ź�
	while((I2C1->SR1 & 0x0002) == 0); // 1 �����ַ�������
	temp = I2C1->SR2; // �����SR2�Ĵ���

	IIC_ACK_Enable(0);
	Send_STOP();

	while(!(I2C1->SR1 & 1<<6));  // ���ݼĴ�����Ϊ��
	dataValue=I2C1->DR;//�����Ĵ�������

	return dataValue;
}

/*
 * ��ģʽ�µ� Ӳ��IIC ��ʼ��
 */
void IIC_ACK_Enable(u8 enable) {
	if(enable) 
	{
    I2C1->CR1 |= 1<<10; // Ӧ��ʹ�� 
	} 
	else 
  {
	  I2C1->CR1 &= ~(1<<10); // Ӧ��disable
	}
}

/*
IIC æ��־
I2C_CR1[15] 0:��������������ͨѶ��,1:�����������ڽ�������ͨѶ
*/
uint8_t IIC_BUSY() {

 return I2C1->SR2 & 1<<1;
}

//��ģʽ�� ֹͣ��������
void Send_STOP()
{
	 I2C1->CR1 |= 1<<9;
}

//��ģʽ�� ��ʼ��������
void Send_START()
{
	 I2C1->CR1 |= 1<<8;
}
