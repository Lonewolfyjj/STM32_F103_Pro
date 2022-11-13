#ifndef _W25Q128_H_
#define _W25Q128_H_
#include "sys.h"  		   

#define	W25Q128_CS 		PBout(12)  		//W25Q128的片选信号

//指令表
#define DUMMY 0xFF // 空指令
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
