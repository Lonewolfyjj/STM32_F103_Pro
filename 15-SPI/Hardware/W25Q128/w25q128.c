#include "w25q128.h" 
#include "spi.h"
#include "delay.h"	   
#include "usart.h"	

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector 
											 
//初始化SPI FLASH的IO口
void W25Q128_Init(void)
{ 
	uint32_t result;

	/*FLASH NSS配置*/
  GPIOB->CRH&=0XFFF0FFFF; 
  GPIOB->CRH|=0X00030000; // PB12 --- nss 通用推挽输出
  GPIOB->ODR |= 1<<12;
	
	SPI2_Init(); //初始化SPI

	result=Read_Manufacturer();//读取FLASH ID.
	printf("W25Q128_Init 0x%x\r\n",result);
}  

//读取芯片ID	 0X5217	表示芯片型号为 NM25Q128 	 
u16 Read_Manufacturer(void)
{
  u16 Temp = 0;
	W25Q128_CS=0;
	SPI2_ReadWriteByte(MANUFACTURER_ID);//发送读取ID命令	     	 			   
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
  SPI2_ReadWriteByte(0x00);
	Temp = SPI2_ReadWriteByte(0x00)<<8;
	Temp |= SPI2_ReadWriteByte(0x00);
	W25Q128_CS=1;				    

  printf("Temp = 0x%x\r\n",Temp);	
	return Temp;
} 

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25Q128_Sector_Erase(u32 address)
{
	//监视falsh擦除情况,测试用   
	printf("fe:%x\r\n",address);	   
	W25Q128_CS=0;                            	//使能器件   
	SPI2_ReadWriteByte(SECTOR_ERASE);      	//发送扇区擦除指令 
	SPI2_ReadWriteByte((u8)((address)>>16));  	//发送24bit地址    
	SPI2_ReadWriteByte((u8)((address)>>8));   
	SPI2_ReadWriteByte((u8)address);  
	W25Q128_CS=1;                            	//取消片选     	      
	W25Q128_Wait();   				   		//等待擦除完成
}

// 等待flash 内部擦除完成
void W25Q128_Wait()
{
	u8 Temp;
	W25Q128_CS=0;
  SPI2_ReadWriteByte(Read_Status_Register);
	do{
    Temp = SPI2_ReadWriteByte(0x00);
	} 
	while(Temp & 0x01);
	W25Q128_CS=1;
}

// 读取flash 扇区的内容
void W25Q128_Read(u8 *readBuffer, u32 readAddr, u32 numByteToRead)
{ 
    u16 i;   										    
		printf("    0x%x                \r\n", readAddr);
	  W25Q128_CS=0;                            	//使能器件   
    SPI2_ReadWriteByte(READ_DATA);         	//发送读取命令   
    SPI2_ReadWriteByte((u8)((readAddr)>>16));  	//发送24bit地址    
    SPI2_ReadWriteByte((u8)((readAddr)>>8));   
    SPI2_ReadWriteByte((u8)readAddr);   
    for(i=0;i<numByteToRead;i++)
	  { 
        readBuffer[i]=SPI2_ReadWriteByte(DUMMY);   	//循环读数  
    }
	W25Q128_CS=1; 			    
} 

// 写入flash
