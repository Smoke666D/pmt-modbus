#ifndef _PWM_H
#define _PWM_H
//******************************FUNCTIONS***************************************
void PWM_Inin(uint16_t pin, GPIO_TypeDef* port, TIM_TypeDef* TIMx, uint8_t chanel);
void SetPWM(TIM_TypeDef* TIMx, uint8_t chanel, uint16_t data);
//******************************************************************************
#endif
