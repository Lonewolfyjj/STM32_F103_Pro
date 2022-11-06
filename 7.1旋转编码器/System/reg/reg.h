#ifndef _REG_H_
#define _REG_H_

	#define RCC_APB2ENR		*((volatile unsigned int*)0x40021018)//RCC_APB2ENR 0x40021000 + 0x18
		
	
	#define GPIOA_CRH     *((volatile unsigned int*)0x40010804)
	
	#define GPIOB_CRL		  *((volatile unsigned int*)0x40010C00)//GPIOB_CRL     0X40010C00 + 0X00
	#define GPIOB_CRH		  *((volatile unsigned int*)0X40010C04)
	#define	GPIOB_ODR		  *((volatile unsigned int*)0x40010C0C)//GPIOB_ODR     0X40010C00 + 0X0C
		
	#define GPIOE_CRL		  *((volatile unsigned int*)0x40011800)//GPIOB_CRL     0x40011800 + 0X000
	#define	GPIOE_ODR		  *((volatile unsigned int*)0x4001180C)//GPIOB_ODR     0x4001180C + 0X0C
		
	#define BA(addr,n)   *((volatile unsigned int*) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5) + (n<<2)))

  #define RCC_CR    *((volatile unsigned int*)0x40021000)
	#define	RCC_CFGR  *((volatile unsigned int*)0x40021004)
	#define RCC_CIR   *((volatile unsigned int*)0x40021008)
	#define RCC_APB2RSTR  *((volatile unsigned int*)0x4002100c)
  #define	FLASH_ACR *((volatile unsigned int*)0x40022000)
	
	//----------波特率相关设置--------
	#define USART_SR    *((volatile unsigned int*)0x40013800)
  #define USART_DR    *((volatile unsigned int*)0x40013804)
	#define USART_BRR   *((volatile unsigned int*)0x40013808)
	#define USART_CR1   *((volatile unsigned int*)0x4001380c)
	
	
#endif
