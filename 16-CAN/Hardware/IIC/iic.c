#include "iic.h"
#include "sys.h"
#include "usart.h"

void IIC_Init()
{
	RCC->APB2ENR |= 1<<3; //使能GPIOB时钟
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  //使能I2C1时钟
	RCC->APB1ENR |= 1<<21;
	GPIOB->CRL |= 0xFF000000; // PB6 pb7复用功能开漏输出模式 50MHZ

	// 配置IIC 的时钟频率36M
	I2C1->CR2 |= 0x24<<0;

	I2C1->OAR1 |= 1<14;// 必须始终由软件保持为’1’。

	I2C1->CCR |= 1<15; // 快速IIC
	I2C1->CCR &= ~(1<14); // Tlow/Thigh = 2
	I2C1->CCR |= 30; //主模式下的快速模式的分频系数,用于产生400Khz的高速通信频率

	I2C1->TRISE |= 91;

	I2C1->OAR1 |= STM32_SLAVE_ADDRESS; // 设定STM32作为从机的地址 	

	IIC_ACK_Enable(1);
	I2C1->CR1 |= 1<<0; //使能IIc
}

void send_write_byte(u8 addr,u8 dataValue)
{
	u16 temp;
	while(IIC_BUSY());
	Send_START();
	while((I2C1->SR1 & 0x01) == 0); //1 起始条件已发送
	
	I2C1->DR = ( EEPROM_ADDRESS); // 地址 + 写 
  while((I2C1->SR1 & 0x0002) == 0); // 1 代表地址发送完成
  temp = I2C1->SR2; // 必须读SR2寄存器
	
	I2C1->DR = addr; // 写EEPROM 地址数据 
	while((I2C1->SR1 & 0x0080) == 0);//  1：数据寄存器空
	
  I2C1->DR |= dataValue; // 写EEPROM 地址数据 
	while((I2C1->SR1 & 0x0080) == 0);//  1：数据寄存器空

  Send_STOP();
}

u8 send_read_byte(u8 addr)
{
	u8 dataValue,temp;

	while(IIC_BUSY());

	Send_START();
	while((I2C1->SR1 & 0x01) == 0); //1 起始条件已发送

	I2C1->DR = (EEPROM_ADDRESS); //发送设备地址+读信号
	while((I2C1->SR1 & 0x0002) == 0); // 1 代表地址发送完成
	temp = I2C1->SR2; // 必须读SR2寄存器

	I2C1->DR = addr;//发送存储单元地址
	while((I2C1->SR1 & 0x0080) == 0);//  1：数据寄存器非空

	Send_START();
	while((I2C1->SR1 & 0x01) == 0); //1 起始条件已发送

	I2C1->DR = (EEPROM_ADDRESS | 0X01); //发送设备地址+读信号
	while((I2C1->SR1 & 0x0002) == 0); // 1 代表地址发送完成
	temp = I2C1->SR2; // 必须读SR2寄存器

	IIC_ACK_Enable(0);
	Send_STOP();

	while(!(I2C1->SR1 & 1<<6));  // 数据寄存器不为空
	dataValue=I2C1->DR;//读出寄存器数据

	return dataValue;
}

/*
 * 主模式下的 硬件IIC 初始化
 */
void IIC_ACK_Enable(u8 enable) {
	if(enable) 
	{
    I2C1->CR1 |= 1<<10; // 应答使能 
	} 
	else 
  {
	  I2C1->CR1 &= ~(1<<10); // 应答disable
	}
}

/*
IIC 忙标志
I2C_CR1[15] 0:在总线上无数据通讯；,1:在总线上正在进行数据通讯
*/
uint8_t IIC_BUSY() {

 return I2C1->SR2 & 1<<1;
}

//主模式下 停止条件产生
void Send_STOP()
{
	 I2C1->CR1 |= 1<<9;
}

//主模式下 起始条件产生
void Send_START()
{
	 I2C1->CR1 |= 1<<8;
}
