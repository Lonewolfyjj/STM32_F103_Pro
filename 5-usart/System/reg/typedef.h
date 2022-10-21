#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#define RCC_BASE_ADDRESS 0x40021000


#define GPIOA_BASE_ADDRESS 0x40010800 
#define GPIOB_BASE_ADDRESS 0X40010C00 
#define GPIOC_BASE_ADDRESS 0x40011000
#define GPIOD_BASE_ADDRESS 0x40011400
#define GPIOE_BASE_ADDRESS 0x40011800
#define GPIOF_BASE_ADDRESS 0x40011C00
#define GPIOG_BASE_ADDRESS 0x40012000


typedef struct {
	volatile unsigned int CR;
	volatile unsigned int CFGR;
	volatile unsigned int CIR;
	volatile unsigned int APB2RSTR; // APB2ÍâÉè¸´Î»¼Ä´æÆ÷
	volatile unsigned int APB1RSTR; 
  volatile unsigned int AHBENR;
	volatile unsigned int APB2ENR;
	volatile unsigned int APB1ENR;
	volatile unsigned int BDCR;
	volatile unsigned int CSR;
} RCC_TYPE_DEF;


typedef struct {
	volatile unsigned int CRL;
	volatile unsigned int CRH;
	volatile unsigned int IDR;
	volatile unsigned int ODR;
	volatile unsigned int BSRR;
	volatile unsigned int BRR;
	volatile unsigned int LCKR;
} GPIO_TYPE_DEF;


#define RCC1   ((RCC_TYPE_DEF *) RCC_BASE_ADDRESS)
#define GPIOB1  ((GPIO_TYPE_DEF *) GPIOB_BASE_ADDRESS)
#define GPIOE1  ((GPIO_TYPE_DEF *) GPIOE_BASE_ADDRESS)

#endif
