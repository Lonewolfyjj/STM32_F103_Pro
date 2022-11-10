#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f10x.h"  		   

#define  		SPI_FLASH_CS_LOW()     						GPIO_ResetBits( GPIOB, GPIO_Pin_12 )
#define  		SPI_FLASH_CS_HIGH()    						GPIO_SetBits( GPIOB, GPIO_Pin_12)
#define NM25Q128	0X5217
//÷∏¡Ó±Ì
#define DUMMY 0xFF // ø’÷∏¡Ó
#define MANUFACTURER_ID              0x90
#define SECTOR_ERASE		             0x20 
#define READ_DATA                    0x03
#define READ_STATUS_REGISTER		     0x05 
#define WRITE_ENABLE                 0x06
#define WRITE_DATA                   0x02
#define CHIP_ERASE                    0xC7


void W25Q128_Init(void);
void W25Q128_Wait(void);
u16 Read_Manufacturer(void);
void W25Q128_Sector_Erase(u32 address);
void W25Q128_Read(u8 *readBuffer, u32 readAddr, u32 numByteToRead);
void W25Q128_Write_Enable(void);
void W25Q128_Write_Page(u8 *writeBuffer, u32 writeAddr, u32 numByteToWrite);
void W25Q128_Erase_Chip(void);
void W25Q128_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 numByteToWrite) ;
void W25Q128_Write(u8* pBuffer,u32 WriteAddr,u16 numByteToWrite);



#endif


