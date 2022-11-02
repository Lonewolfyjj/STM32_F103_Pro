#include "spi.h"
 
void SPI2_Init(void)
{	 
	  gpio_config();
   SPI2_Config();
}   


void gpio_config(void) 
{
  RCC->APB2ENR|=1<<3;  	//PORTB时钟使能 	 
  RCC->APB1ENR|=1<<14;   	//SPI2时钟使能 

	/*配置 MISO MOSI SCK*/
	GPIOB->CRH &= 0X000FFFFF; // io必须先复位，才能使用
	GPIOB->CRH |= 0XB8B00000; //	PB13 --- sck 推挽复用输出
  GPIOB->ODR |= 1<<13;
	GPIOB->ODR |= 1<<15;
}

void SPI2_Config(void) 
{
	SPI2->CR1 &= 0X0000;
	SPI2->CR1 |= 0<<15; // 双线双向模式
  SPI2->CR1 |= 0<<11; // 使用8位数据帧格式进行发送/接收
	SPI2->CR1 |= 0<<10; // 全双工(发送和接收)
	SPI2->CR1 |= 1<<9; // 启用软件从设备管理
	SPI2->CR1 |= 1<<8; // 它決定了NSS引脚的的电平，0，则NSS一直输出低电平！！注意！！
	SPI2->CR1 |= 0<<7; // 先发送MSB

	SPI2->CR1 &= ~(7<<3); // fPCLK/2
  SPI2->CR1 |= 1<<2; // 配置为主设备
	SPI2->CR1 |= 3<<0; //空闲状态时， SCK保持高电平 + 数据采样从第二个时钟边沿开始
  SPI2->CR1 |= 1<<6; // 开启SPI设备
}

/*发送寄存器是否为空
  1：发送缓冲为空
*/
u8 isTxEmpty() 
{
  return SPI2->SR & 0x2;
}

/*接收寄存器是否为空
  1：接收缓冲非空
*/
u8 isRxNotEmpty() 
{
  return SPI2->SR & 0x1;
}

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 data)
{		
  u8 result;
	while(!isTxEmpty()); // 发送不为空执行发送
	SPI2->DR = data;
	
	while(!isRxNotEmpty()); // 数据接收器为空一直等待接收数据
	result = SPI2->DR;
	
  return result;		    
}
