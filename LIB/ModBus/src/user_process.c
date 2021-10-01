//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "user_process.h"
#include "system.h"
#include "init.h"

#if (device == BIC)
  #include "BIC.h"
#endif
#if (device == BKC)
  #include "BKC.h"
#endif
#if (device == BSS)
  #include "BSS.h"
#endif
#if (device == BIG)
  #include "BIG.h"
#endif
#if (device == IT14)
  #include "IT14.h"
#endif
#if (device == TSP_SH)
  #include "TSP_SH.h"
#endif
#if (device == BKC14)
  #include "BKC14.h"
#endif
#if (device == KMG18)
  #include "KMG18.h"
#endif
#if (device == BKC01)
  #include "BKC01.h"
#endif

//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static UCHAR   CurrentSlaveAdrees = 0x01;
static uint8_t SW_COUNT = 0;
//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************
void TIM3_IRQHandler(void);
void TIM2_IRQHandler(void);
//******************************************************************************
//******************************SWCH/ADR****************************************
//******************************************************************************
uint8_t GetSwitsh(void)
{
  uint32_t temp = 0,mask,data;
  
  #if ( DSW_ENB > 0 )
    #if ( DSW_SHORT == 0 )
      temp = GPIOB->IDR;
      temp &= 0x3f8;
      temp = temp >>2;
      if (GPIOA->IDR & GPIO_IDR_IDR15)
      {
        temp |= 0x01;
      }
      else
      {
        temp &= 0xfffffffe;
      }
      temp = ~temp;
      for (int i=0;i<4;i++)
      {  
        if (temp & (0x80>>i)) temp1 |= 0x01<<i;
        if (temp & (0x01<<i)) temp1 |= 0x80>>i;
      }
    #else
      temp = ( ( ( ~GPIOB->IDR ) & ( DSW4 | DSW3 | DSW2 | DSW1 ) ) >> 3 );
    #endif
  #else
    temp = 0x01;
  #endif
 return (uint8_t)temp;  
}

UCHAR GetCurSlaveAdr(void)
{  
  return CurrentSlaveAdrees;
}

//******************************************************************************
//******************************************************************************
//******************************************************************************

void TIM2_IRQHandler(void)
{
  WatchDog_reset();   
  if(TIM2->SR & TIM_SR_UIF)
  {
    __disable_irq ();        
    
    if(SW_COUNT++ == 5)
    {
      CurrentSlaveAdrees = GetSwitsh(); 
      SW_COUNT = 0;
    }
    
    #if (device == BIC)
      BIC_FSA();
    #endif  
    
    #if (device == BIG)
      BIG_Process();
    #endif   
    
    #if (device == BKC)
      BKC_FSA();
    #endif   
    
    #if (device == IT14)
      IT14_FSA();
    #endif  
      
    #if (device == TSP_SH)
      TSP_SH_FSA();
    #endif  
      
    #if (device == BKC14)
      BKC14_FSA();
    #endif

    #if (device == KMG18)
      KMG18_FSA();
    #endif  
      
    #if (device == BKC01)
      BKC01_FSA();
    #endif
    
    __enable_irq (); 
  }  
  TIM2->SR = 0;         
  return; 
}