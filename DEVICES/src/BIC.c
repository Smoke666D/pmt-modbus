//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "BIC.h"
#include "system.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "Register.h"
//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static uint8_t          SysReg          = 0;            // Регистр внутренних флажков
static uint16_t         UserSysReg      = 0;            // Регистр пользовательских настроек

static uint8_t          CharCounter     = 0;            // Счетчик символов
static uint8_t          BIC_DisplayBuf[ComandList];  // Буффер строки на вывод

// Маасив сообщений для инициализации:
static uint8_t   BIC_InitSeq [InitSizeSeq]  = {LineFeedCmd,             // Очищаем экран
                                               FormFeedCmd,             // Перемещаем курсор в начало
                                               AutoCarRetModeCmd,       // Режим автоматического возврата каретки
                                               CurIsInvisibleCmd,       // Невидимый курсор
                                               EscCmd,                  // Начало команды
                                               LuminanceCmd,            // Команда изменения яркости
                                               LuminValeDef             // Значение яркости по умолчанию
                                              };


//******************************************************************************
//********************************CONST*****************************************
//******************************************************************************

static const uint8_t    UserFontsAdr[UserFontsMax] = {UserFont0adr,
                                                      UserFont1adr,
                                                      UserFont2adr,
                                                      UserFont3adr,
                                                      UserFont4adr,
                                                      UserFont5adr,
                                                      UserFont6adr,
                                                      UserFont7adr,
                                                      UserFont8adr,
                                                      UserFont9adr,
                                                      UserFont10adr,
                                                      UserFont11adr,
                                                      UserFont12adr,
                                                      UserFont13adr,
                                                      UserFont14adr,
                                                      UserFont15adr
                                                     };


//static const uint8_t    BettaFont[UserFontMas]   = {0x4C, 0x3A, 0x29, 0x9D, 0xF8};
//static const uint8_t    RoFont[UserFontMas]      = {0x4C, 0x4A, 0x27, 0x44, 0xF8};
//static const uint8_t    SvasticFont[UserFontMas] = {0xA0, 0x97, 0x4F, 0x2F, 0x00};

static const uint16_t   DispBaunds[5]  = {19200, 9600, 4800, 2400, 1200};        // Скорости индикатора, в зависимости от джамперов
static const uint16_t   DispParitys[4] = {USART_Parity_Even, USART_Parity_Odd, USART_Parity_No, USART_Parity_No};        // Партитетность, в зависимости от джамперов

//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************


void BIC_Init(void)
{
  uint8_t       DataInitAdr = 0;
    
  uint8_t       i = 0, k = 0;
  uint16_t      buf = 0;
  
  GPIO_InitTypeDef      GPIO_InitStruct;        // Структура инициализирующих данных GPIO
  USART_InitTypeDef     USART_InitStruct;       // Структура инициализирующих данных USART    
  uint16_t D_pins =     D7_pin | D6_pin | D5_pin | D4_pin | D3_pin | D2_pin | D1_pin | D0_pin;   // Пины параллельного порта (кроме USART)
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);        // Включаем тактирование USART2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);         // Включаем тактирование GPIOА
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);         // Включаем тактирование GPIOB
  
  GPIO_InitStruct.GPIO_Pin   = D_pins;                          // Иницилизируем системные пины
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;                // Режим выхода
  GPIO_Init(USART2_port, &GPIO_InitStruct);                     // Заданные настройки сохраняем в регистрах  
  
  GPIO_InitStruct.GPIO_Pin   = TX2_pin;                         // Настройки вывода USART2_Tx
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                // Скорость порта максимальная
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;                 // Режим альтернативной функции, выход Push-Pull
  GPIO_Init(USART2_port, &GPIO_InitStruct);                     // Заданные настройки сохраняем в регистрах  
  
  GPIO_InitStruct.GPIO_Pin   = nCS_pin | nWR_pin;               // Иницилизируем системные пины
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;                // Режим выхода
  GPIO_Init(ControlPort, &GPIO_InitStruct);                     // Заданные настройки сохраняем в регистрах  
  
  GPIO_InitStruct.GPIO_Pin   = BUSY_pin;                        // Иницилизируем системные пины
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;                   // Режим выхода
  GPIO_Init(ControlPort, &GPIO_InitStruct);                     // Заданные настройки сохраняем в регистрах  
    
  USART_InitStruct.USART_BaudRate                      = DispBaund;                       // Выбираем скорость из джамперов
  if (!DispParity)  USART_InitStruct.USART_WordLength  = USART_WordLength_8b;             // Длина слова 8 бит 
  else USART_InitStruct.USART_WordLength               = USART_WordLength_9b;             // Длина слова 9 бит (8+паритет)
  USART_InitStruct.USART_StopBits                      = USART_StopBits_1;                // 1 стоп-бит
  USART_InitStruct.USART_Parity                        = DispParity;                      // Паритет в зависимости от джамперов
  USART_InitStruct.USART_HardwareFlowControl           = USART_HardwareFlowControl_None;  // Без аппаратного контроля
  USART_InitStruct.USART_Mode                          = USART_Mode_Tx;                   // Включен передатчик и приемник USART1
  
  USART_Init(USART2, &USART_InitStruct);                // Заданные настройки сохраняем в регистрах USART1
  USART_Cmd(USART2, ENABLE);                            // Включаем USART     
  USART2->DR = 0x00;                                    // Засылаем первый байт
  WatchDog_reset();                                     // Сброс вачдога
  for(i=0;i<DisplaySize;i++) BIC_DisplayBuf[i] = 0x20;  // Заполняем буфер пробелами
  nCSreset;                                             // Выставляем чипселект
  
  UserSysReg = ReadHolding(UserSysRegAdr);              // Читаем системные настройки
  BIC_InitSeq[InitSizeStart-1] = (uint8_t)(UserSysReg & BrData);        // Записываем в массив инициализаци яркость
  
  DataInitAdr = InitSizeStart;                                          // Устанавливаем первый адресс для записи
  for(i=0;i<UserFontsMax;i++)                                           // Пробегаемся по 16 возможным символам
  {
    BIC_InitSeq[DataInitAdr] = EscCmd;                                  // Записываем начало команды записи
    DataInitAdr++;                                                      // Инкрементируем адресс в последовательности команд
    BIC_InitSeq[DataInitAdr] = SendUserFontCmd;                         // Записываем команду записи
    DataInitAdr++;                                                      // Инкрементируем адресс в последовательности команд
    BIC_InitSeq[DataInitAdr] = UserFontsAdr[i];                         // Записываем адресс символа
    DataInitAdr++;                                                      // Инкрементируем адресс в последовательности команд
    for(k=0;k<3;k++)                                                    // Перебираем 3 регистра по 16 бит
    {
      buf = ReadHolding(UserFontsStAdr + i*3 + k);                      // Читаем очередной регистр
      if(k)                                                             // Если это не первый регистр
      {
        BIC_InitSeq[DataInitAdr] = (uint8_t)((buf & 0xFF00) >> 8);      // Выдераем первый байт
        DataInitAdr++;                                                  // Инкрементируем адресс в последовательности команд
      }
      BIC_InitSeq[DataInitAdr] = (uint8_t)(buf & 0x00FF);               // выдираем второй байт
      DataInitAdr++;                                                    // Инкрементируем адресс в последовательности команд
    }
  }
  return;
}





//******************************************************************************
// Проверка окончания передачи байта и прочтения его дисплеем
// Return:
//  0 - линия занята
//  1 - линия свободна
//******************************************************************************
uint8_t DisplayNotBusy(void)
{
  if ((!BUSYread) && (USART2->SR & USART_SR_TC))
  { 
    SysReg |= NotBusy;
    return 1;
  }
  else return 0;
}
//******************************************************************************












//******************************************************************************
// Запись данных в дисплей
// Input:
//      data - указатель на массив данных
//      len  - длинна массива
// Output:
//      0 - Запись массива не закончена, требуеться очередное вхождение в функцию
//      1 - Запись массива завершена
//******************************************************************************
uint8_t WriteDataToDisp(uint8_t* data, uint16_t len)
{
  if (CharCounter < len)
  {
    USART2->DR = data[CharCounter];     // Шлем очередное сообщение
    SysReg &= ~NotBusy;                 // Сбрасываем флаг свободности канала
    CharCounter++;                      // Инкрементируем счетчик сообщений инциализации
    return 0;                           // Возвращаем статус не оконченности посылки
  }
  else
  {
    CharCounter = 0;    // Сбрасываем счетчик
    return 1;           // Возвращаем статус окончания посылки            
  }
}
//******************************************************************************








//******************************************************************************
// Функция конечного автомата для имитатора BIC. Вставляеться в обработчик поль-
// зовательского счетчика. Входные данные - регистры Mod Bus. Первые 20 реги-
// стров - это символы строки (ROM). Данные - младший разряд. 21-ый регистр - 
// - системные настройки(RAM). 0x0100 - смена раскладки, малдший разряд - ярк-
// ость. 22-69 регистры - пользовательские символы (RAM). В первом регистре
// используеться только младший байт. Изменения после перзаписи этих регистров
// вступают в силу только после рестарта контроллера.
//******************************************************************************
void BIC_FSA(void)
{
  uint8_t  i, k = 0, buf = 0; 
  WatchDog_reset(); 
  if (DisplayNotBusy())         // Если канал не занят
  {
    //*********************************INIT*************************************
    if (!(SysReg & InitDone))                   // Если инициализация не закончена или не произведена
    {
      if(WriteDataToDisp(BIC_InitSeq, InitSizeSeq)) SysReg |= InitDone;
      return;
    }
    //******************************LINE WRITING********************************
    if ((SysReg & LineWriting) && (WriteDataToDisp(BIC_DisplayBuf, ComandList))) // Если все еще пишеться строка и строка записана целиком
    {
      SysReg &= ~LineWriting;   // Сбрасываем флаг записи строки
      USART2->DR = FormFeedCmd; // Возвращаем каретку в начало дисплея
      SysReg &= ~NotBusy;       // Сбрасываем флаг свободности канала
    }
    //********************************NEW DATA**********************************
    else if (GetHoldingModifyFlag())            // Если строка не пишеться, проверяем наличие новых данных в регистрах ModBus
    {      
      SysReg |= LineWriting;            // Выставляем флажок начала записи строки
      ResetHoldingModifyFlag();         // Сбрасываем флаг приема новых данных
      k = 0;
      for(i=4;i<ComandList;i++)         // Заполняем буфер
      {
        buf = (uint8_t)ReadHolding(k);                  // Читаем из регистров данные
        if (buf < UserFontsMax)                         BIC_DisplayBuf[i] = UserFontsAdr[buf];  // Если адресы пользов. символов
        if ((buf < 0x1F) && (buf >= UserFontsMax))      BIC_DisplayBuf[i] = 0x20;               // Отсекаем команды, оставляя только символы
        else                                            BIC_DisplayBuf[i] = buf;                // Заполняем буфер дисплея данными из регистров ModBus
        k++;
      }
      UserSysReg        = ReadHolding(UserSysRegAdr);
      BIC_DisplayBuf[0] = EscCmd;                                               // Начало команды
      BIC_DisplayBuf[1] = LuminanceCmd;                                         // Смена яркости дисплея
      BIC_DisplayBuf[2] = (uint8_t)(UserSysReg & BrData);                       // Значение яркости
      if (UserSysReg & LayoutData) BIC_DisplayBuf[3] = KatakanaFontCmd;         // Смена раскладки 0х19 - вторая 
      else                         BIC_DisplayBuf[3] = InternationalFontCmd;    // Смена раскладки 0х18 - стандартная
    }
    //**************************************************************************
  } 
  return;
}
//******************************************************************************


