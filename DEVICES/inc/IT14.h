#ifndef _IT14_H
#define _IT14_H

//******************************FUNCTIONS***************************************
void IT14_Init(void);
void IT14_FSA(void);
//*********************************GPIO*****************************************
#define         SEG_R_pin       GPIO_Pin_0      // TIM3_CH3
#define         SEG_G_pin       GPIO_Pin_1      // TIM3_CH4
#define         SEG_port        GPIOA

#define         nOE_pin         GPIO_Pin_3
#define         nOE_Port        GPIOA
//***************************MACROS********************************************
#define         nOE_SET         DOUT_SET(nOE_Port,nOE_pin)
#define         nOE_RESET      	DOUT_RESET(nOE_Port,nOE_pin)
//******************************Device******************************************
#define         ADC_OnOff       0

#define         DigNum          5
#define         MaxBr           0x7FFF
#define         DigMask         0x00FF
#define         ColorMask       0x0300
#define         BrMask          0x000F
#define         RedBrK          2
#define         RedBrYK         1.64
#define         GrBrK           2

#define         GreenCode       0x1
#define         RedCode         0x2
#define         YellowCode      0x3

#define         RedBrDef        0x000F
#define         GreenBrDef      0x000F
#define         YellowBrDef     0x000F
//******************************************************************************
//*************************Mod Bus Registers************************************
//******************************************************************************
// DIG0..4:
// 7 6 5 4 3 2 1 0 | 7 6 5 4 3 2 1 0
// n n n n n n L L | A B C D E F G DP
// LL - RG
// 0x1 - Green
// 0x2 - Red
// 0x3 - Yellow
//*******************************RAM********************************************
#define         DIG0            0x00    
#define         DIG1            0x01
#define         DIG2            0x02
#define         DIG3            0x03
#define         DIG4            0x04

#define         BR_G            0x05    // 0..F
#define         BR_R            0x06    // 0..F
#define         BR_Y            0x07    // 0..F
//*******************************ROM********************************************


//******************************************************************************
//******************************************************************************
//******************************************************************************

#endif
