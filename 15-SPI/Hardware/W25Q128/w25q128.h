#ifndef _W25Q128_H_
#define _W25Q128_H_
#include "sys.h"  		   

#define	W25Q128_CS 		PBout(12)  		//W25Q128的片选信号

//指令表
#define DUMMY 0xFF // 空指令
#define MANUFACTURER_ID 0x90

#define SECTOR_ERASE		0x20 
#define READ_DATA 0x03
#define Read_Status_Register		0x05 

void W25Q128_Init(void);
void W25Q128_Wait(void);
u16 Read_Manufacturer(void);
void W25Q128_Sector_Erase(u32 address);
void W25Q128_Read(u8 *readBuffer, u32 readAddr, u32 numByteToRead);
#endif
