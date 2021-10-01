//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "BIG.h"
#include "system.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "Register.h"
//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static uint16_t         TempDataInd = 0;
static uint8_t          PointerCounter = 0;
//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************
void OutInd(uint16_t DaitaInd);
//******************************************************************************
//*******************************USER CODE**************************************
//******************************************************************************
void OutInd(uint16_t DaitaInd)
{ 
  uint16_t      temp = 0;  
  uint8_t       c = 0;
  float         G = 1;
  float         R = 1; 
  
  temp = DaitaInd & 0x0300;
  GPIOA->BRR  = 0xff;
  GPIOA->BSRR = DaitaInd & 0x00ff;
  TIM3->CCR3  = 0;
  TIM3->CCR4  = 0;
  if (temp == 0x0100)
  {  
    temp = ReadHolding(BRIGTHNESS_1) & 0x000F;
    if (temp)
    {
      for (c=1;c<=temp;c++)
      {
        G = G*1.64;
      }
    } 
    TIM3->CCR4= (uint16_t) G; // 23906;  //
  } 
  if (temp == 0x0200)
  {  
    temp = ReadHolding(BRIGTHNESS_2) & 0x000F;
    if (temp)
    {
      for (c=1;c<=temp;c++)
      {
        R = R*1.55;
      }
    } 
    TIM3->CCR3= (uint16_t) R; // 23906; //
  }
   if (temp == 0x0300)
   {
    temp = ReadHolding(BRIGTHNESS_3) & 0x000F;
    if (temp)
    {
      for (c=1;c<=temp;c++)
      {
        G = G*1.64;
        R = R*1.55;
      }
    } 
    TIM3->CCR4=(uint16_t)G;
    TIM3->CCR3=(uint16_t)R;
  
  }; //23906, 9567   
  return;
}

void BIG_Process()   
{
  if (GetHoldingModifyFlag())
  {                   
    if (GetHoldingModifyFlag() == 2)
    {
      if ( ReadHolding(1) < RECODING_TIBLE_SIZE )                
      {
        WriteShortToHolding(0, ReadHolding( ReadHolding(1) + USER_TABLE ));  
      }
    }  
    TempDataInd = ReadHolding(0) & 0x3FF;           
    ResetHoldingModifyFlag();
    OutInd(TempDataInd); 
  }             
  return;
 }


void TIM3_IRQHandler(void)
{   
  if (TempDataInd & 0x0080) 
  {
    __disable_irq ();    
    switch (++PointerCounter) 
    {
      case 3:
        GPIOA->BRR =  0x80; 
        break;
      case 7:
        GPIOA->BSRR = 0x80;
        PointerCounter = 0;
        break;
    }     
    __enable_irq (); 
  }
  TIM3->SR = 0;
  return; 
}
//******************************************************************************
//******************************************************************************
//******************************************************************************