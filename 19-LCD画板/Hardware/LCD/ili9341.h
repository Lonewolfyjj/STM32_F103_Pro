#ifndef  _ILI9341_H_
#define  _ILI9341_H_


#include "font.h"
#include "sys.h"


/******** ILI9341 显示屏的 FSMC 参数定义 *********/
//FSMC_Bank1_NORSRAM用于LCD命令操作的地址
#define      FSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x6c000000 )

//FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
#define      FSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x6c000800 )



/********数据信号 线***************/
// PGIOD
#define FSMC_DATA_PORTD_CLK                RCC_APB2Periph_GPIOD   
#define FSMC_DATA_PORTD         GPIOD
#define FSMC_D0            GPIO_Pin_14 // PD14
#define FSMC_D1            GPIO_Pin_15 // PD15
#define FSMC_D2            GPIO_Pin_0 // PD0
#define FSMC_D3            GPIO_Pin_1 // PD1
#define FSMC_D13           GPIO_Pin_8 //PD8
#define FSMC_D14           GPIO_Pin_9 //PD9
#define FSMC_D15           GPIO_Pin_10 //PD10


// GPIOE
#define FSMC_DATA_PORTE_CLK                RCC_APB2Periph_GPIOE  
#define FSMC_DATA_PORTE          GPIOE
#define FSMC_D4             GPIO_Pin_7 //PE7
#define FSMC_D5             GPIO_Pin_8 //PE8
#define FSMC_D6             GPIO_Pin_9 //PE9
#define FSMC_D7             GPIO_Pin_10 //PE10
#define FSMC_D8             GPIO_Pin_11 //PE11
#define FSMC_D9             GPIO_Pin_12 //PE12
#define FSMC_D10            GPIO_Pin_13 //PE13
#define FSMC_D11            GPIO_Pin_14 //PE14
#define FSMC_D12            GPIO_Pin_15 //PE15

/********背光***************/
// GPIOB
#define LCD_BACKLIGHT_PORT_CLK                RCC_APB2Periph_GPIOB  
#define LCD_BACKLIGHT_PORT                    GPIOB
#define LCD_BACKLIGHT_PIN                     GPIO_Pin_0

/********数据/命令选择 控制线***************/
#define FSMC_A10_PORT_CLK                RCC_APB2Periph_GPIOG  
#define FSMC_A10_PORT        			 GPIOG
#define FSMC_A10_PIN         			 GPIO_Pin_0


/********读使能 控制线***************/
#define FSMC_NOE_PORT_CLK     RCC_APB2Periph_GPIOD 
#define FSMC_NOE_PORT         GPIOD
#define FSMC_NOE_PIN          GPIO_Pin_4

/********写使能 控制线***************/
#define FSMC_NWE_PORT_CLK     RCC_APB2Periph_GPIOD
#define FSMC_NWE_PORT         GPIOD
#define FSMC_NWE_PIN          GPIO_Pin_5

/******** BANK 1 RANGE 4 片选控制线***************/
#define FSMC_NE4_PORT_CLK     RCC_APB2Periph_GPIOG
#define FSMC_NE4_PORT         GPIOG
#define FSMC_NE4_PIN          GPIO_Pin_12



#define      CMD_SetCoordinateX		 		    0x2A	     //设置X坐标
#define      CMD_SetCoordinateY		 		    0x2B	     //设置Y坐标
#define      CMD_SetPixel		 		          0x2C	     //填充像素


#define 			ILI9341_LESS_PIXEL	240			//液晶屏较短方向的像素宽度
#define 			ILI9341_MORE_PIXEL	320			//液晶屏较长方向的像素宽度
#define             ILI9341_DispWindow_X_Star		    0     //起始点的X坐标
#define             ILI9341_DispWindow_Y_Star		    0     //起始点的Y坐标


extern u16  LCD_X_LENGTH ;
extern u16  LCD_Y_LENGTH ;

//液晶屏扫描模式
//参数可选值为0-7
extern uint8_t LCD_SCAN_MODE;

#define      BACKGROUND		                BLACK   //默认背景颜色

#define      WHITE		 		           0xFFFF	   //白色
#define      BLACK                         0x0000	   //黑色 
#define      GREY                          0xF7DE	   //灰色 
#define      BLUE                          0x001F	   //蓝色 
#define      BLUE2                         0x051F	   //浅蓝色 
#define      RED                           0xF800	   //红色 
#define      MAGENTA                       0xF81F	   //红紫色，洋红色 
#define      GREEN                         0x07E0	   //绿色 
#define      CYAN                          0x7FFF	   //蓝绿色，青色 
#define      YELLOW                        0xFFE0	   //黄色 
#define      BRED                          0xF81F
#define      GRED                          0xFFE0
#define      GBLUE                         0x07FF


uint16_t ILI9341_Read_Data (void);
void ILI9341_Write_Data ( uint16_t usData );
void ILI9341_Write_Cmd ( uint16_t usCmd );

void FSMC_config(void);
void Gpio_config(void);
void init_lcd(void);
void openWindow(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height) ;
void set9341Config(void);
void ILI9341_BackLed_Control ( FunctionalState enumState );
uint16_t ILI9341_ReadID(void);
void ILI9341_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled );
void drawRect(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height,u16 color) ;
void setScanOrigation(u8 ucOption) ;
void ILI9341_SetPointPixel ( uint16_t usX, uint16_t usY );
void ILI9341_SetPointPixel1 ( uint16_t usX, uint16_t usY,uint16_t usColor )	;
void showHan( uint16_t usX, uint16_t usY,uint8_t *buff) ;
void openWindow(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height) ;
void ILI9341_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar );
void ILI9341_DispString_CH (uint16_t usX , uint16_t usY, char * pStr );
void ILI9341_DispChar_EN ( uint16_t usX, uint16_t usY, const char cChar );
void ILI9341_DispString_EN ( 	uint16_t usX ,uint16_t usY,  char * pStr );
void ILI9341_Clear ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void LCD_SetBackColor(uint16_t Color);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) ;
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor);
void ILI9341_Clear ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight );
void ILI9341_DispString_EN_CH (uint16_t usX , uint16_t usY, char * pStr );
void ILI9341_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 );
void ILI9341_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled );
void ILI9341_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled );
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont (void);
void ILI9341_DispStringLine_EN (  uint16_t line,  char * pStr );

#endif
