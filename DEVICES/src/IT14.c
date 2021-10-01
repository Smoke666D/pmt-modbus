//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "IT14.h"
#include "SPI.h"
#include "ADC.h"
#include "PWM.h"
#include "DIO.h"
#include "system.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "Register.h"
//******************************************************************************
//******************************************************************************
//******************************************************************************
#if (device == IT14)
//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static uint8_t          NewData     = 0;                // Цифры на запись
static uint16_t         IndiData[DigNum];               // Данные на поссылку
static uint8_t          NewBr       = 0;                // Флаг новых данных
static uint8_t          CurRedBr    = RedBrDef;         // Текущая яркость красного
static uint8_t          CurGreenBr  = GreenBrDef;       // Текущая яркость зеленого
static uint8_t          CurYellowBr = YellowBrDef;      // Текущая яркость желтого
static uint8_t          NewColor    = 0;                // Флаг нового цвета
static uint8_t          CurCollor   = 0;                // Текущий цвет
//******************************************************************************
//*********************************CONST****************************************
//******************************************************************************

  const uint16_t DigAdr[DigNum] = {DIG0,DIG1,DIG2,DIG3,DIG4};

//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************
void CheckNewData(uint8_t dig);
void SetBr(uint8_t rbr, uint8_t gbr);
void IT14_UpdateData(void);
//******************************************************************************
//******************************************************************************
//******************************************************************************
void IT14_UpdateData(void)
{  
  SPI_Send(IndiData,DigNum);
  return;
}


void SetBr(uint8_t rbr, uint8_t gbr)
{
  uint8_t  i;
  float red   = 1;
  float green = 1;
  
  for (i=0;i<rbr;i++)
  {
    if(gbr == 0)
    {
      red = red * RedBrK;
    }
    else
    {
      red = red * RedBrYK;
    }
  }
  for (i=0;i<gbr;i++)
  {
    green = green * GrBrK;
  }
  SetPWM(TIM3, 3, (uint16_t)(red));         // Red
  SetPWM(TIM3, 4, (uint16_t)(green));       // Green
  return;
}

void CheckNewData(uint8_t dig)
{
  uint16_t data = ReadHolding(dig);
  if (IndiData[dig] != (data & DigMask))
  {
    IndiData[dig] = data & DigMask;
    NewData       = 1;
  }
  if (CurCollor != ((data & ColorMask) >> 8))
  {
    CurCollor = (data & ColorMask) >> 8;
    NewColor  = 1;
  }
  return;
}
//******************************************************************************
//*******************************INIT*******************************************
//******************************************************************************
void IT14_Init(void)
{
  uint8_t i;
  
  for(i=0;i<DigNum;i++)
  {
    IndiData[i]=0x0000;
  }
  WriteShortToHolding(BR_R,RedBrDef);
  WriteShortToHolding(BR_G,GreenBrDef);
  WriteShortToHolding(BR_Y,YellowBrDef);
  //RCC->APB2ENR |= RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPBEN;        // Запускаем тактирование GPIOА и GPIOB
  //RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;                           // Запускаем тактирование TIM3
  
  SPI_init(0);
  #if (ADC_OnOff)
    ADC_init();
  #endif
  DOUT_Init(nOE_pin,nOE_Port);
  PWM_Inin(SEG_R_pin,SEG_port,TIM3,3);
  PWM_Inin(SEG_G_pin,SEG_port,TIM3,4);
  nOE_RESET;
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
void IT14_FSA(void)
{
  uint8_t i;
  uint16_t data = 0;
  uint8_t ModifyFlag = GetHoldingModifyFlag();  // Получам флаг изменения данных в регистрах
  //****************************************************************************
  if (ModifyFlag)
  {
    ModifyFlag--;
    switch(ModifyFlag)
    {
      //************************************************************************
      case DIG0:
        CheckNewData(DIG0);
        break;
      case DIG1:
        CheckNewData(DIG1);
        break;
      case DIG2:
        CheckNewData(DIG2);
        break;
      case DIG3:
        CheckNewData(DIG3);
        break;
      case DIG4:
        CheckNewData(DIG4);
        break;
      //************************************************************************
      case BR_R:
        CurRedBr = ReadHolding(BR_R) & BrMask;          // Читаем яркость красную
        NewBr    = 1;                                   // Выставляем флаг обновления яркости
        break;
      case BR_G:
        CurGreenBr = ReadHolding(BR_G) & BrMask;        // Читаем яркость зеленую
        NewBr      = 1;                                 // Выставляем флаг обновления яркости
        break;
      case BR_Y:
        CurYellowBr = ReadHolding(BR_Y) & BrMask;       // Читаем яркость оранжевую
        NewBr       = 1;                                // Выставляем флаг обновления яркости
        break;
      //************************************************************************
    }
    ResetHoldingModifyFlag();
  }
  //****************************************************************************
  else 
  {
    if (NewData && SPI_IsTxEnd())
    {
      SPI_Send(IndiData,DigNum);
      NewData = 0;
    }
    else if(NewColor|NewBr)                     // Если надо менять цвет или яркость
    {                                           // Цвет меняеться с помощью яркости
      switch(CurCollor)
      {
        case GreenCode:                         // Зеленый цвет
          SetBr(0,CurGreenBr);                  // Загружаем зеленую яркость
          break;
        case RedCode:                           // Красный цвет
          SetBr(CurRedBr,0);                    // Загружаем красную яркость
          break;
        case YellowCode:                        // Желтый цвет
          SetBr(CurYellowBr,CurYellowBr);       // Загружаем желтую яркость
          break;
      }
      NewBr = 0;
      if (NewColor)
      {
        for(i=0;i<DigNum;i++)
        {
          data = (ReadHolding(DigAdr[i])&DigMask)|(CurCollor<<8);
          WriteShortToHolding(DigAdr[i], data);
        }
      }
      NewColor = 0;
    }
  }
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
#endif