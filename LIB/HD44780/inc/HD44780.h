#ifndef __HD44780_H
#define __HD44780_H

#include "stm32f10x.h"

//***************************Define*********************************************

//*****************************HD***********************************************
#define         SWTimout                2               // 
#define         LedBright               50              // %

#define         HD_BUFFER_SIZE	        0x32
#define         HD_BUFFER_DATA_MSK      0x00FF
#define         HD_BUFFER_RS_MSK        0x0100

#define         HD_LATCH_TIMEOUT        1
//**************************HD_USER*********************************************
#define         HD_USER_START           0x01    // Флаг начала записи 
#define         HD_USER_BUSY            0x02    // флаг занятости канала
#define         HD_USER_DATA            0x04    // флаг отправленных данных
#define         HD_USER_LATCH           0x08    // флаг выставления защелки
//************************LCD DISPALY*******************************************
#define         LCD_AutoScroll          0       // Автоскролл
#define         LCD_ScrollLeftToRight   0       // Скролить с лева на право
#define         LCD_Cursor              0       // Отображать курсор
#define         LCD_Blink               0       // Мигать курсором
#define         LCD_8bit                1       // Шина 8 бит или 4 бита
#define         LCD_2lines              1       // Дисплей 2 динии или 1 линию
#define         LCD_5x10                0       // 
#if (!LCD_5x10)
  #define       LCD_5x8                 1
#else
  #define       LCD_5x8                 0
#endif
#define         LSD_LEN                 16      // Длинна дисплея
#define         LCD_InitLen             5       // Длина последовательности для и нициализации дисплея
//**************************PINOUT**********************************************
#define         Data_port       GPIOA
#define         Sys_port        GPIOB           

#define         RnW_pin         GPIO_Pin_0      //B
#define         RS_pin          GPIO_Pin_1      //B
#define         E_pin           GPIO_Pin_2      //B
#define         Sys_pins        (RnW_pin|RS_pin|E_pin)

#define         D0_pin          GPIO_Pin_0      //A
#define         D1_pin          GPIO_Pin_1      //A
#define         D2_pin          GPIO_Pin_2      //A
#define         D3_pin          GPIO_Pin_3      //A
#define         D4_pin          GPIO_Pin_4      //A
#define         D5_pin          GPIO_Pin_5      //A
#define         D6_pin          GPIO_Pin_6      //A
#define         D7_pin          GPIO_Pin_7      //A
#define         Data_pins       (D0_pin|D1_pin|D2_pin|D3_pin|D4_pin|D5_pin|D6_pin|D7_pin)


#define         LedOn_pin       GPIO_Pin_10
#define         LedOn_port      GPIOB
#define         LedOn_tim       TIM2
#define         LedOn_ch        3

#define         V0_pin          GPIO_Pin_15
#define         V0_port         GPIOA
#define         V0_tim          TIM2
#define         V0_ch           1
//***************************MACROS*********************************************
#define         RS_set          DOUT_SET(Sys_port, RS_pin)
#define         RS_reset        DOUT_RESET(Sys_port, RS_pin)
#define         RnW_set         DOUT_SET(Sys_port, RnW_pin)
#define         RnW_reset       DOUT_RESET(Sys_port, RnW_pin)
#define         E_set           DOUT_SET(Sys_port, E_pin)
#define         E_reset         DOUT_RESET(Sys_port, E_pin)
#define         E_invert        DOUT_INVERT(Sys_port, E_pin)

#define         WriteByte(data) ((Data_port->ODR)&Data_pins)|((uint16_t)(data))

#define         Busy_Read       DIN_READ(Data_port,D7_pin)

#define         LedSet(data)    SetPWM(LedOn_tim,LedOn_ch,data)
#define         V0Set(data)     SetPWM(V0_tim,V0_ch,data)
//******************************************************************************
#define         LEDbr           100 //%
//*********************Команды*********************

//RS = 0; R/W = 0

#define         HD_CMD_CD               0x01    // Clear display Очистка дисплея (00)

#define         HD_CMD_RH               0x02    // Return home Возврат коретки (00)

#define         HD_CMD_MS               0x04    // Entry Mode Set
#define         HD_CMD_MS_LR            0x02    // 1 - Left -> Right
#define         HD_CMD_MS_AS            0x01    // 1 - Auto Scroll


#define         HD_CMD_DOC              0x08    // Display on/off control
#define         HD_CMD_DOC_DO           0x04    // Display On
#define         HD_CMD_DOC_CO           0x02    // Cursor On
#define         HD_CMD_DOC_BO           0x01    // Blink On

#define         HD_CMD_CDS              0x10    // Cursor or display shift
#define         HD_CMD_CDS_DS           0x08    // Display shift
#define         HD_CMD_CDS_RS           0x04    // Right shift

#define         HD_CMD_FS               0x20    // Function Set
#define         HD_CMD_FS_8BDL          0x10    // Data length=1 - 8 bit, 0 - 4 bit
#define         HD_CMD_FS_N             0x08    // Number of display lines + 1
#define         HD_CMD_FS_F             0x04    // F=1 = 5x10, F=0 5x8

#define         HD_CMD_SCGRAMA          0x40    // Set CGRAM address
#define         HD_CMD_CGRAM_MSK        0x3F

#define         HD_CMD_SDDRAMA          0x40    // Set DDRAM address
#define         HD_CMD_SDDRAMA_MSK      0x7F



#define         DisOnOffCont            0x08 

#define         DisShiftMask            0x08    // 1 - перемещения дисплея на 1 яч, 0 - курсора
#define         RLShiftMask             0x04    // 1- вправо, 0 - влево

#define         ShiftTypeOnOff          0x10    // Сдвиги 

#define         Bit8Mask                0x10    // 1 - 8 бит, 0 - 4 бит
#define         Line2Mask               0x08    // 0 - 1 строка, 1 - 2 строки
#define         pic57Mask               0x04    // 0 - 5х7, 1 - 5x10 

#define         FunSet                  0x20

#define         SetCGadress             0x04
#define         SetDDadress             0x08

//RS = 0; R/W = 1 - Read Busy Flug & Adress
//RS = 1; R/W = 0 - Wright data to RAM
//RS = 1; R/W = 1 - Read data from RAM
//****************************Function*****************************************
uint8_t HD_GetBusy(void);                               // Проверить занят ли порт
void    HD_Write(uint16_t* data,uint16_t len);          // Запись массива данных/команд
void    HD_Proc(void);                                  // Положиме меня в прерывание
void    HD_init(void);                                  // Инициализация индикатора
void    HD_SetBackLight(uint16_t br);                   // Контроль задней подсветки
void    HD_DispShift(uint16_t step, uint8_t RnL);       // Сдвиг каретки 
void    HD_CleanLCD(void);                              // Очистка дисплея
void    HD_RetHome(void);                               // Переместить каретку в ноль
void    HD_MoveCur(uint8_t x);                          // Переместить каретку в координату x
void    HD_WriteString(char* str);                      // Вывести на экран строчку
void    HD_DEMO(void);                                  // Hello World
void    HD_Send_Float(float fl);
//*****************************************************************************

#endif /*__HD44780_H */