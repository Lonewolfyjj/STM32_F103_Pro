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

  //printf("Temp = 0x%x\r\n",Temp);	
	return Temp;
} 

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25Q128_Sector_Erase(u32 address)
{
	//����falsh�������,������   
	printf("fe:%x\r\n",address);	
	//address*=4096;
    W25Q128_Write_Enable();	
	W25Q128_CS=0;                            	//ʹ������   
	SPI2_ReadWriteByte(SECTOR_ERASE);      	  //������������ָ�� 
	SPI2_ReadWriteByte((u8)((address)>>16));  //����24bit��ַ    
	SPI2_ReadWriteByte((u8)((address)>>8));   
	SPI2_ReadWriteByte((u8)address);  
	W25Q128_CS=1;                            	//ȡ��Ƭѡ     	      
	W25Q128_Wait();   				   		          //�ȴ��������
	
}
	
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* ����FLASHдʹ������ */
  W25Q128_Write_Enable();	
  W25Q128_Wait();
  /* �������� */
  /* ѡ��FLASH: CS�͵�ƽ */
  W25Q128_CS=0;  
  /* ������������ָ��*/
  SPI2_ReadWriteByte(SECTOR_ERASE);
  /*���Ͳ���������ַ�ĸ�λ*/
  SPI2_ReadWriteByte((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  SPI2_ReadWriteByte((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  SPI2_ReadWriteByte(SectorAddr & 0xFF);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
   W25Q128_CS=1;  
  /* �ȴ��������*/
  W25Q128_Wait();
}


// �ȴ�flash �ڲ��������
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

// ��ȡflash ����������
void W25Q128_Read(u8 *readBuffer, u32 readAddr, u32 numByteToRead)
{ 
	u16 i;   										    
	//printf("    0x%x                \r\n", readAddr);
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


void W25Q128_Write_Enable() 
{
	W25Q128_CS=0;                           //ʹ������   
	SPI2_ReadWriteByte(WRITE_ENABLE);      	//дʹ�� 
	W25Q128_CS=1;                           //ȡ��Ƭѡ     	      
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25Q128_Write_Page(u8 *writeBuffer, u32 writeAddr, u32 numByteToWrite)
{ 
	u16 i;   										    
	W25Q128_Write_Enable();	
	W25Q128_CS=0;                            	//ʹ������   
	SPI2_ReadWriteByte(WRITE_DATA);         	//���Ͷ�ȡ����   
	SPI2_ReadWriteByte((u8)((writeAddr)>>16));  	//����24bit��ַ    
	SPI2_ReadWriteByte((u8)((writeAddr)>>8));   
	SPI2_ReadWriteByte((u8)writeAddr);   
	for(i=0;i<numByteToWrite;i++)
	{ 
        SPI2_ReadWriteByte(writeBuffer[i]);   	//ѭ������  
	}
	W25Q128_CS=1;
	W25Q128_Wait();		
} 


//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25Q128_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		//printf("write page WriteAddr = 0x%x ,pageremain = 0x%x ",WriteAddr,pageremain);
		W25Q128_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
u8 W25QXX_BUFFER[4096];		 
void W25Q128_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		//printf("secpos = %d\r\n",secpos);
		W25Q128_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25Q128_Sector_Erase(secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25Q128_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  
		}
		else 
		{ 
		  W25Q128_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		}
		if(NumByteToWrite == secremain){ 
			break;//д�������
		}
		else //д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		  pBuffer+=secremain;  				//ָ��ƫ��
			WriteAddr+=secremain;				//д��ַƫ��	   
		  NumByteToWrite-=secremain;			//�ֽ����ݼ�
			if(NumByteToWrite > 4096)
			{
				secremain=4096;//��һ����������д����
			}
			else 
			{
				secremain=NumByteToWrite;		//��һ����������д����
			}
		}	 
	}	
}

//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25Q128_Erase_Chip(void)   
{                                   
	W25Q128_Write_Enable();                 	 	//SET WEL 
	W25Q128_Wait();   
	W25Q128_CS=0;                            	//ʹ������   
	SPI2_ReadWriteByte(CHIP_ERASE);        	//����Ƭ��������  
  W25Q128_CS=1;                            	//ȡ��Ƭѡ    
  printf("waiting chip erase....");	
	W25Q128_Wait();   				   		//�ȴ�оƬ��������
}   

