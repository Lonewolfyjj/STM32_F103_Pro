#include "w25q128.h" 
#include "spi.h"
#include "delay.h"	   
#include "usart.h"	

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector 
											 
//��ʼ��SPI FLASH��IO��
void W25Q128_Init(void)
{ 
	uint32_t result;

	/*FLASH NSS����*/
  GPIOB->CRH&=0XFFF0FFFF; 
  GPIOB->CRH|=0X00030000; // PB12 --- nss ͨ���������
  GPIOB->ODR |= 1<<12;
	
	SPI2_Init(); //��ʼ��SPI

	result=Read_Manufacturer();//��ȡFLASH ID.
	printf("W25Q128_Init 0x%x\r\n",result);
}  

//��ȡоƬID	 0X5217	��ʾоƬ�ͺ�Ϊ NM25Q128 	 
u16 Read_Manufacturer(void)
{
  u16 Temp = 0;
	W25Q128_CS=0;
	SPI2_ReadWriteByte(MANUFACTURER_ID);//���Ͷ�ȡID����	     	 			   
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
  SPI2_ReadWriteByte(0x00);
	Temp = SPI2_ReadWriteByte(0x00)<<8;
	Temp |= SPI2_ReadWriteByte(0x00);
	W25Q128_CS=1;				    

  printf("Temp = 0x%x\r\n",Temp);	
	return Temp;
} 

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25Q128_Sector_Erase(u32 address)
{
	//����falsh�������,������   
	printf("fe:%x\r\n",address);	   
	W25Q128_CS=0;                            	//ʹ������   
	SPI2_ReadWriteByte(SECTOR_ERASE);      	//������������ָ�� 
	SPI2_ReadWriteByte((u8)((address)>>16));  	//����24bit��ַ    
	SPI2_ReadWriteByte((u8)((address)>>8));   
	SPI2_ReadWriteByte((u8)address);  
	W25Q128_CS=1;                            	//ȡ��Ƭѡ     	      
	W25Q128_Wait();   				   		//�ȴ��������
}

// �ȴ�flash �ڲ��������
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

// ��ȡflash ����������
void W25Q128_Read(u8 *readBuffer, u32 readAddr, u32 numByteToRead)
{ 
    u16 i;   										    
		printf("    0x%x                \r\n", readAddr);
	  W25Q128_CS=0;                            	//ʹ������   
    SPI2_ReadWriteByte(READ_DATA);         	//���Ͷ�ȡ����   
    SPI2_ReadWriteByte((u8)((readAddr)>>16));  	//����24bit��ַ    
    SPI2_ReadWriteByte((u8)((readAddr)>>8));   
    SPI2_ReadWriteByte((u8)readAddr);   
    for(i=0;i<numByteToRead;i++)
	  { 
        readBuffer[i]=SPI2_ReadWriteByte(DUMMY);   	//ѭ������  
    }
	W25Q128_CS=1; 			    
} 

// д��flash
