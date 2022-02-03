#ifndef _BKC01_H
#define _BKC01_H
//*******************************INCLUDE****************************************
#include "DIO.h"
//******************************FUNCTIONS***************************************
void BKC01_Init(void);          // Инициализация переферии
void BKC01_FunInit(void);       // Инициализация, требующая работы прерываний
void BKC01_FSA(void);           // Конечный автомат
//*******************************GPIO*******************************************
#define         KL1_pin         GPIO_Pin_15
#define         KL1_port        GPIOB

#define         KL2_pin         GPIO_Pin_14
#define         KL2_port        GPIOB

#define         KL3_pin         GPIO_Pin_11
#define         KL3_port        GPIOA

#define         KL4_pin         GPIO_Pin_13
#define         KL4_port        GPIOB

#define         KL5_pin         GPIO_Pin_12
#define         KL5_port        GPIOB

#define         KEY_pin         GPIO_Pin_13
#define         KEY_port        GPIOC

#define         ZUM_pin         GPIO_Pin_11
#define         ZUM_port        GPIOB
#define         ZUM_ch          4               // Remap
//**********LCD back light************
#define         LED_pin         GPIO_Pin_10
#define         LED_port        GPIOB
#define         LED_ch          3               // Remap
//************LCD contrast************
#define         VO_pin          GPIO_Pin_15     
#define         VO_port         GPIOA
#define         VO_ch           1               // Remap

#define         PWM_tim         TIM2
//******************************MACROS******************************************
#define         KL1_READ        DIN_READ(KL1_port,KL1_pin)
#define         KL2_READ        DIN_READ(KL2_port,KL2_pin)
#define         KL3_READ        DIN_READ(KL3_port,KL3_pin)
#define         KL4_READ        DIN_READ(KL4_port,KL4_pin)
#define         KL5_READ        DIN_READ(KL5_port,KL5_pin)
#define         KEY_READ        DIN_READ(KEY_port,KEY_pin)
//******************************SysReg******************************************
#define         SysRegBlink             0x0001          // Флаг под мигание
#define         SysRegChange            0x0002          // Изменнение регистра
#define         SysRegSwDone            0x0004          // Команда замолкнуть
#define         SysRegBIPx1             0x0008          // Команда пикнуть один раз
#define         SysRegBIPx2             0x0010          // Команда пикнуть два раз
#define         SysRegBIPx3             0x0020          // Команда пикнуть три раз
#define         SysRegLed               0x0040          // Флаг начала горения светодиодов
#define         SysRegKey               0x0080          // Флаг срабатывания ключа
#define         SysRegBlinkStat         0x0100          // Флаг состояния мигания
//******************************Device******************************************
// TIM4 - 10000 hz => 0.1 ms
#define         BKC_TIMEOUT             100
#define         ZUMMER_freq             50
#define         KeyDownTimout           40      // 40 ms Таймаут нажатия кнопки для отсекания дребезг контактов
#define         KeyHoldTimeout          5000    // 500 ms Таймаут удержания кнопки для нового функцианала
#define         LED_TIMEOUT             1200000 // 2 мин Таймаут гашения подсветки дисплея
#define         WaitChTimeout           100000  // 10 с   сброс значения без нажатия ентер

#define         KeyHoldShortTimeout     1000    // 250 мс первые 3 значения
#define         KeyHoldLongTimeout      2500    // 100 мс последующие
#define         ShortToLongCount        3       // Кол-во медленных действий, за которым следуют быстрые

#define         BIP_TIMEOUT             1000    // 1 s Период гудка
#define         LCD_TIMEOUT             12000   // Период мигания дисплея

#define         SW_msk                  0x0F
#define         SW_Hold                 0x10

#define         SetPointMIN             1E-8
#define         SetPointMAX             107
#define         SetPointDef             SetPointMIN

#define         SetPointE_Nom           72
#define         SumUpSetPointNom        (SetPointE_Nom + SetPointMAX)
#define         SetPointPosDef          0
//******************************************************************************
//*************************Mod Bus Registers************************************
//******************************************************************************

//*******************************RAM********************************************


//*******************************ROM********************************************
#define         SetPoint0Adr            0x00
#define         SetPoint1Adr            0x01
#define         LCD_ContrastAdr         0x02
#define         LCD_BrightAdr           0x03
//******************************************************************************
//******************************************************************************
//******************************************************************************
#endif