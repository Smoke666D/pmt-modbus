//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "KMG18.h"
#include "SPI.h"
#include "DIO.h"
#include "PWM.h"
#include "system.h"
#include "Register.h"
//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static uint8_t   DataReady;
static uint16_t  data[2];
static uint16_t  sw = 0;
//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************

//******************************************************************************
//*******************************USER CODE**************************************
//******************************************************************************
void KMG18_Init(void)
{
  PWM_Inin(LED_BR_pin,LED_BR_port,LED_BR_tim,LED_BR_ch);
  LED_SET(ReadHolding(BrAdr));
  DIN_Init(KL_pins,KL_port);
  DOUT_Init(nG_pin,nG_port);
  DOUT_Init(SEG_pins,SEG_port);
  SPI_init(0);
  return;
}

void KMG18_FSA(void)
{
  uint16_t buf = 0;
  uint8_t  ModifyFlag = GetHoldingModifyFlag();  // Читаем флаг записи в регистры
  
  if (ModifyFlag)                               // Если были изменения...
  {
    ModifyFlag--;               // Для обработки к модифити флагу прибавляют еденицу, вычитаем ее.
    ResetHoldingModifyFlag();   // Сбрасываем флаг модификации регистров
    switch(ModifyFlag)          // Перебираем адресса в памяти
    {
      case BarAdr:
        buf = ReadHolding(BarAdr);
        data[1]   = buf & 0x00FF;
        data[0]   = (buf & 0xFF00) >> 8;
        DataReady = 1;
        break;
      case LedAdr:
        buf = ReadHolding(LedAdr);
        if (buf & SEG_R_MSK)
        {
          SEG_R_SET;
        }
        else
        {
          SEG_R_RESET;
        }
        if (buf & SEG_G_MSK)
        {
          SEG_G_SET;
        }
        else
        {
          SEG_G_RESET;
        }
        break;
      case SwAdr:
        break;
      case BrAdr:
        LED_SET(ReadHolding(BrAdr));
        break;
    }
  }
  
  if(DataReady && SPI_IsTxEnd())
  {
    SPI_Send(data, 2);
    DataReady = 0;
  }
  
  if (KL1_READ)
  {
    buf |= 0x0001;
  }
  else
  {
    buf &= ~0x0001;
  }
  
if (KL2_READ)
  {
    buf |= 0x0002;
  }
  else
  {
    buf &= ~0x0002;
  }  
    
  if (KL3_READ)
  {
    buf |= 0x0004;
  }
  else
  {
    buf &= ~0x0004;
  }  
    
  if (KL4_READ)
  {
    buf |= 0x0008;
  }
  else
  {
    buf &= ~0x0008;
  }
  
  if (buf != sw)
  {
    sw = buf;
    WriteShortToHolding(SwAdr,((~sw)&0x000F));
  }
  
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************