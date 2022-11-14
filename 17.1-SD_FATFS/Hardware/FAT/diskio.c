/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "w25q128.h"
#include "usart.h"
#include "stm32_sdio_sd.h"

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_FLASH 0
#define DEV_SDCARD 1

#define FLASH_SECTOR_SIZE 4096
#define SDCARD_SECTOR_SIZE 512

extern SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
  SD_Error SD_State = SD_OK;
  u16 FLASH_ID;
	switch (pdrv) {
		case DEV_FLASH:
	  
	   FLASH_ID = Read_Manufacturer();
		 if(FLASH_ID == NM25Q128) {
		   stat = 0;
		 } 
		 else
		 {
		   stat = STA_NOINIT;
		 }
	    
		
		return stat;

	case DEV_SDCARD :
		 
     SD_State = SD_Init();
	   if(SD_State == SD_OK) 
     {
		    stat = 0;
		 } 
		 else 
		 {
		    stat = STA_NOINIT;
		 }  

		return stat;

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
  u16 i;

	switch (pdrv) {
	case DEV_FLASH :
			W25Q128_Init();
      // 延时一小段时间
	    i=500;
	    while(--i);	
      stat = disk_status(pdrv);
	    printf("init stat = %d\r\n",stat);
		return stat;

	case DEV_SDCARD :
      stat = disk_status(pdrv);
	    printf("sd card init stat = %d\r\n",stat);
	    return stat;
//	case DEV_USB :
//		result = USB_disk_initialize();

//		// translate the reslut code here

//		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
  SD_Error SD_State = SD_OK;
	switch (pdrv) {
	case DEV_FLASH :
		/* 扇区偏移2MB，外部Flash文件系统空间放在SPI Flash后面6MB空间 */
      sector+=512; 
		  W25Q128_Read((u8 *)buff, sector * FLASH_SECTOR_SIZE, count * FLASH_SECTOR_SIZE);
      res = RES_OK;
		return res;

	case DEV_SDCARD :

    SD_State = SD_ReadMultiBlocks((BYTE *)buff, (uint64_t) sector * SDCARD_SECTOR_SIZE, SDCARD_SECTOR_SIZE, count);
	  if (SD_State == SD_OK) 
    {
		    /* Check if the Transfer is finished */
			  SD_State = SD_WaitReadOperation();
			  while(SD_GetStatus() != SD_TRANSFER_OK);
		} 
		
    if (SD_State != SD_OK)
    {
	      res = RES_PARERR; 
	  } 
		else 
    {
		    res = RES_OK;
		}
   
		return res;

	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res  = RES_PARERR;
  SD_Error SD_State = SD_OK;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}
	switch (pdrv) {
		case DEV_FLASH :
			
			/* 扇区偏移2MB，外部Flash文件系统空间放在SPI Flash后面14MB空间 */
			sector+=512;
			W25Q128_Sector_Erase(sector * FLASH_SECTOR_SIZE);
			W25Q128_Write((u8 *)buff,sector * FLASH_SECTOR_SIZE,count * FLASH_SECTOR_SIZE);
			res = RES_OK;
		return res;

	case DEV_SDCARD :
       
      SD_State = SD_WriteMultiBlocks((BYTE *)buff, (uint64_t) sector * SDCARD_SECTOR_SIZE, SDCARD_SECTOR_SIZE, count);
	    if (SD_State == SD_OK) 
			{
					 /* Check if the Transfer is finished */
					SD_State = SD_WaitReadOperation();
					while(SD_GetStatus() != SD_TRANSFER_OK);
			} 
			
			if (SD_State != SD_OK)
			{
					res = RES_PARERR; 
			} 
			else 
			{
					res = RES_OK;
			}

		  return res;

	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;


	if (pdrv == DEV_FLASH) {
    switch (cmd) {
        /* 扇区数量：3584*4096/1024/1024=14(MB) */
        case GET_SECTOR_COUNT: // FLASH_SECTOR_SIZE - 512 
          *(DWORD * )buff = 3584; // 这个值来源于前面512扇区给了文件系统表，后面的才是可以使用的空间  w25q128 4096 扇区 - 512 扇区 =	3584
        break;
        /* 扇区大小  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = 4096; //FLASH_SECTOR_SIZE
        break;
        /* 同时擦除扇区个数 */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
        break;        
      }
      res = RES_OK;
	} 
	else if (pdrv == DEV_SDCARD) 
  {
	    switch (cmd) {
 
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize; 
        break;
        /* 扇区大小  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = SDCARD_SECTOR_SIZE; //SD_SECTOR_SIZE
        break;
        /* 同时擦除扇区个数 */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
        break;        
      }
      res = RES_OK;
	}
	else 
  {
	   res = RES_PARERR;
	}

	return res;
}

DWORD get_fattime (void)
{
    return (DWORD)(2022 - 80) << 25 |
           (DWORD)(10 + 1) << 21 |
           (DWORD)9 << 16 |
           (DWORD)20 << 11 |
           (DWORD)20 << 5 |
           (DWORD)0 >> 1;
}
