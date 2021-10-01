//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "DIO.h"
#include "stm32f10x_gpio.h"
//******************************************************************************
//******************************************************************************
//******************************************************************************
void DOUT_Init(uint16_t pin, GPIO_TypeDef* port)
{
  GPIO_InitTypeDef      GPIO_InitStruct;                // Структура инициализирующих данных GPIO
  //********************************RCC*****************************************
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
  //********************************GPIO****************************************
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // Макс скорость
  GPIO_InitStruct.GPIO_Pin   = pin;                     // Настройки вывода
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;        // Режим выхода без подтягивающих резисторов
  GPIO_Init(port, &GPIO_InitStruct);                    // Заданные настройки сохраняем в регистрах
  //****************************************************************************
  return;
}

void DIN_Init(uint16_t pin, GPIO_TypeDef* port)
{
  GPIO_InitTypeDef      GPIO_InitStruct;                // Структура инициализирующих данных GPIO
  //********************************RCC*****************************************
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
  //********************************GPIO****************************************
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // Макс скорость
  GPIO_InitStruct.GPIO_Pin   = pin;                     // Иницилизируем системные пины
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;           // Режим входа
  GPIO_Init(port, &GPIO_InitStruct);                    // Заданные настройки сохраняем в регистрах  
  //****************************************************************************
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************