#ifndef __init_H
#define __init_H
//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "user_process.h"
//***************************Define*********************************************


//*********************TIM1*****************

//#define         TIM1ReloadFreq    240000     // Hz

//**************************PINOUT**********************************************
#define         RS485_DE_pin    GPIO_Pin_8
#define         RS485_TX_pin    GPIO_Pin_9
#define         RS485_RX_pin    GPIO_Pin_10
#define         RS485_port      GPIOA
//***************************MACROS*********************************************
#define         RS485_nRE       DOUT_RESET(RS485_port,RS485_DE_pin)
#define         RS485_DE        DOUT_SET(RS485_port,RS485_DE_pin)
//********************7-seg****************
#define         INDPort         GPIOA           // Порт для семесигментника
#define         IND_A           GPIO_Pin_0
#define         IND_B           GPIO_Pin_1
#define         IND_C           GPIO_Pin_2
#define         IND_D           GPIO_Pin_3
#define         IND_E           GPIO_Pin_4
#define         IND_F           GPIO_Pin_5
#define         IND_G           GPIO_Pin_6
#define         IND_DP          GPIO_Pin_7

#define         SEGPort         GPIOB           // Порт общих
#define         SEG1            GPIO_Pin_0      // Общий первого сегмента
#define         SEG2            GPIO_Pin_1      // Общий второго сегмента
//*******************DIP_SWITCH************
#if (device == BKC01)
  #define       DSW_ENB         1               // Существование дип свитча
  #define       DSW_SHORT       1  
#else 
  #define       DSW_ENB         1               // Существование дип свитча
  #define       DSW_SHORT       0
#endif

#if ( DSW_SHORT == 0 )
  #define         DSW8            GPIO_Pin_15     // A
  #define         DSW7            GPIO_Pin_3      // B
  #define         DSW6            GPIO_Pin_4      // B
  #define         DSW5            GPIO_Pin_5      // B
  #define         DSW4            GPIO_Pin_6      // B
  #define         DSW3            GPIO_Pin_7      // B
  #define         DSW2            GPIO_Pin_8      // B
  #define         DSW1            GPIO_Pin_9      // B
#else 
  #define         DSW4            GPIO_Pin_6      // B
  #define         DSW3            GPIO_Pin_5      // B
  #define         DSW2            GPIO_Pin_4      // B
  #define         DSW1            GPIO_Pin_3      // B
#endif
//******************
#define         varClrBitBB(var, BitNumber)     (*(vu32 *)\(SRAM_BB_BASE | ((((u32)&var) - SRAM_BASE) << 5) | ((BitNumber) << 2)) = 0)
#define         varSetBitBB(var, BitNumber)     (*(vu32 *)\(SRAM_BB_BASE | ((((u32)&var) - SRAM_BASE) << 5) | ((BitNumber) << 2)) = 1)
#define         varGetBitBB(var, BitNumber)     (*(vu32 *)\(SRAM_BB_BASE | ((((u32)&var) - SRAM_BASE) << 5) | ((BitNumber) << 2)))

#define         FLAG0           0 // Флаг 0.
#define         FLAG1           1 // Флаг 1.
//   u32 Flags=(1<<FLAG1); // Битовое поле.

//***************************DEVICE ON/OFF*************************************

//"1" - ON
//"0" - OFF

#define         WatchDog_OnOff          0  

//****************************Function*****************************************
//uint32_t HSE_Status_q;
void Init(void);       
void gpio_init(void);
void TIM1_init(void);
void TIM3_init(void);
void TIM2_init(void);
void TIM4_init(void);

unsigned long GetPeriodT(void);
#endif /*__init_H */