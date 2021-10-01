#ifndef _BIC_H
#define _BIC_H


//******************************FUNCTIONS***************************************
void BIC_Init(void);
void BIC_FSA(void);
//*********************************GPIO*****************************************
#define         D7_pin          GPIO_Pin_7
#define         D6_pin          GPIO_Pin_6
#define         D5_pin          GPIO_Pin_5
#define         D4_pin          GPIO_Pin_4
#define         D3_pin          GPIO_Pin_3
#define         D2_pin          GPIO_Pin_2
#define         D1_pin          GPIO_Pin_1
#define         D0_pin          GPIO_Pin_0
#define         TX2_pin         GPIO_Pin_2
#define         USART2_port     GPIOA

#define         nCS_pin         GPIO_Pin_0
#define         nWR_pin         GPIO_Pin_1
#define         BUSY_pin        GPIO_Pin_2
#define         ControlPort     GPIOB
//*********************************MACROS***************************************
#define         BUSYread        (ControlPort->IDR & BUSY_pin)
#define         nWRset          (ControlPort->BSRR |= nWR_pin)
#define         nWRreset        (ControlPort->BSRR |= (nWR_pin << 16))
#define         nCSset          (ControlPort->BSRR |= nCS_pin)
#define         nCSreset        (ControlPort->BSRR |= (nCS_pin << 16))
//******************************************************************************






//******************************************************************************
//****************************CU209-TW202A**************************************
//******************************************************************************

//*******************************CONST******************************************
#define         InitSizeStart           7               // Окончание первых команд и начало пользовательских символов             
#define         InitSizeSeq             135             // Кол-во команд для инициализации
#define         DisplaySize             20              // Кол-во символов на дисплее
#define         LuminValeDef            0xff            // 100%
#define         ComandList              (DisplaySize + 4)
//****************************PCB JUMPERS***************************************
#define         JUMP0           0
#define         JUMP1           0
#define         JUMP2           0
#define         JUMP3           0
#define         JUMP4           0
//*************************RS232 parametrs**************************************
#define         DispBaundJump           (uint8_t)(JUMP0 +  JUMP1*2 +  JUMP2*4)  
#define         DispBaund               DispBaunds[DispBaundJump]

#define         DispParityJump          (uint8_t)(JUMP3 + JUMP4*2)
#define         DispParity              DispParitys[DispParityJump]
//********************Software comands (1 byte)*********************************
#define         BackSpaceCmd                    0x08    // The cursor moves one char to the left
#define         HorizontalTabCmd                0x09    // The cursor moves one char to the right
#define         LineFeedCmd                     0x0A    // All displayed char are cleared. The cursor doesn't move
#define         FormFeedCmd                     0x0C    // The cursor moves to the left end
#define         CarRetCmd                       0x0D    // The cursor moves to the left end
#define         ClrCmd                          0x0F    // All displayed char are cleared. The cursor doesn't move
#define         AutoCarRetModeCmd               0x11    // DC1
#define         OverWriteModeCmd                0x12    // DC2
#define         HorizontalScrollModeCmd         0x13    // DC3
#define         CurIsInvisibleCmd               0x14    // или 16 или 17 (варианты в pdf)
#define         CurIsBlinkCharCmd               0x15    //
#define         InternationalFontCmd            0x18    // Стандартная раскладка
#define         KatakanaFontCmd                 0x19    // Смена раскладки
#define         EuroCurrencyMarkCmd             0x1A    // По адрессу 0xAD в CT0 раскладке появляеться значок евро
#define         EscCmd                          0x1B    // Используеться для следующих команд:
//********************Software comands (3 byte)*********************************
#define         SendUserFontCmd                 0x43    // Запись пользовательского символа
#define         PositionCurCmd                  0x48    // Переместить курсор свободно
#define         SoftwareRstCmd                  0x49    // Сброс всех настроек
#define         LuminanceCmd                    0x4C    // Регулеровка яркости дисплея
#define         FlickerlessWriteCmd             0x53    // Установка скоростного режима
#define         CursorBlinkSpeedCmd             0x54    // 

//EXAMPLE OF MASSAGE:    ESC + Cmd + Data
//******************************************************************************






//******************************************************************************
//*******************************USER FONTS*************************************
//******************************************************************************
#define         UserFontMas     5
#define         UserFontsMax    16


#define         UserFont0adr    0x00
#define         UserFont1adr    0x01
#define         UserFont2adr    0x02
#define         UserFont3adr    0x03
#define         UserFont4adr    0x04
#define         UserFont5adr    0x05
#define         UserFont6adr    0x06
#define         UserFont7adr    0x07
#define         UserFont8adr    0x0B
#define         UserFont9adr    0x10
#define         UserFont10adr   0x1C
#define         UserFont11adr   0x1D
#define         UserFont12adr   0x1E
#define         UserFont13adr   0x1F
#define         UserFont14adr   0xA0
#define         UserFont15adr   0xAD







//******************************************************************************
//********************************DEVICE****************************************
//******************************************************************************



//******************************MEMORY******************************************
#define         LineStAdr       0
#define         UserSysRegAdr   (uint16_t)DisplaySize
#define         UserFontsStAdr  (uint16_t)(UserSysRegAdr + 1)

#define         BrData          0x00ff
#define         LayoutData      0x0100
//******************************SYSTEM******************************************
#define         NotBusy         0x01
#define         InitDone        0x02
#define         LineWriting     0x04
#define         UserFontWr      0x10
//******************************************************************************










#endif