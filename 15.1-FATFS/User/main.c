#include "reg.h"
#include "delay.h"
#include "typedef.h"
#include "usart.h"
#include "timer.h"
#include "spi.h"
#include "led.h"
#include "w25q128.h"
#include "ff.h"

void Rcc_init(void);

static FRESULT file_check(void);
FILINFO fno;
FATFS fs;													/* FatFs文件系统对象 */
FRESULT res_flash;                /* 文件操作结果 */
FIL fnew;													/* 文件对象 */
BYTE work[FF_MAX_SS]; //一定是一个全局变量
BYTE WriteBuffer[] =              /* 写缓冲区*/
"你好，这是我的第一个FATFS 的文件系统测试程序"; 
BYTE ReadBuffer[1024]={0};
UINT fnum;            					  /* 文件成功读写数量 */
int main()
{ 
	Rcc_init();
	delay_init();
	led_init();
  usart_init(72,115200);

  printf("****** 这是一个SPI FLASH 文件系统实验 ******\r\n");
   
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	//初始化函数调用流程如下
	//f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	res_flash = f_mount(&fs,"0:",1);
	printf("res_flash = %d\r\n",res_flash);
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("FLASH还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_flash=f_mkfs("0:",0,work,sizeof(work));
		if(res_flash == FR_OK)
		{
			printf("》FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_flash = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_flash = f_mount(&fs,"0:",1);
		}
		else
		{
			LED_RED = 0;
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}	
  else if(res_flash!=FR_OK)
  {
    printf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
    printf("！！可能原因：SPI Flash初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }	
	
	/*------------------- 文件系统测试：写测试 --------------------------*/
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	res_flash = f_open(&fnew, "0:新建文本文档abc.txt",FA_CREATE_ALWAYS | FA_WRITE);
	if(res_flash == FR_OK )
	{
		printf("》打开/创建新建文本文档abc.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash == FR_OK)
    {
      printf("文件写入成功，写入字节数据：%d\r\n",fnum);
			delay_ms(1);
      printf("向文件写入的数据为：\r\n%s\r\n",WriteBuffer);		
      delay_ms(1);			
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res_flash);
    }    
		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
		LED_RED = 0;
		printf("！！打开/创建文件失败。\r\n");
	}
	
  file_check();
	
	
  /*------------------- 文件系统测试：读测试 --------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
	res_flash = f_open(&fnew, "0:新建文本文档abc.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GRREN = 0;
		printf("》打开文件成功。\r\n");
	  
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      printf(" 文件读取成功,读到字节数据：%d\r\n",fnum);
      printf(" 读取得的文件数据为：\r\n %s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\r\n",res_flash);
    }		
	}
	else
	{
		LED_RED = 0;
		printf("！！打开文件失败(%d)。\r\n",res_flash);
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
  
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"0:",1);
	while(1)
	{ 
    
	}
}


static FRESULT file_check(void)
{  
  /* 获取文件信息 */
  res_flash=f_stat("0:新建文本文档abc.txt",&fno);
	 printf("res_flash = %d\r\n",res_flash);
  if(res_flash==FR_OK)
  {
    printf("“新建文本文档abc.txt”文件信息：\r\n");
    printf("》文件大小: %ld(字节)\r\n", fno.fsize);
    printf("》时间戳: %u/%02u/%02u, %02u:%02u\r\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("》属性: %c%c%c%c%c\r\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',      // 是一个目录
           (fno.fattrib & AM_RDO) ? 'R' : '-',      // 只读文件
           (fno.fattrib & AM_HID) ? 'H' : '-',      // 隐藏文件
           (fno.fattrib & AM_SYS) ? 'S' : '-',      // 系统文件
           (fno.fattrib & AM_ARC) ? 'A' : '-');     // 档案文件
  }
  return res_flash;
}

void Rcc_init(void) 
{
	// 1.RCC 寄存器打开内部时钟
	RCC1->CR |= 1<<0;
	// 2. 关闭RCC_CIR 中断，避免中断干扰时钟设置
  RCC1->CIR = 0;
  // 3. 设置HSE 的信号源外部晶振不旁路，启动HSE 	
	RCC1->CR &= ~(1<<18);
	// 4. 开启HSE
	RCC1->CR |= 1<<16;
	// 5. 等待HSE 就绪
	while((RCC->CR & (1<<17)) == 0);
	// 6. 配置 AHP APB1 APB2分频倍频系数
	RCC1->CFGR &= ~(1<<7); // AHB=DIV1;   
	RCC1->CFGR |= 4<<8;    // APB1=DIV2 
	RCC1->CFGR &= ~(1<<13); // APB2=DIV1;
	// 7. 关闭PLL
	RCC1->CR &= ~(1<<24);  // 关闭PLL
	// 8. 配置PLL 分频倍频
	RCC1->CFGR |= 0xf<<18; // 16 倍频
	RCC1->CFGR |= 1<<16; // PLLSRC时钟源选择
	RCC1->CFGR |= 1<<17; //PLLXTPRE 分频选择
  FLASH_ACR |=0x32; //FLASH 2个延时周期
  // 9. 使能PLL
	RCC1->CR |= (1<<24); // 开启PLL
	// 10. 等待PLL 使能OK
	while((RCC1->CR & (1<<25)) == 0); 
	// 11. 选择主PLL 作为时钟
	RCC1->CFGR &= ~(3<<0); // 清零
	RCC1->CFGR |= 2<<0;
	// 12.等待主PLL稳定
	while((RCC1->CFGR & (3<<2)) != (2<<2));	

}


