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
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT res_flash;                /* �ļ�������� */
FIL fnew;													/* �ļ����� */
BYTE work[FF_MAX_SS]; //һ����һ��ȫ�ֱ���
BYTE WriteBuffer[] =              /* д������*/
"��ã������ҵĵ�һ��FATFS ���ļ�ϵͳ���Գ���"; 
BYTE ReadBuffer[1024]={0};
UINT fnum;            					  /* �ļ��ɹ���д���� */
int main()
{ 
	Rcc_init();
	delay_init();
	led_init();
  usart_init(72,115200);

  printf("****** ����һ��SPI FLASH �ļ�ϵͳʵ�� ******\r\n");
   
	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	//��ʼ������������������
	//f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	res_flash = f_mount(&fs,"0:",1);
	printf("res_flash = %d\r\n",res_flash);
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("FLASH��û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_flash=f_mkfs("0:",0,work,sizeof(work));
		if(res_flash == FR_OK)
		{
			printf("��FLASH�ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_flash = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_flash = f_mount(&fs,"0:",1);
		}
		else
		{
			LED_RED = 0;
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}	
  else if(res_flash!=FR_OK)
  {
    printf("�����ⲿFlash�����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
    printf("��������ԭ��SPI Flash��ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }	
	
	/*------------------- �ļ�ϵͳ���ԣ�д���� --------------------------*/
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_flash = f_open(&fnew, "0:�½��ı��ĵ�abc.txt",FA_CREATE_ALWAYS | FA_WRITE);
	if(res_flash == FR_OK )
	{
		printf("����/�����½��ı��ĵ�abc.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash == FR_OK)
    {
      printf("�ļ�д��ɹ���д���ֽ����ݣ�%d\r\n",fnum);
			delay_ms(1);
      printf("���ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);		
      delay_ms(1);			
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
		LED_RED = 0;
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
  file_check();
	
	
  /*------------------- �ļ�ϵͳ���ԣ������� --------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
	res_flash = f_open(&fnew, "0:�½��ı��ĵ�abc.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GRREN = 0;
		printf("�����ļ��ɹ���\r\n");
	  
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      printf(" �ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf(" ��ȡ�õ��ļ�����Ϊ��\r\n %s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\r\n",res_flash);
    }		
	}
	else
	{
		LED_RED = 0;
		printf("�������ļ�ʧ��(%d)��\r\n",res_flash);
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	
  
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"0:",1);
	while(1)
	{ 
    
	}
}


static FRESULT file_check(void)
{  
  /* ��ȡ�ļ���Ϣ */
  res_flash=f_stat("0:�½��ı��ĵ�abc.txt",&fno);
	 printf("res_flash = %d\r\n",res_flash);
  if(res_flash==FR_OK)
  {
    printf("���½��ı��ĵ�abc.txt���ļ���Ϣ��\r\n");
    printf("���ļ���С: %ld(�ֽ�)\r\n", fno.fsize);
    printf("��ʱ���: %u/%02u/%02u, %02u:%02u\r\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("������: %c%c%c%c%c\r\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',      // ��һ��Ŀ¼
           (fno.fattrib & AM_RDO) ? 'R' : '-',      // ֻ���ļ�
           (fno.fattrib & AM_HID) ? 'H' : '-',      // �����ļ�
           (fno.fattrib & AM_SYS) ? 'S' : '-',      // ϵͳ�ļ�
           (fno.fattrib & AM_ARC) ? 'A' : '-');     // �����ļ�
  }
  return res_flash;
}

void Rcc_init(void) 
{
	// 1.RCC �Ĵ������ڲ�ʱ��
	RCC1->CR |= 1<<0;
	// 2. �ر�RCC_CIR �жϣ������жϸ���ʱ������
  RCC1->CIR = 0;
  // 3. ����HSE ���ź�Դ�ⲿ������·������HSE 	
	RCC1->CR &= ~(1<<18);
	// 4. ����HSE
	RCC1->CR |= 1<<16;
	// 5. �ȴ�HSE ����
	while((RCC->CR & (1<<17)) == 0);
	// 6. ���� AHP APB1 APB2��Ƶ��Ƶϵ��
	RCC1->CFGR &= ~(1<<7); // AHB=DIV1;   
	RCC1->CFGR |= 4<<8;    // APB1=DIV2 
	RCC1->CFGR &= ~(1<<13); // APB2=DIV1;
	// 7. �ر�PLL
	RCC1->CR &= ~(1<<24);  // �ر�PLL
	// 8. ����PLL ��Ƶ��Ƶ
	RCC1->CFGR |= 0xf<<18; // 16 ��Ƶ
	RCC1->CFGR |= 1<<16; // PLLSRCʱ��Դѡ��
	RCC1->CFGR |= 1<<17; //PLLXTPRE ��Ƶѡ��
  FLASH_ACR |=0x32; //FLASH 2����ʱ����
  // 9. ʹ��PLL
	RCC1->CR |= (1<<24); // ����PLL
	// 10. �ȴ�PLL ʹ��OK
	while((RCC1->CR & (1<<25)) == 0); 
	// 11. ѡ����PLL ��Ϊʱ��
	RCC1->CFGR &= ~(3<<0); // ����
	RCC1->CFGR |= 2<<0;
	// 12.�ȴ���PLL�ȶ�
	while((RCC1->CFGR & (3<<2)) != (2<<2));	

}


