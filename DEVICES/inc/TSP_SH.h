#ifndef _TSP_SH_H
#define _TSP_SH_H
//******************************FUNCTIONS***************************************
void TSP_SH_Init(void);
void TSP_SH_FSA(void);
//*********************************GPIO*****************************************
#define         SW1_pin         GPIO_Pin_10     // DIN
#define         SW2_pin         GPIO_Pin_11     // DIN
#define         SW_port         GPIOB

#define         BR_SW_pin       GPIO_Pin_0      // TIM3_CH3
#define         BR_SW_port      GPIOA

#define         nOE_pin         GPIO_Pin_3      // DOUT
#define         nOE_Port        GPIOA
//***************************MACROS********************************************
#define         nOE_SET         DOUT_SET(nOE_Port,nOE_pin)
#define         nOE_RESET      	DOUT_RESET(nOE_Port,nOE_pin)
#define         SW1_READ        DIN_READ(SW_port,SW1_pin)
#define         SW2_READ        DIN_READ(SW_port,SW2_pin)
//******************************Device******************************************
#define         ADC_OnOff       0

#define         CR_NewData      0x01    // Маска новых данных на запись
#define         CR_NewBr        0x02    // Маска новой яркости
#define         CR_SW1          0x04    // Маска состояния нулевого свича        
#define         CR_SW2          0x08    // Маска состояния первого свича        
#define         CR_SW_CH        0x10    // Маска необходимости перезаписи регистра кнопок

#define         DigNum          7
#define         MaxBr           0x7FFF
#define         BrDef           0x0FFF

#define         SW1mask         0x0001
#define         SW2mask         0x0002
//******************************************************************************
//*************************Mod Bus Registers************************************
//******************************************************************************

//*******************************RAM********************************************
#define         DIG01           0x0000
#define         DIG23           0x0001
#define         DIG45           0x0002
#define         DIG6            0x0003

#define         SW01            0x0004

#define         BR              0x0005
//*******************************ROM********************************************


//******************************************************************************
//******************************************************************************
//******************************************************************************

#endif
