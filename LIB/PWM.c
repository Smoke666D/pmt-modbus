//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "PWM.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void PWM_Inin(uint16_t pin, GPIO_TypeDef* port, TIM_TypeDef* TIMx, uint8_t chanel)
{
  GPIO_InitTypeDef              GPIO_InitStruct;        // Структура инициализирующих данных GPIO
  TIM_TimeBaseInitTypeDef       TIM_BaseConfig;         // Структура инициализации часов
  TIM_OCInitTypeDef             TIM_OCConfig;           // Конфигурация выхода таймера
  //********************************RCC*****************************************
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;        // Включаем тактирование альтернативных функций
  if(port == GPIOA)
  {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  }
  else if(port == GPIOB)
  {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  }
  else if(port == GPIOC)
  {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  }
  else if(port == GPIOD)
  {
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
  }
  
  if(TIMx == TIM2)
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  }
  else if(TIMx == TIM3)
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  }
  else if(TIMx == TIM4)
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
  } 
  #if defined (STM32F10X_HD) || defined  (STM32F10X_CL)
    else if(TIMx == TIM5)
    {
      RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    }
    else if(TIMx == TIM6)
    {
      RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    } 
    else if(TIMx == TIM7)
    {
      RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    }
  #endif
  //******************************GPIO AF***************************************
  GPIO_InitStruct.GPIO_Pin   = pin;                     // Иницилизируем пины
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;         // Режим входа
  GPIO_Init(port, &GPIO_InitStruct);                    // Заданные настройки сохраняем в регистрах  
  //*******************************TIM******************************************
  TIM_OCStructInit(&TIM_OCConfig);                      // Стадартная инициализация структуры инициализации
  TIM_TimeBaseStructInit(&TIM_BaseConfig);              // Стадартная инициализация структуры инициализации
  TIM_BaseConfig.TIM_Prescaler     = 1;                 // Предделитель 1 
  TIM_BaseConfig.TIM_ClockDivision = TIM_CKD_DIV1;      // Делитель частот 1
  TIM_BaseConfig.TIM_Period        = 0xFFFF;            // Частота шима
  TIM_BaseConfig.TIM_CounterMode   = TIM_CounterMode_Up;// Отсчет от нуля до TIM_Period
  TIM_TimeBaseInit(TIMx, &TIM_BaseConfig);              // Инициализируем таймер №4
  //******************************PWM*******************************************
  TIM_OCConfig.TIM_OCMode      = TIM_OCMode_PWM1;       // Конфигурируем выход таймера, режим - PWM1   
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;// Собственно - выход включен
  TIM_OCConfig.TIM_OCPolarity  = TIM_OCPolarity_High;   // Полярность => пульс - это единица (+3.3V)
  TIM_OCConfig.TIM_Pulse       = 0;                     // Пульс длинной   
  switch(chanel)
  {
    case 1:
      TIM_OC1Init(TIMx, &TIM_OCConfig);                 // Инициализируем 1 выход таймера
      break;
    case 2:
      TIM_OC2Init(TIMx, &TIM_OCConfig);                 // Инициализируем 2 выход таймера
      break;
    case 3:
      TIM_OC3Init(TIMx, &TIM_OCConfig);                 // Инициализируем 3 выход таймера
      break;
    case 4:
      TIM_OC4Init(TIMx, &TIM_OCConfig);                 // Инициализируем 4 выход таймера
      break;
  }
  TIM_CtrlPWMOutputs(TIMx, ENABLE);                     // Включаем ШИМ выхода
  TIM_Cmd(TIMx, ENABLE);                                // Включаем таймер
  return;
}
         
void SetPWM(TIM_TypeDef* TIMx, uint8_t chanel, uint16_t data)
{
  switch(chanel)
  {
    case 1:
      TIMx->CCR1 = data;
      break;
    case 2:
      TIMx->CCR2 = data;
      break;
    case 3:
      TIMx->CCR3 = data;
      break;
    case 4:
      TIMx->CCR4 = data;
      break;
  }
  return;
}