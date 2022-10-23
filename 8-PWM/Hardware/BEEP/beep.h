#ifndef _BEEP_H_
#define _BEEP_H_
  void beep_init(void);
	#define GPIOB_ODR_Address 0x40010C0C
  #define PBout1(n)  BA(GPIOB_ODR_Address,n)            
  #define BEEP PBout1(8)
#endif

	
