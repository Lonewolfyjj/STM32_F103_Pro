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

  //printf("Temp = 0x%x\r\n",Temp);	
	return Temp;
} 

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25Q128_Sector_Erase(u32 address)
{
	//监视falsh擦除情况,测试用   
	printf("fe:%x\r\n",address);	
	//address*=4096;
    W25Q128_Write_Enable();	
	W25Q128_CS=0;                            	//使能器件   
	SPI2_ReadWriteByte(SECTOR_ERASE);      	  //发送扇区擦除指令 
	SPI2_ReadWriteByte((u8)((address)>>16));  //发送24bit地址    
	SPI2_ReadWriteByte((u8)((address)>>8));   
	SPI2_ReadWriteByte((u8)address);  
	W25Q128_CS=1;                            	//取消片选     	      
	W25Q128_Wait();   				   		          //等待擦除完成
	
}
	
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* 发送FLASH写使能命令 */
  W25Q128_Write_Enable();	
  W25Q128_Wait();
  /* 擦除扇区 */
  /* 选择FLASH: CS低电平 */
  W25Q128_CS=0;  
  /* 发送扇区擦除指令*/
  SPI2_ReadWriteByte(SECTOR_ERASE);
  /*发送擦除扇区地址的高位*/
  SPI2_ReadWriteByte((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  SPI2_ReadWriteByte((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  SPI2_ReadWriteByte(SectorAddr & 0xFF);
  /* 停止信号 FLASH: CS 高电平 */
   W25Q128_CS=1;  
  /* 等待擦除完毕*/
  W25Q128_Wait();
}


// 等待flash 内部擦除完成
void W25Q128_Wait()
{
	u8 Temp;
	W25Q128_CS=0;
    SPI2_ReadWriteByte(READ_STATUS_REGISTER);
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
	//printf("    0x%x                \r\n", readAddr);
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


void W25Q128_Write_Enable() 
{
	W25Q128_CS=0;                           //使能器件   
	SPI2_ReadWriteByte(WRITE_ENABLE);      	//写使能 
	W25Q128_CS=1;                           //取消片选     	      
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25Q128_Write_Page(u8 *writeBuffer, u32 writeAddr, u32 numByteToWrite)
{ 
	u16 i;   										    
	W25Q128_Write_Enable();	
	W25Q128_CS=0;                            	//使能器件   
	SPI2_ReadWriteByte(WRITE_DATA);         	//发送读取命令   
	SPI2_ReadWriteByte((u8)((writeAddr)>>16));  	//发送24bit地址    
	SPI2_ReadWriteByte((u8)((writeAddr)>>8));   
	SPI2_ReadWriteByte((u8)writeAddr);   
	for(i=0;i<numByteToWrite;i++)
	{ 
        SPI2_ReadWriteByte(writeBuffer[i]);   	//循环读数  
	}
	W25Q128_CS=1;
	W25Q128_Wait();		
} 


//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25Q128_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		//printf("write page WriteAddr = 0x%x ,pageremain = 0x%x ",WriteAddr,pageremain);
		W25Q128_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
u8 W25QXX_BUFFER[4096];		 
void W25Q128_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		//printf("secpos = %d\r\n",secpos);
		W25Q128_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25Q128_Sector_Erase(secpos);		//擦除这个扇区
			for(i=0;i<secremain;i++)	   		//复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25Q128_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  
		}
		else 
		{ 
		  W25Q128_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		}
		if(NumByteToWrite == secremain){ 
			break;//写入结束了
		}
		else //写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		  pBuffer+=secremain;  				//指针偏移
			WriteAddr+=secremain;				//写地址偏移	   
		  NumByteToWrite-=secremain;			//字节数递减
			if(NumByteToWrite > 4096)
			{
				secremain=4096;//下一个扇区还是写不完
			}
			else 
			{
				secremain=NumByteToWrite;		//下一个扇区可以写完了
			}
		}	 
	}	
}

//擦除整个芯片		  
//等待时间超长...
void W25Q128_Erase_Chip(void)   
{                                   
	W25Q128_Write_Enable();                 	 	//SET WEL 
	W25Q128_Wait();   
	W25Q128_CS=0;                            	//使能器件   
	SPI2_ReadWriteByte(CHIP_ERASE);        	//发送片擦除命令  
  W25Q128_CS=1;                            	//取消片选    
  printf("waiting chip erase....");	
	W25Q128_Wait();   				   		//等待芯片擦除结束
}   

