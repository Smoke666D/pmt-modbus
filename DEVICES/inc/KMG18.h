#ifndef _KMG18_H
#define _KMG18_H


//******************************FUNCTIONS***************************************
void KMG18_Init(void);
void KMG18_FSA(void);
//*********************************GPIO*****************************************
#define         SEG_R_pin       GPIO_Pin_2      // DIO
#define         SEG_G_pin       GPIO_Pin_1      // DIO
#define         SEG_pins        (SEG_G_pin|SEG_R_pin)
#define         SEG_port        GPIOB

#define         LED_BR_pin      GPIO_Pin_0      // PWM
#define         LED_BR_port     GPIOB
#define         LED_BR_tim      TIM3
#define         LED_BR_ch       3

#define         nG_pin          GPIO_Pin_3
#define         nG_port         GPIOA

#define         KL1_pin         GPIO_Pin_10
#define         KL2_pin         GPIO_Pin_11
#define         KL3_pin         GPIO_Pin_12
#define         KL4_pin         GPIO_Pin_13
#define         KL_pins         (KL1_pin|KL2_pin|KL3_pin|KL4_pin)
#define         KL_port         GPIOB
//***************************MACROS********************************************
#define         KL1_READ        DIN_READ(KL_port,KL1_pin)
#define         KL2_READ        DIN_READ(KL_port,KL2_pin)
#define         KL3_READ        DIN_READ(KL_port,KL3_pin)
#define         KL4_READ        DIN_READ(KL_port,KL4_pin)
#define         SEG_R_SET       DOUT_SET(SEG_port,SEG_R_pin)
#define         SEG_R_RESET     DOUT_RESET(SEG_port,SEG_R_pin)
#define         SEG_G_SET       DOUT_SET(SEG_port,SEG_G_pin)
#define         SEG_G_RESET     DOUT_RESET(SEG_port,SEG_G_pin)
#define         LED_SET(data)   SetPWM(LED_BR_tim,LED_BR_ch,data)
//******************************Device******************************************
#define         LED_BR          0xFFFF
#define         SEG_R_MSK       0x01
#define         SEG_G_MSK       0x02
//******************************************************************************
//*************************Mod Bus Registers************************************
//******************************************************************************

//*******************************RAM********************************************
#define         BarAdr                  0x00    // 
#define         LedAdr                  0x01    // 
#define         SwAdr                   0x02
#define         BrAdr                   0x03    // 

#endif