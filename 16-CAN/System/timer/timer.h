#ifndef _TIMER_H_
#define _TIMER_H_

void init_timer3(unsigned short int psc, unsigned short int arr);
void init_my_nvic(unsigned char interrupt_channel);
void init_timer3_pwd(void);
void init_timer2_pwd(void);
void timer3_init_input_capture(void);
unsigned int IC_TIM3_CH1_GetFreq(void);
unsigned int IC_TIM3_CH1_GetDuty(void); 
void timer3_init_ic(void); 
#endif
