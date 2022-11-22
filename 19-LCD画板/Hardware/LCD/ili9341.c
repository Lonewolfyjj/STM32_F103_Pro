#include "ili9341.h"
#include "usart.h"
#include "delay.h"
#include "font.h"

u16  LCD_X_LENGTH   =   ILI9341_LESS_PIXEL;
u16  LCD_Y_LENGTH   =   ILI9341_MORE_PIXEL;

static sFONT *LCD_Currentfonts = &Font8x16;  //Ӣ������

static uint16_t CurrentTextColor   = WHITE;//ǰ��ɫ
static uint16_t CurrentBackColor   = BLACK;//����ɫ

uint8_t * pBuffer;
uint8_t LCD_SCAN_MODE = 5;

void init_lcd() 
{

	Gpio_config();
	FSMC_config();
    delay_ms(50);     
    ILI9341_BackLed_Control(ENABLE);
    ILI9341_ReadID();
    set9341Config();
    setScanOrigation(LCD_SCAN_MODE);
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
    
    /*���ñ�������*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_BACKLIGHT_PIN;
    GPIO_Init(LCD_BACKLIGHT_PORT, &GPIO_InitStructure);

  
    /* ����FSMC���Ӧ��������,FSMC-D0~D15 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Pin = FSMC_D0 | FSMC_D1 | FSMC_D2 | FSMC_D3 | FSMC_D13 | FSMC_D14 | FSMC_D15;
    GPIO_Init(FSMC_DATA_PORTD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = FSMC_D4 | FSMC_D5 | FSMC_D6 | FSMC_D7 | FSMC_D8 | FSMC_D9 | FSMC_D10 | FSMC_D11 | FSMC_D12;
    GPIO_Init(FSMC_DATA_PORTE, &GPIO_InitStructure);

    
    /*��������/����ѡ��*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = FSMC_A10_PIN;
    GPIO_Init(FSMC_A10_PORT, &GPIO_InitStructure);
    
    /*��ʹ�� ������*/
    GPIO_InitStructure.GPIO_Pin = FSMC_NOE_PIN;
    GPIO_Init(FSMC_NOE_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = FSMC_NWE_PIN;
    GPIO_Init(FSMC_NWE_PORT, &GPIO_InitStructure);
    
    /*BANK 1 RANGE 4 Ƭѡ������*/
    GPIO_InitStructure.GPIO_Pin = FSMC_NE4_PIN;
    GPIO_Init(FSMC_NE4_PORT, &GPIO_InitStructure);
    
    
}


void FSMC_config()
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  ReadWriteTiming;

		
	/*ʹ��FSMC����ʱ��*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );
    
	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/72M=14ns
	ReadWriteTiming.FSMC_AddressSetupTime        = 0x01;	
    //���ݱ���ʱ�䣨DATAST��+ 1��HCLK = 3/72M=42ns
	ReadWriteTiming.FSMC_DataSetupTime           = 0x02;	
    //ѡ��ƥ��SRAM��ģʽ
	ReadWriteTiming.FSMC_AccessMode              = FSMC_AccessMode_B;	 
    
	//��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
	ReadWriteTiming.FSMC_AddressHoldTime         = 0x00;	 
	//��������ת�����ڣ������ڸ���ģʽ��NOR����
	ReadWriteTiming.FSMC_BusTurnAroundDuration   = 0x00;
	//����ʱ�ӷ�Ƶ��������ͬ�����͵Ĵ洢��
	ReadWriteTiming.FSMC_CLKDivision             = 0x00;	
	//���ݱ���ʱ�䣬������ͬ���͵�NOR
	ReadWriteTiming.FSMC_DataLatency             = 0x00;		

    ReadWriteTiming.FSMC_BusTurnAroundDuration   = 0x00;


	// ѡ��FSMCӳ��Ĵ洢���� Bank1 sram4
	FSMC_NORSRAMInitStructure.FSMC_Bank               = FSMC_Bank1_NORSRAM4;
	
	//���õ�ַ���������������Ƿ��ã�������NOR
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux     = FSMC_DataAddressMux_Disable; 
	
	//����Ҫ���ƵĴ洢�����ͣ�SRAM����
	FSMC_NORSRAMInitStructure.FSMC_MemoryType         = FSMC_MemoryType_NOR;   
	
	//�洢�����ݿ�ȣ�16λ
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth    = FSMC_MemoryDataWidth_16b; 
	
	//�����Ƿ�ʹ��ͻ������ģʽ��������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode    = FSMC_BurstAccessMode_Disable;
	
	//�����Ƿ�ʹ�ܵȴ��źţ�������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait   = FSMC_AsynchronousWait_Disable;
	
	//���õȴ��źŵ���Ч���ԣ�������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	
	//�����Ƿ�֧�ְѷǶ����ͻ��������������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_WrapMode           = FSMC_WrapMode_Disable; 
	
	//���õȴ��źŲ����ʱ�䣬������ͬ�����͵Ĵ洢��
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive   = FSMC_WaitSignalActive_BeforeWaitState;
	
	//�洢��дʹ�� 
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation     = FSMC_WriteOperation_Enable;
	
	//��ʹ�õȴ��ź�
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal         = FSMC_WaitSignal_Disable;  		
	
	// ��ʹ����չģʽ����дʹ����ͬ��ʱ��
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode       = FSMC_ExtendedMode_Disable; // FSMC_ExtendedMode_Enable
	
	//ͻ��д����
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst         = FSMC_WriteBurst_Disable;   
	
	//��дʱ������
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &ReadWriteTiming;
	
	//��дͬ��ʱ��ʹ����չģʽʱ������ò���Ч
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &ReadWriteTiming; 

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  //ʹ��BANK1 

}



/**
  * @brief  ��ILI9341д������
  * @param  usCmd :Ҫд��������Ĵ�����ַ��
  * @retval ��
  */	
__inline void ILI9341_Write_Cmd ( uint16_t usCmd )
{
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = usCmd;
	
}

/**
  * @brief  ��ILI9341д������
  * @param  usCmd :Ҫд��������Ĵ�����ַ��
  * @retval ��
  */	
__inline void ILI9341_Write_Data ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = usData;
	
}


/**
  * @brief  ��ILI9341��ȡ����
  * @param  ��
  * @retval ��ȡ��������
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

    ILI9341_Write_Cmd(0x2A);  // ����ȷ�����������С
    ILI9341_Write_Data((uint8_t)(startX >> 8));  
    ILI9341_Write_Data((uint8_t)(startX & 0XFF));  
    ILI9341_Write_Data((uint8_t)((startX + width) >> 8));  
    ILI9341_Write_Data((uint8_t)((startX + width) & 0XFF));  
    
    
    
    ILI9341_Write_Cmd(0x2B);  // ����ȷ�����������С
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
    ILI9341_Write_Data ( 0x06 ); //ԭ����0x02��Ϊ0x06�ɷ�ֹҺ����ʾ����ʱ�����Ƶ����
    
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
    ILI9341_Write_Data ( 0x28 );    /*����  ���Ͻǵ� (���)�����½� (�յ�)ɨ�跽ʽ*/
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
      ģʽ6��		.     ģʽ7��		.	  ģʽ4��	  .	   ģʽ5��					
            A		.			A		.		A		  .		A									
            |		.			|		.		|		  .		|							
            Y		.		    X		.		Y		  .		X					
            0		.			1		.		2		  .		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
ģʽ2��				.	ģʽ3��			.	ģʽ0��			.	ģʽ1��					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
            4		.		    5		.		6		  .		7	
            Y		.		    X		.		Y		  .		X						
            |		.		    |		.		|		  .		|							
            V		.			V		.		V		  .		V	
ģʽ1 ���徵��
ģʽ2 ���徵��
ģʽ4 ���徵��
ģʽ7 ���徵��

*/
void setScanOrigation(u8 ucOption) 
{
   //������飬ֻ������0-7
	if(ucOption > 7) 
    {
		return;
	}
    LCD_SCAN_MODE = ucOption;
	//����ģʽ����XY��������ؿ��
	if(ucOption%2 == 0)	
	{
		//0 2 4 6ģʽ��X�������ؿ��Ϊ240��Y����Ϊ320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7ģʽ��X�������ؿ��Ϊ320��Y����Ϊ240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
	}

	//0x36��������ĸ�3λ����������GRAMɨ�跽��	
	ILI9341_Write_Cmd ( 0x36 );
    ILI9341_Write_Data ( 0x08 |(ucOption << 5)   & ~(1<<4));//����ucOption��ֵ����LCD��������0-7��ģʽ
 
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );		/* x ��ʼ�����8λ */
	ILI9341_Write_Data ( 0x00 );		/* x ��ʼ�����8λ */
	ILI9341_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x ���������8λ */	
	ILI9341_Write_Data ( (LCD_X_LENGTH-1)&0xFF );				/* x ���������8λ */

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );		/* y ��ʼ�����8λ */
	ILI9341_Write_Data ( 0x00 );		/* y ��ʼ�����8λ */
	ILI9341_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y ���������8λ */	 
	ILI9341_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );				/* y ���������8λ */

	/* write gram start */
	ILI9341_Write_Cmd ( CMD_SetPixel );		
}

void showHan( uint16_t usX, uint16_t usY,uint8_t *buff) 
{
    uint8_t i,j,k;
    openWindow ( usX, usY, WIDTH_CH_CHAR-1, HEIGHT_CH_CHAR-1 );
	ILI9341_Write_Cmd(0x2C);
    for(i=0; i < WIDTH_CH_CHAR; i++)  
    {
        for(j = 0; j < WIDTH_CH_CHAR/8; j++) 
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
		if ( * pStr <= 126 )  	//Ӣ���ַ�
		{
			if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )  // �ۼ������������һ�У����Զ�����
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
			
			usX +=  LCD_Currentfonts->Width; // x�����Զ���һ���ַ��Ŀ��
		
		    pStr ++; // ����ָ��������һλ

		}
		
		else   //�����ַ�
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
			
			usX += WIDTH_CH_CHAR;  // ����������һ���ֵĿ��
			
			pStr += 2;           //һ�����������ֽ� 
        }
		
    }	
}

void ILI9341_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar )
{
	//������ʾ����
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
		
		pStr += 2;           //һ�����������ֽ� 

	}	   
	
}


void ILI9341_DispChar_EN ( uint16_t usX, uint16_t usY, const char cChar )
{
	uint8_t  byteCount, bitCount,fontLength;	
	uint16_t ucRelativePositon;
	uint8_t *Pfont;
	
	//��ascii���ƫ�ƣ���ģ������ASCII���ǰ32����ͼ�η��ţ�
	ucRelativePositon = cChar - ' ';
	
	//ÿ����ģ���ֽ���
	fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height)/8;
		
	//��ģ�׵�ַ
	/*ascii���ƫ��ֵ����ÿ����ģ���ֽ����������ģ��ƫ��λ��*/
	Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	
	//������ʾ����
	openWindow ( usX, usY, LCD_Currentfonts->Width-1, LCD_Currentfonts->Height-1);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );

	//���ֽڶ�ȡ��ģ����
	//����ǰ��ֱ����������ʾ���ڣ���ʾ���ݻ��Զ�����
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
        //һλһλ����Ҫ��ʾ����ɫ
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

void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}


void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
    *TextColor = CurrentTextColor;
    *BackColor = CurrentBackColor;
}

/**
  * @brief  ����LCD��ǰ��(����)��ɫ,RGB565
  * @param  Color: ָ��ǰ��(����)��ɫ 
  * @retval None
  */
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  ����LCD�ı�����ɫ,RGB565
  * @param  Color: ָ��������ɫ 
  * @retval None
  */
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}

sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
 * @brief  �� ILI9341 ��ʾ����ʹ�� Bresenham �㷨���߶� 
 * @param  usX1 �����ض�ɨ�跽�����߶ε�һ���˵�X����
 * @param  usY1 �����ض�ɨ�跽�����߶ε�һ���˵�Y����
 * @param  usX2 �����ض�ɨ�跽�����߶ε���һ���˵�X����
 * @param  usY2 �����ض�ɨ�跽�����߶ε���һ���˵�Y����
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9341_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 )
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //������������ 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //���õ������� 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//��ֱ�� 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//ˮƽ�� 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //ѡȡ�������������� 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//������� 
	{  
		ILI9341_SetPointPixel ( usX_Current, usY_Current );//���� 
		
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 
		
		if ( lError_X > lDistance ) 
		{ 
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  
		
		if ( lError_Y > lDistance ) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 
		
	}
}    
	
	/**
 * @brief  �� ILI9341 ��ʾ���ϻ�һ������
 * @param  usX_Start �����ض�ɨ�跽���¾��ε���ʼ��X����
 * @param  usY_Start �����ض�ɨ�跽���¾��ε���ʼ��Y����
 * @param  usWidth�����εĿ�ȣ���λ�����أ�
 * @param  usHeight�����εĸ߶ȣ���λ�����أ�
 * @param  ucFilled ��ѡ���Ƿ����þ���
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :���ľ���
  *     @arg 1 :ʵ�ľ��� 
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9341_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled )
{
	if ( ucFilled )
	{
		openWindow ( usX_Start, usY_Start, usWidth, usHeight );
		ILI9341_FillColor ( usWidth * usHeight ,CurrentTextColor);	
	}
	else
	{
		ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start );
		ILI9341_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );
		ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1 );
		ILI9341_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );		
	}

}

/**
 * @brief  �� ILI9341 ��ʾ����ʹ�� Bresenham �㷨��Բ
 * @param  usX_Center �����ض�ɨ�跽����Բ�ĵ�X����
 * @param  usY_Center �����ض�ɨ�跽����Բ�ĵ�Y����
 * @param  usRadius��Բ�İ뾶����λ�����أ�
 * @param  ucFilled ��ѡ���Ƿ�����Բ
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :����Բ
  *     @arg 1 :ʵ��Բ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled )
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;
	
	
	sCurrentX = 0; sCurrentY = usRadius;	  
	
	sError = 3 - ( usRadius << 1 );     //�ж��¸���λ�õı�־
	
	
	while ( sCurrentX <= sCurrentY )
	{
		int16_t sCountY;
		
		
		if ( ucFilled ) 			
			for ( sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++ ) 
			{                      
				ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCountY );           //1���о����� 
				ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCountY );           //2       
				ILI9341_SetPointPixel ( usX_Center - sCountY,   usY_Center + sCurrentX );           //3
				ILI9341_SetPointPixel ( usX_Center - sCountY,   usY_Center - sCurrentX );           //4
				ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCountY );           //5    
        ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCountY );           //6
				ILI9341_SetPointPixel ( usX_Center + sCountY,   usY_Center - sCurrentX );           //7 	
        ILI9341_SetPointPixel ( usX_Center + sCountY,   usY_Center + sCurrentX );           //0				
			}
		
		else
		{          
			ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCurrentY );             //1���о�����
			ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCurrentY );             //2      
			ILI9341_SetPointPixel ( usX_Center - sCurrentY, usY_Center + sCurrentX );             //3
			ILI9341_SetPointPixel ( usX_Center - sCurrentY, usY_Center - sCurrentX );             //4
			ILI9341_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCurrentY );             //5       
			ILI9341_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCurrentY );             //6
			ILI9341_SetPointPixel ( usX_Center + sCurrentY, usY_Center - sCurrentX );             //7 
			ILI9341_SetPointPixel ( usX_Center + sCurrentY, usY_Center + sCurrentX );             //0
    }			
		
		
		sCurrentX ++;

		
		if ( sError < 0 ) 
			sError += 4 * sCurrentX + 6;	  
		
		else
		{
			sError += 10 + 4 * ( sCurrentX - sCurrentY );   
			sCurrentY --;
		} 	
	}
}


/**
  * @brief  ����Ӣ����������
  * @param  fonts: ָ��Ҫѡ�������
	*		����Ϊ����ֵ֮һ
  * 	@arg��Font24x32;
  * 	@arg��Font16x24;
  * 	@arg��Font8x16;
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}



/**
 * @brief  �� ILI9341 ��ʾ������ʾӢ���ַ���
 * @param  line �����ض�ɨ�跽�����ַ�������ʼY����
  *   ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽָ���������꣬
  *   ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ��
	*		��ʾ������ʹ��LINE��ʱ����Ҫ��Ӣ���������ó�Font8x16
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9341_DispStringLine_EN (  uint16_t line,  char * pStr )
{
	uint16_t usX = 0;
	
	while ( * pStr != '\0' )
	{
		if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			line += LCD_Currentfonts->Height;
		}
		
		if ( ( line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			line = ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN ( usX, line, * pStr);
		
		pStr ++;
		
		usX += LCD_Currentfonts->Width;
		
	}
	
}
