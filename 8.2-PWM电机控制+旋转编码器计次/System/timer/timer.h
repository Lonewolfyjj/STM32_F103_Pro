#ifndef _TIMER_H_
#define _TIMER_H_

void init_timer3(unsigned short int psc, unsigned short int arr);
void init_my_nvic(unsigned char interrupt_channel);
void init_timer3_pwd(void) ;
void setPwmDuty(int32_t duty);

extern uint8_t duty_width;
#endif
