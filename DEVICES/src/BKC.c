//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "BKC.h"
#include "system.h"
#include "Register.h"
#include "math.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static uint8_t          SysReg          = 0;            // Регистр внутренних флажков
static uint8_t          KeyDownCounter  = 0;            // Счетчик времени нажатости кнопки
static uint8_t          I2C_time        = 0;            // Счетчик времени между чтением регистров расшерителя портов
static uint16_t         data            = 0;            // Данные с первого регистра
//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************
uint8_t CheckEventCycle(uint16_t _SR1, uint16_t _SR2);
uint8_t I2C_ByteRead(uint8_t HwAddr, uint8_t ReadAddr);
//******************************************************************************
//*******************************INIT*******************************************
//******************************************************************************
void BKC_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStruct;                                     // Структура инициализирующих данных GPIO
  I2C_InitTypeDef          I2C_InitStruct;                                      // Структура инициализирующих данных I2C
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);                         // Включаем тактирование GPIOА
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);                         // Включаем тактирование GPIOB
  GPIO_InitStruct.GPIO_Pin =  SCL_pin | SDA_pin;                                // Инициализируем пины I2C
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                                // Максимальная скорость
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;                                  // Альтернативная функция  
  GPIO_Init(I2C_port, &GPIO_InitStruct);                                        // Инициализируем
  GPIO_InitStruct.GPIO_Pin =  SW_trig_pin;                                      // Иницилизируем пин кнопки
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;                                   // Режиимвхода
  GPIO_Init(SW_port, &GPIO_InitStruct);                                         // Инициализируем
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);                           // Включаем тактирование I2C1
  I2C_DeInit(I2C2);                                                             // Чистим структуру инициализации
  I2C_InitStruct.I2C_Mode                = I2C_Mode_I2C;                        // Мастер режим
  I2C_InitStruct.I2C_DutyCycle           = I2C_DutyCycle_16_9;                  // ХЗ)))
  I2C_InitStruct.I2C_OwnAddress1         = 1;                                   // Адресс МК - что бы было
  I2C_InitStruct.I2C_Ack                 = I2C_Ack_Enable;                      // Разрешаем ASK сообщения
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;        // 7-бит адресс
  I2C_InitStruct.I2C_ClockSpeed = 100000;  /* 100kHz */                         // Скорость передачи данных 100 KHz
  I2C_Cmd(I2C2, ENABLE);                                                        // Включаем устройство
  I2C_Init(I2C2, &I2C_InitStruct);                                              // Записываем данные
  I2C_AcknowledgeConfig(I2C2, ENABLE);                                          // Включаем ASK-и
  return;
}
//******************************************************************************
//**********************************I2C2****************************************
//******************************************************************************



//*****************CheckEventCycle*******************
// Цеклический опрос регистров I2C2 до предела из
// BKC.h - I2C_timout. 
// Include:
//      _SR1 - ожидаемое сотсояние ркгистра SR1
//      _SR2 - ожидаемое сотсояние ркгистра SR2
// Output:
//      1 - Успешно прошел тест
//      0 - Значения не установились
//***************************************************
uint8_t CheckEventCycle(uint16_t _SR1, uint16_t _SR2)
{
  uint32_t i;
  for(i=0;i<I2C_timout;i++) 
  {
    if ((I2C2->SR1 & _SR1) && (I2C2->SR2 & _SR2)) 
    {
      return 1;
    }
  }
  return 0;
}
//***************************************************





//*********************I2C_ByteRead**************************
// Чтение регистра из расшерителя порта
// Input:
//      HwAddr   - сетевой адресс микросхемы
//      ReadAddr - адресс ресистра в микрухе
// Output:
//      data - данные из регистра (0 - возможно ошибка связи)
//***********************************************************
uint8_t I2C_ByteRead(uint8_t HwAddr, uint8_t ReadAddr)
{
  uint8_t  tmp = 0;             
  
  if(I2C2->SR2 & I2C_SR2_BUSY)
  {
    return 0;                                           // Проверяем занят ли канал
  }
  I2C2->CR1 |= I2C_CR1_START;                           // Посылаем сигнала старта
  if(!CheckEventCycle(0x0001, 0x0003)) 
  {
    return 0;                                           // SB                   MSL,BUSY 
  }
  I2C2->DR = HwAddr;                                    // Отправляем сетевой адресс мкросхемы на запись
  if(!CheckEventCycle(0x0082, 0x0007)) 
  {
    return 0;                                           // ADDR, TxE            MSL,BUSY,TRA
  }
  I2C2->DR = ReadAddr;                                  // Отправляем адресс регистра
  if(!CheckEventCycle(0x0084, 0x0007)) 
  {
    return 0;                                           // DTF, TxE             MSL,BUSY,TRA
  }
  I2C2->CR1 |= I2C_CR1_START;                           // Отправляем повторный старт
  if(!CheckEventCycle(0x0001, 0x0003)) 
  {
    return 0;                                           // SB                   MSL,BUSY 
  }
  I2C2->DR = HwAddr + 1;                                // Отправляем сетевой адресс мкросхемы на чтение
  if(!CheckEventCycle(0x0002, 0x0002)) 
  {
    return 0;                                           // ADDR                 BUSY
  }
  if(!CheckEventCycle(0x0040, 0x0002)) 
  {
    return 0;                                           // RxNE                 BUSY
  }
  tmp = (uint8_t)I2C2->DR;                              // Чтаем байт с регистра
  I2C2->CR1 &= ~I2C_CR1_ACK;                            // Выключем ACK  
  I2C2->CR1 |= I2C_CR1_STOP;                            // Посылаем команду остановки (NAK без ACK)
  return ~tmp;                                          // Возвращаем данные срегистра
 }
//************************************************************









//******************************************************************************
//******************************************************************************
//******************************************************************************
void BKC_FSA(void)
{  
  uint16_t input = 0;
  uint16_t output = 0;
  
  //********************************NEW DATA**********************************
  if (GetHoldingModifyFlag())
  {
    ResetHoldingModifyFlag();                                                   // Сбрасываем флаг приема новых данных
  }
  else
  {
    if ((!(SwichPressed)) && (SysReg & DataWriteDone))                          // Если кнопка не нажата и данные записаны 
    {
      SysReg &= ~DataWriteDone;                                                 // Cбрасываем флажок записи данных
      KeyDownCounter = 0;                                                       // Сбрасываем счетчик удержания кнопки
    }
    
    if ((SwichPressed) && (!(SysReg & DataWriteDone)) && (KeyDownCounter++ > KeyDownTimout) ) 
    {
      SysReg |= KeyDownOk;                                                      // Если кнопка удержана достаточно времени - ставим флаг       
    }
    if (SysReg & KeyDownOk)                                                     // Если кнопка удержана нажатой достаточное время
    {
      if (!(SysReg & Data0Done))                                                // Если данные из нулевого регистра не прочитаны
      {
        data = ((uint16_t)(I2C_ByteRead(TCA_HwAdr,InputPort0)));                // Читаем данне из нулевого регистра
        SysReg |= Data0Done;                                                    // Выставляем флажок
        I2C_time = 0;                                                           // Сбрасываем счктчик
      }
      else
      {
        if(I2C_time < 5) I2C_time++;                                            // Ждем между чтением второго регистра 
        else 
        {
          input = (uint16_t)(I2C_ByteRead(TCA_HwAdr,InputPort1));               // Читаем первого регистр
          data |= input << 8;                                                   // Дополняем данные
          SysReg |= Data1Done;                                                  // Выставляем флажок прочтения первого регистра
        }
      }
      if (SysReg & Data1Done)                                                   // Если прочитан первый регистр
      {
        WriteShortToHolding(ValS2  , (uint16_t)(15 - (data & 0x000F)));         // Записываем в первый регистр состояние вертелки S2
        
        output = (uint16_t)(15 - ((data >> 8)  & 0x000F));
        output = output + (uint16_t)(15 - ((data >> 12) & 0x000F))*10;
        output = output + (uint16_t)(15 - ((data >> 4)  & 0x000F))*100;
        
        WriteShortToHolding(ValS3,output);
        /*
        WriteShortToHolding(ValS3_2, (uint16_t)(15 - ((data >> 4)  & 0x000F)));
        WriteShortToHolding(ValS3_0, );
        WriteShortToHolding(ValS3_1, (uint16_t)(15 - ((data >> 12) & 0x000F)));
        */
        SysReg &= ~(KeyDownOk|Data1Done|Data0Done);                                       // Сбрасываем флажок удержания кнопки
        SysReg |= DataWriteDone;                                                          // Выставляем флажок записи данных
      }
    } 
  }
}