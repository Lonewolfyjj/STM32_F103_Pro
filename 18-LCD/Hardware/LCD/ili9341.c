#include "ili9341.h"
#include "usart.h"
#include "delay.h"
#include "font.h"

u16  LCD_X_LENGTH   =   ILI9341_LESS_PIXEL;
u16  LCD_Y_LENGTH   =   ILI9341_MORE_PIXEL;
static sFONT *LCD_Currentfonts = &Font8x16;  //英文字体

static uint16_t CurrentTextColor   = WHITE;//前景色
static uint16_t CurrentBackColor   = BLACK;//背景色

uint8_t * pBuffer;

void init_lcd() 
{

	Gpio_config();
	FSMC_config();
    delay_ms(50);     
    ILI9341_BackLed_Control(ENABLE);
    ILI9341_ReadID();
    set9341Config();
    setScanOrigation(0);
    LCD_SetBackColor(CurrentBackColor);
    LCD_SetTextColor(CurrentTextColor);
    //drawRect (0,0,64,64,0xffff);

}

void Gpio_config()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(
        FSMC_DATA_PORTD_CLK | FSMC_DATA_PORTE_CLK | LCD_BACKLIGHT_PORT_CLK | 
        FSMC_A10_PORT_CLK | FSMC_NOE_PORT_CLK | FSMC_NWE_PORT_CLK | 
        FSMC_NE4_PORT_CLK,
        ENABLE);
    
    /*配置背光引脚*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_BACKLIGHT_PIN;
    GPIO_Init(LCD_BACKLIGHT_PORT, &GPIO_InitStructure);

  
    /* 配置FSMC相对应的数据线,FSMC-D0~D15 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = FSMC_D0 | FSMC_D1 | FSMC_D2 | FSMC_D3 | FSMC_D13 | FSMC_D14 | FSMC_D15;
    GPIO_Init(FSMC_DATA_PORTD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = FSMC_D4 | FSMC_D5 | FSMC_D6 | FSMC_D7 | FSMC_D8 | FSMC_D9 | FSMC_D10 | FSMC_D11 | FSMC_D12;
    GPIO_Init(FSMC_DATA_PORTE, &GPIO_InitStructure);

    
    /*配置数据/命令选择*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = FSMC_A10_PIN;
    GPIO_Init(FSMC_A10_PORT, &GPIO_InitStructure);
    
    /*读使能 控制线*/
    GPIO_InitStructure.GPIO_Pin = FSMC_NOE_PIN;
    GPIO_Init(FSMC_NOE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = FSMC_NWE_PIN;
    GPIO_Init(FSMC_NWE_PORT, &GPIO_InitStructure);
    
    /*BANK 1 RANGE 4 片选控制线*/
    GPIO_InitStructure.GPIO_Pin = FSMC_NE4_PIN;
    GPIO_Init(FSMC_NE4_PORT, &GPIO_InitStructure);
    
    
}


void FSMC_config()
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  ReadWriteTiming;

		
	/*使能FSMC外设时钟*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );
    
	//地址建立时间（ADDSET）为1个HCLK 1/72M=14ns
	ReadWriteTiming.FSMC_AddressSetupTime        = 0x01;	
    //数据保持时间（DATAST）+ 1个HCLK = 3/72M=42ns
	ReadWriteTiming.FSMC_DataSetupTime           = 0x02;	
    //选择匹配SRAM的模式
	ReadWriteTiming.FSMC_AccessMode              = FSMC_AccessMode_B;	 
    
	//地址保持时间（ADDHLD）模式A未用到
	ReadWriteTiming.FSMC_AddressHoldTime         = 0x00;	 
	//设置总线转换周期，仅用于复用模式的NOR操作
	ReadWriteTiming.FSMC_BusTurnAroundDuration   = 0x00;
	//设置时钟分频，仅用于同步类型的存储器
	ReadWriteTiming.FSMC_CLKDivision             = 0x00;	
	//数据保持时间，仅用于同步型的NOR
	ReadWriteTiming.FSMC_DataLatency             = 0x00;		

    ReadWriteTiming.FSMC_BusTurnAroundDuration   = 0x00;


	// 选择FSMC映射的存储区域： Bank1 sram4
	FSMC_NORSRAMInitStructure.FSMC_Bank               = FSMC_Bank1_NORSRAM4;
	
	//设置地址总线与数据总线是否复用，仅用于NOR
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux     = FSMC_DataAddressMux_Disable; 
	
	//设置要控制的存储器类型：SRAM类型
	FSMC_NORSRAMInitStructure.FSMC_MemoryType         = FSMC_MemoryType_NOR;   
	
	//存储器数据宽度：16位
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth    = FSMC_MemoryDataWidth_16b; 
	
	//设置是否使用突发访问模式，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode    = FSMC_BurstAccessMode_Disable;
	
	//设置是否使能等待信号，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait   = FSMC_AsynchronousWait_Disable;
	
	//设置等待信号的有效极性，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	
	//设置是否支持把非对齐的突发操作，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WrapMode           = FSMC_WrapMode_Disable; 
	
	//设置等待信号插入的时间，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive   = FSMC_WaitSignalActive_BeforeWaitState;
	
	//存储器写使能 
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation     = FSMC_WriteOperation_Enable;
	
	//不使用等待信号
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal         = FSMC_WaitSignal_Disable;  		
	
	// 不使用扩展模式，读写使用相同的时序
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode       = FSMC_ExtendedMode_Disable; // FSMC_ExtendedMode_Enable
	
	//突发写操作
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst         = FSMC_WriteBurst_Disable;   
	
	//读写时序配置
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &ReadWriteTiming;
	
	//读写同样时序，使用扩展模式时这个配置才有效
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &ReadWriteTiming; 

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  //使能BANK1 

}



/**
  * @brief  向ILI9341写入命令
  * @param  usCmd :要写入的命令（表寄存器地址）
  * @retval 无
  */	
__inline void ILI9341_Write_Cmd ( uint16_t usCmd )
{
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = usCmd;
	
}

/**
  * @brief  向ILI9341写入命令
  * @param  usCmd :要写入的命令（表寄存器地址）
  * @retval 无
  */	
__inline void ILI9341_Write_Data ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = usData;
	
}


/**
  * @brief  从ILI9341读取数据
  * @param  无
  * @retval 读取到的数据
  */	
__inline uint16_t ILI9341_Read_Data ( void )
{
	return ( * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) );
}



uint16_t ILI9341_ReadID(void) 
{
    uint32_t temp;
    
    ILI9341_Write_Cmd(0xd3); 
    temp = ILI9341_Read_Data(); 
    temp = ILI9341_Read_Data();
    temp = ILI9341_Read_Data();
    temp <<=8;
    temp |= ILI9341_Read_Data();
    printf("lcd id = 0x%x\r\n",temp); 
    
    return 0;
}

void ILI9341_BackLed_Control ( FunctionalState enumState )
{
	if (enumState)
		GPIO_SetBits (LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN );	
	else
		GPIO_ResetBits (LCD_BACKLIGHT_PORT, LCD_BACKLIGHT_PIN );
		
}


void openWindow(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height) 
{

    ILI9341_Write_Cmd(0x2A);  // 开窗确定横向区域大小
    ILI9341_Write_Data((uint8_t)(startX >> 8));  
    ILI9341_Write_Data((uint8_t)(startX & 0XFF));  
    ILI9341_Write_Data((uint8_t)((startX + width) >> 8));  
    ILI9341_Write_Data((uint8_t)((startX + width) & 0XFF));  
    
    
    
    ILI9341_Write_Cmd(0x2B);  // 开窗确定横向区域大小
    ILI9341_Write_Data((uint8_t)(startY >> 8));  
    ILI9341_Write_Data((uint8_t)(startY & 0XFF));  
    ILI9341_Write_Data((uint8_t)((startY + height) >> 8));  
    ILI9341_Write_Data((uint8_t)((startY + height) & 0XFF));  

   
}

void drawRect(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height,u16 color) 
{
    uint32_t i;
    openWindow(startX, startY, width, height);
    
    ILI9341_Write_Cmd(0x2C);
    

    for(i=0;i<width * height ;i++) 
    {
        ILI9341_Write_Data(color); 
    }
    
}

void set9341Config() 
{
/*  Power control B (CFh)  */
    //DEBUG_DELAY  ();
    ILI9341_Write_Cmd ( 0xCF  );
    ILI9341_Write_Data ( 0x00  );
    ILI9341_Write_Data ( 0x81  );
    ILI9341_Write_Data ( 0x30  );
    
    /*  Power on sequence control (EDh) */
    //DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xED );
    ILI9341_Write_Data ( 0x64 );
    ILI9341_Write_Data ( 0x03 );
    ILI9341_Write_Data ( 0x12 );
    ILI9341_Write_Data ( 0x81 );
    
    /*  Driver timing control A (E8h) */
    //DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xE8 );
    ILI9341_Write_Data ( 0x85 );
    ILI9341_Write_Data ( 0x10 );
    ILI9341_Write_Data ( 0x78 );
    
    /*  Power control A (CBh) */
    //DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xCB );
    ILI9341_Write_Data ( 0x39 );
    ILI9341_Write_Data ( 0x2C );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x34 );
    //ILI9341_Write_Data ( 0x02 );
    ILI9341_Write_Data ( 0x06 ); //原来是0x02改为0x06可防止液晶显示白屏时有条纹的情况
    
    /* Pump ratio control (F7h) */
    //DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xF7 );
    ILI9341_Write_Data ( 0x20 );
    
    /* Driver timing control B */
    //DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xEA );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
//    DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xB1 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x1B );
    
    /*  Display Function Control (B6h) */
   // DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xB6 );
    ILI9341_Write_Data ( 0x0A );
    ILI9341_Write_Data ( 0xA2 );
    
    /* Power Control 1 (C0h) */
   // DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xC0 );
    ILI9341_Write_Data ( 0x35 );
    
    /* Power Control 2 (C1h) */
  //  DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0xC1 );
    ILI9341_Write_Data ( 0x11 );
    
    /* VCOM Control 1 (C5h) */
    ILI9341_Write_Cmd ( 0xC5 );
    ILI9341_Write_Data ( 0x45 );
    ILI9341_Write_Data ( 0x45 );
    
    /*  VCOM Control 2 (C7h)  */
    ILI9341_Write_Cmd ( 0xC7 );
    ILI9341_Write_Data ( 0xA2 );
    
    /* Enable 3G (F2h) */
    ILI9341_Write_Cmd ( 0xF2 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Gamma Set (26h) */
    ILI9341_Write_Cmd ( 0x26 );
    ILI9341_Write_Data ( 0x01 );
  //  DEBUG_DELAY ();
    
    /* Positive Gamma Correction */
    ILI9341_Write_Cmd ( 0xE0 ); //Set Gamma
    ILI9341_Write_Data ( 0x0F );
    ILI9341_Write_Data ( 0x26 );
    ILI9341_Write_Data ( 0x24 );
    ILI9341_Write_Data ( 0x0B );
    ILI9341_Write_Data ( 0x0E );
    ILI9341_Write_Data ( 0x09 );
    ILI9341_Write_Data ( 0x54 );
    ILI9341_Write_Data ( 0xA8 );
    ILI9341_Write_Data ( 0x46 );
    ILI9341_Write_Data ( 0x0C );
    ILI9341_Write_Data ( 0x17 );
    ILI9341_Write_Data ( 0x09 );
    ILI9341_Write_Data ( 0x0F );
    ILI9341_Write_Data ( 0x07 );
    ILI9341_Write_Data ( 0x00 );
    
    /* Negative Gamma Correction (E1h) */
    ILI9341_Write_Cmd ( 0XE1 ); //Set Gamma
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x19 );
    ILI9341_Write_Data ( 0x1B );
    ILI9341_Write_Data ( 0x04 );
    ILI9341_Write_Data ( 0x10 );
    ILI9341_Write_Data ( 0x07 );
    ILI9341_Write_Data ( 0x2A );
    ILI9341_Write_Data ( 0x47 );
    ILI9341_Write_Data ( 0x39 );
    ILI9341_Write_Data ( 0x03 );
    ILI9341_Write_Data ( 0x06 );
    ILI9341_Write_Data ( 0x06 );
    ILI9341_Write_Data ( 0x30 );
    ILI9341_Write_Data ( 0x38 );
    ILI9341_Write_Data ( 0x0F );
    
    /* memory access control set */
   // DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0x36 ); 	
    ILI9341_Write_Data ( 0x28 );    /*竖屏  左上角到 (起点)到右下角 (终点)扫描方式*/
    ///DEBUG_DELAY ();
    
    /* column address control set */
    ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0xEF );
    
    /* page address control set */
   // DEBUG_DELAY ();
    ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x00 );
    ILI9341_Write_Data ( 0x01 );
    ILI9341_Write_Data ( 0x3F );
    
    /*  Pixel Format Set (3Ah)  */
   // DEBUG_DELAY ();
    ILI9341_Write_Cmd ( 0x3a ); 
    ILI9341_Write_Data ( 0x55 );
    
    /* Sleep Out (11h)  */
    ILI9341_Write_Cmd ( 0x11 );	

    /* Display ON (29h) */
    ILI9341_Write_Cmd ( 0x29 ); 
}





/*
------------------------------------------------------------
      模式0：		.     模式1：		.	  模式2：	  .	   模式3：					
            A		.			A		.		A		  .		A									
            |		.			|		.		|		  .		|							
            Y		.		    X		.		Y		  .		X					
            0		.			1		.		2		  .		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
模式4：				.	模式5：			.	模式6：			.	模式7：					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
            4		.		    5		.		6		  .		7	
            Y		.		    X		.		Y		  .		X						
            |		.		    |		.		|		  .		|							
            V		.			V		.		V		  .		V	
*/
void setScanOrigation(u8 ucOption) 
{
   //参数检查，只可输入0-7
	if(ucOption > 7) 
    {
		return;
	}
	//根据模式更新XY方向的像素宽度
	if(ucOption%2 == 0)	
	{
		//0 2 4 6模式下X方向像素宽度为240，Y方向为320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7模式下X方向像素宽度为320，Y方向为240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
	}

	//0x36命令参数的高3位可用于设置GRAM扫描方向	
	ILI9341_Write_Cmd ( 0x36 );
    ILI9341_Write_Data ( 0x08 |(ucOption << 5)   & ~(1<<4));//根据ucOption的值设置LCD参数，共0-7种模式
 
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );		/* x 起始坐标高8位 */
	ILI9341_Write_Data ( 0x00 );		/* x 起始坐标低8位 */
	ILI9341_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x 结束坐标高8位 */	
	ILI9341_Write_Data ( (LCD_X_LENGTH-1)&0xFF );				/* x 结束坐标低8位 */

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );		/* y 起始坐标高8位 */
	ILI9341_Write_Data ( 0x00 );		/* y 起始坐标低8位 */
	ILI9341_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y 结束坐标高8位 */	 
	ILI9341_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );				/* y 结束坐标低8位 */

	/* write gram start */
	ILI9341_Write_Cmd ( CMD_SetPixel );		
}

void showHan( uint16_t usX, uint16_t usY,uint8_t *buff) 
{
    uint8_t i,j,k;
    openWindow ( usX, usY, SIZE-1, SIZE-1 );
	ILI9341_Write_Cmd(0x2C);
    for(i=0;i<SIZE;i++)  
    {
        for(j = 0; j < SIZE/8; j++) 
        {
            for(k = 0; k < 8; k++) 
            {
                if((*buff << k) & 0x80 )    
                {
                    printf("*");
                    ILI9341_Write_Data(CurrentTextColor);
                } 
                else 
                {
                    printf(" ");
                    ILI9341_Write_Data(CurrentBackColor);
                }  
            }
            buff++;
        }
        printf("\r\n");
    }
}



static void ILI9341_SetCursor ( uint16_t usX, uint16_t usY )	
{
	openWindow ( usX, usY, 1, 1 );
}

static __inline void ILI9341_FillColor ( uint32_t ulAmout_Point, uint16_t usColor )
{
	uint32_t i = 0;
	
	
	/* memory write */
	ILI9341_Write_Cmd(0x2C);
		
	for ( i = 0; i < ulAmout_Point; i ++ )
		ILI9341_Write_Data ( usColor );
		
	
}

void ILI9341_SetPointPixel ( uint16_t usX, uint16_t usY )	
{	
	if (( usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH ))
  {
		ILI9341_SetCursor ( usX, usY );
		
		ILI9341_FillColor ( 1, CurrentTextColor );
	}
	
}


void ILI9341_SetPointPixel1 ( uint16_t usX, uint16_t usY,uint16_t usColor )	
{	
	if (( usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH ))
  {
		ILI9341_SetCursor ( usX, usY );
		
		ILI9341_FillColor ( 1, usColor );
	}
	
}


void ILI9341_DispString_EN_CH (uint16_t usX , uint16_t usY, char * pStr )
{
	uint16_t usCh;
	
	while( * pStr != '\0' )
	{
		if ( * pStr <= 126 )  	//英文字符
		{
			if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )  // 累计坐标如果超过一行，则自动换行
			{
				usX = ILI9341_DispWindow_X_Star;
				usY += LCD_Currentfonts->Height;
			}
			
			if ( ( usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY = ILI9341_DispWindow_Y_Star;
			}
		
		    ILI9341_DispChar_EN ( usX, usY, * pStr );
			
			usX +=  LCD_Currentfonts->Width; // x坐标自动加一个字符的宽度
		
		    pStr ++; // 数据指针往后走一位

		}
		
		else   //汉字字符
		{
			if (( usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY += HEIGHT_CH_CHAR;
			}
			
			if ( ( usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR ) > LCD_Y_LENGTH )
			{
				usX = ILI9341_DispWindow_X_Star;
				usY = ILI9341_DispWindow_Y_Star;
			}	
			
			usCh = * ( uint16_t * ) pStr;	
			
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			ILI9341_DispChar_CH ( usX, usY, usCh );
			
			usX += WIDTH_CH_CHAR;  // 坐标往后走一个字的宽度
			
			pStr += 2;           //一个汉字两个字节 
        }
		
    }	
}

void ILI9341_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar )
{
	//设置显示窗口
	openWindow ( usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR );
	
	ILI9341_Write_Cmd ( CMD_SetPixel );
    
    pBuffer = GetGBKCode_from_sd(pBuffer,usChar);
    
    showHan(usX,usY,pBuffer);
}

void ILI9341_DispString_CH (uint16_t usX , uint16_t usY, char * pStr )
{	
	uint16_t usCh;

	while( *pStr != '\0' )
	{		
		if ( ( usX - ILI9341_DispWindow_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += HEIGHT_CH_CHAR;
		}
		
		if ( ( usY - ILI9341_DispWindow_Y_Star + HEIGHT_CH_CHAR ) > LCD_Y_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}	
		
		usCh = * ( uint16_t * ) pStr;	
        usCh = ( usCh << 8 ) + ( usCh >> 8 );

		ILI9341_DispChar_CH ( usX, usY, usCh );
		
		usX += WIDTH_CH_CHAR;
		
		pStr += 2;           //一个汉字两个字节 

	}	   
	
}


void ILI9341_DispChar_EN ( uint16_t usX, uint16_t usY, const char cChar )
{
	uint8_t  byteCount, bitCount,fontLength;	
	uint16_t ucRelativePositon;
	uint8_t *Pfont;
	
	//对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
	ucRelativePositon = cChar - ' ';
	
	//每个字模的字节数
	fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height)/8;
		
	//字模首地址
	/*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	
	//设置显示窗口
	openWindow ( usX, usY, LCD_Currentfonts->Width-1, LCD_Currentfonts->Height-1);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );

	//按字节读取字模数据
	//由于前面直接设置了显示窗口，显示数据会自动换行
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
        //一位一位处理要显示的颜色
        for ( bitCount = 0; bitCount < 8; bitCount++ )
        {
            if ( Pfont[byteCount] & (0x80>>bitCount) )
                ILI9341_Write_Data ( CurrentTextColor );			
            else
                ILI9341_Write_Data ( CurrentBackColor );
        }	
	}	
}

void ILI9341_DispString_EN (uint16_t usX ,uint16_t usY,  char * pStr )
{
	while ( * pStr != '\0' )
	{
		if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += LCD_Currentfonts->Height;
		}
		
		if ( ( usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN ( usX, usY, * pStr);
		
		pStr ++;
		
		usX += LCD_Currentfonts->Width;
		
	}
	
}

void ILI9341_Clear ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{
	openWindow ( usX, usY, usWidth, usHeight );

	ILI9341_FillColor ( usWidth * usHeight, BACKGROUND);		
	
}

void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
    *TextColor = CurrentTextColor;
    *BackColor = CurrentBackColor;
}

/**
  * @brief  设置LCD的前景(字体)颜色,RGB565
  * @param  Color: 指定前景(字体)颜色 
  * @retval None
  */
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  设置LCD的背景颜色,RGB565
  * @param  Color: 指定背景颜色 
  * @retval None
  */
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}



