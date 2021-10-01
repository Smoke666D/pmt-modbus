//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "I2C.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "math.h"
#include "system.h"
//***************************LOCAL VAR*****************************************
static uint8_t          I2C_NUM   = 1;                  // Номер устройства
static uint8_t          I2C_SPEED = 100000;             // Скорость обменна данных
static uint8_t          I2C_CR    = I2C_CR_MASTER;      // Настройки устройства
//*****************************************************************************
static uint8_t		I2C_USER    = 0;		        // Регистр состояний
static uint16_t	        I2C_COUNT   = 0;                        // Счетчик байт в буфере
static uint16_t         I2C_SEQ_LEN = 0;                        // Длинна последовательности команд
static uint8_t          I2C_HWADR_BUFFER[I2C_BUFFER_SIZE];      // Буфер аппаратных адрессов устройств
static uint16_t 	I2C_ADR_BUFFER[I2C_BUFFER_SIZE];        // Буфер адресов памяти
static uint8_t 	        I2C_DATA_BUFFER[I2C_BUFFER_SIZE];       // Буфер данных
static uint8_t          I2C_MODE    = 0;                        // Режим реботы
static uint32_t         I2C_STAT    = 0;                        // Регистры состяния интерфейса
static uint32_t         I2C_DELAY   = 0;                        // Счетчик задержки перед рестартом

uint32_t tmp1, tmp2;
uint8_t  data;


#if(I2C_DebugMode)
  #define               DebugNum        60
  static uint32_t       laststat = 0;
  static uint32_t       counter  = 0;
  uint32_t              StatLog[DebugNum];
#endif
//****************************Function*****************************************
uint32_t I2C_GetStatus(void);                                   // Получить статус I2C
void     I2C2_EV_IRQHandler(void);	                        // Прерывания по событиям
void     I2C2_ER_IRQHandler(void);	                        // Прерывание по ошибкам
//*****************************************************************************
//***************************Initialization************************************
//*****************************************************************************
uint8_t I2C_setup(uint8_t nomber, uint16_t speed, uint8_t master, uint8_t remap)
{
  //***************************
  if (nomber == 1)
  {
    #define     I2C             I2C1            // Обозначение устройства
    #define     I2C_port        GPIOB           // Порт пинов
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// Включаем тактирование I2C1
    if(remap)
    {
      #define   SDA_pin         GPIO_Pin_9
      #define   SCL_pin         GPIO_Pin_8
    }
    else
    {
      #define   SDA_pin         GPIO_Pin_7
      #define   SCL_pin         GPIO_Pin_6
    }
  }
  else if (nomber == 2)
  {
    #define     I2C             I2C2                    // Обозначение устройства
    #define     SCL_pin         GPIO_Pin_10
    #define     SDA_pin         GPIO_Pin_11
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;         // Включаем тактирование I2C2
  }
  else 
  {
    return 0;
  }
  //***************************
  if (speed <= I2C_MAX_SPEED)
  {
    I2C_SPEED = speed;
  }
  else
  {
    return 0;
  }
  //***************************
  if (master)
  {
    I2C_CR |= I2C_CR_MASTER;
  }
  //***************************
  
  if (remap)
  {
    I2C_CR |= I2C_CR_REMAP;
  }
  else
  {
    I2C_CR &= ~I2C_CR_REMAP;
  }
  //***************************
  return 1;
}


void I2C_init(void)
{
  I2C_InitTypeDef          	I2C_InitStruct;         // Структура инициализирующих данных I2C
  GPIO_InitTypeDef         	GPIO_InitStruct;        // Структура инициализирующих данных GPIO
  //****************************************************************************
  #if(I2C_DebugMode)
    uint32_t i;
    for(i=0;i<DebugNum;i++)
    {
      StatLog[i]=0xFFFFFFFF;
    }
  #endif
  //********************************RCC*****************************************
    /*
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;           // Включаем тактирование порта
  if (I2C_REMAP)                                // Если выбраны ремапиные пины
  {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;         // Включаем алтернативные функции
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;         // Задаем альтернативные пины (PB8, PB9)
  }
  if (I2Cn == 1)
  {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// Включаем тактирование I2C1
  }
  else if (I2Cn == 2)
  {
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;         // Включаем тактирование I2C2
  }
    */
  //********************************GPIO****************************************
  GPIO_InitStruct.GPIO_Pin   = SCL_pin | SDA_pin;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_OD;
  GPIO_Init(I2C_port, &GPIO_InitStruct);
  //********************************I2C*****************************************
  I2C_DeInit(I2C);
  I2C_InitStruct.I2C_Mode                = I2C_Mode_I2C;                        // Мастер режим
  I2C_InitStruct.I2C_DutyCycle           = I2C_DutyCycle_16_9;                  // ХЗ)))
  I2C_InitStruct.I2C_OwnAddress1         = 0x01;                                // Адресс МК - что бы было
  //I2C_InitStruct.I2C_Ack                 = I2C_Ack_Enable;		        // Разрешаем ASK сообщения
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;        // 7-бит адресс
  I2C_InitStruct.I2C_ClockSpeed          = 100000;                              // Скорость передачи данных 100 KHz
  I2C->CR1 |= I2C_CR1_PE;                                                  	// Включаем устройство
  I2C_Init(I2C, &I2C_InitStruct);                                        	// Записываем данные
  
  //I2C->SR1 |= I2C_CR1_ACK;                                                      // Разрешаем ACK
  
  //I2C->CR2 |= I2C_CR2_ITEVTEN;							// Разрешаем прерывания по событиям (SB,ADDR,ADD10,STOPF,BTF)
  //I2C->CR2 |= I2C_CR2_ITBUFEN;                                                  // Разрешаем прерывания по событиям буфера (TxE,RxNE)
  //NVIC_EnableIRQ(I2C2_EV_IRQn);							// Разрешаем прерывания по событиям
  //NVIC_SetPriority(I2C2_EV_IRQn, 1);                                            // Устанавливаем приоритет 
  
  //I2C->CR2 |= I2C_CR2_ITBUFEN;						// Разрешаем прерывания по ошибкам
  //NVIC_EnableIRQ(I2C2_ER_IRQn);						// Разрешаем прерывания по ошибкам
  I2C_USER = 0;						                        // Сбрасываем все флажки
  //****************************************************************************
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************






//******************************************************************************
uint32_t I2C_GetStatus(void)
{
  uint32_t fl1 = 0;
  uint32_t fl2 = 0;
  uint32_t out = 0;
  
  fl1 = I2C->SR1;
  fl2 = I2C->SR2;
  fl2 = fl2 << 16;
  out = (fl1 | fl2) & I2C_FLAG_MSK;
  
  return out;
}
//******************************************************************************

























//******************************************************************************
//******************************************************************************
//******************************************************************************
// Запихнуть обработчик рестарата в таймер.
//******************************************************************************
uint8_t trash = 0;      // Помойка

void I2C_Proc(void)
{
  I2C_STAT = I2C_GetStatus();
  //********************************DEBUG***************************************
  #if(I2C_DebugMode)
    if ((counter == 0)||(I2C_STAT != laststat))
    {
      laststat = I2C_STAT;
      StatLog[counter] = I2C_STAT;
      counter++;
    }
  #endif
  //***************************MAIN PROCESSING**********************************
  switch (I2C_MODE)
  {
    //*****************************START****************************************
    case 0:
      if (I2C_USER & Busy_msk)
      {
        I2C_MODE = 1;
      }
      break;
    //********************START SENT -> SEND HW ADR*****************************
    case 1:
      if (I2C_STAT == I2C_STAT_START)                                           // SB                   MSL,BUSY        - EV5 - Старт успешно ушел
      {
        I2C_USER &= ~ReStart_msk;                                               // Снимаем флаг перезапуска
        if(((I2C_USER & ReadMode_msk)>0) && ((I2C_USER & AdrSend_msk)>0))	// Если режим чтения и уже записан адресс памяти
        {
          I2C->DR  = I2C_HWADR_BUFFER[I2C_COUNT] | I2C_ADR_SEND_MSK;            // Добавляем к адресу устройства бит чтения
          I2C_MODE = 4;                                                         // Перходим на чтение байта
        }
        else                                                                    // Если режим записи
        {
          I2C->DR = I2C_HWADR_BUFFER[I2C_COUNT];                                // Отправляем адрес устройства
          I2C_MODE = 2;                                                         // Переходим к записи адреса памяти
        }
      }
      break;
    //********************HW ADR SENT, TRANSMIT MODE****************************
    case 2:
      if (I2C_STAT == I2C_STAT_HWADR_WRITE_SENT)                                // ADDR, TxE            MSL,BUSY,TRA    - EV6 - Адрес на запись устройства ушел, режим записи 
      {
        if (I2C_ADR16)								// Если адрес в памяти двух байтовый
        {
          I2C->DR = (uint8_t)((I2C_ADR_BUFFER[I2C_COUNT] & 0xFF00) >> 8);	// Отправляем старший байт адреса памяти
        }
        else									// Если адрес памяти однобайтовый
        {
          I2C->DR = (uint8_t)(I2C_ADR_BUFFER[I2C_COUNT] & 0x00FF);		// Отправляем один байт адреса памяти
          I2C_USER |= AdrSend_msk;						// Выставляем флаг окончания передачи адреса
        }
        I2C_MODE = 3;                                                           // Переходим к анализу запроса/отправки данных
      }
      break;
    //*************************BYTE SENT****************************************
    case 3:
      if (I2C_STAT == I2C_STAT_DATA_SENT)                                       // 
      {
        if (I2C_ADR16 && !(I2C_USER & AdrSend_msk))				// Если мы не отправили младший байт двухбайтового адреса памяти...
        {
          I2C->DR   = (uint8_t)(I2C_ADR_BUFFER[I2C_COUNT] & 0x00FF);	        // Отправляем младший байт адреса памяти
          I2C_USER |= AdrSend_msk;						// Выставляем флаг окончания передачи адреса
        }
        else if (I2C_USER & ReadMode_msk)                                       // Если режим чтения
        {
          I2C->CR1 |= I2C_CR1_START;                                            // Запускаем старт
          I2C_MODE  = 1;                                                        // Переходим к обработке старта
        }
        else if (!(I2C_USER & DataSend_msk))                                    // Если еще не записанны данные
        {
          I2C->DR   = I2C_DATA_BUFFER[I2C_COUNT];			        // Отправляем байт данных
          I2C_USER |= DataSend_msk;                                             // Выставляем флаг отправки данных
        }
        else                                                                    // Если запись данных окончена
        {
          I2C_COUNT++;								// Инкрементируем счетчик отправленных байт
          //I2C2->CR1 &= ~I2C_CR1_ACK;                                          // Выключем ACK
          I2C->CR1 |= I2C_CR1_STOP;                      			// Шлем STOP
          I2C_USER &= ~(AdrSend_msk|DataSend_msk);                              // Сбрасываем флажки
          if (I2C_COUNT < I2C_SEQ_LEN)                                          // Если последовательность команд не закончена
          {
            I2C_MODE = 50;/*!!!*/                                               // Запускаем старт
          }
          else
          {
            I2C_USER = 0;                                                       // Снимаем флаг занятости уустройства
            I2C_MODE = 0;                                                       // Переходим к ожиданию новой посылки
            #if(I2C_DebugMode)
              counter  = 0;
              laststat = 0;
            #endif
          }
        }
      }
      break;  
    //**********************BYTE HAVE RECEIVED**********************************
    case 4:
      if (I2C_STAT == I2C_STAT_DATA_READY_GET)           // 
      {
        I2C->CR1 |= I2C_CR1_STOP;                       // Посылаем команду остановки (NAK без ACK)
        I2C_DATA_BUFFER[I2C_COUNT] = (I2C->DR);       // Принимаем байт сообщения
        trash = (I2C->DR);
        I2C_COUNT++;					// Инкрементируем счетчик полученных данных
        I2C_USER &= ~(AdrSend_msk|DataSend_msk);        // Сбрасываем флажки
        if (I2C_COUNT < I2C_SEQ_LEN)                    // Если последовательность команд не закончена
        {
          I2C_MODE = 50;                                 // Переходим к рестарту
        }
        else
        {
          I2C_USER = 0;                                 // Снимаем флаги
          I2C_MODE = 0;                                 // Переходим к ожиданию новой посылки
          #if(I2C_DebugMode)
            counter  = 0;
            laststat = 0;
          #endif
        }
      }
      break;
    //**********************BYTE HAVE RECEIVED**********************************  
    case 50:
      if (I2C_STAT == I2C_STAT_DATA_HAVE_GET)
      {
        data = I2C->DR;
      }
      if (I2C_STAT == I2C_STAT_STOP)
      {
        I2C_MODE = 5;
      }
      if (I2C_STAT == I2C_STAT_BUSY)
      {
        //I2C->CR1 |= I2C_CR1_START;      // Запускаем старт
        //I2C->CR1 |= I2C_CR1_STOP;                       // Посылаем команду остановки (NAK без ACK)
      }
      break;
    //**************************RESTART*****************************************
    case 5:
      if (I2C_STAT == I2C_STAT_STOP)    // 
      {
        I2C->CR1 |= I2C_CR1_START;      // Запускаем старт
        I2C_MODE  = 1;                  // Переходим на обработку старта
      }
      else
      {
        I2C->SR1;
        I2C->SR2;
      }
      break;
  }
  return;    
}
//******************************************************************************
//******************************************************************************
//******************************************************************************






















//******************************************************************************
void I2C_Write(uint8_t* HwAdr, uint16_t* adr, uint8_t* data, uint16_t len)
{
  uint16_t i;
	
  I2C_USER  = Busy_msk;			// Переводим в режим записи
  for(i=0;i<len;i++)			// Записываем в буфер данные на отправку
  {
    I2C_HWADR_BUFFER[i] = HwAdr[i];     // Записываем адрес устройства
    I2C_ADR_BUFFER[i]   = adr[i];  	// Копируем адреса
    I2C_DATA_BUFFER[i]  = data[i];	// Копируем данные
  }
  I2C_SEQ_LEN = len;                    // Записываем длинну последовательности команд
  I2C_COUNT   = 0;			// Сбрасываем счетчик байт
  I2C->CR1   |= I2C_CR1_START; 	        // Запускаем старт
}
//*****************************************************************************
void I2C_Read(uint8_t* HwAdr, uint16_t* adr, uint16_t len)
{
  uint16_t i;
	
  if (I2C->SR2 & I2C_SR2_BUSY)
  {
    return;
  }
  I2C_USER = Busy_msk | ReadMode_msk;	// Переводим в режим чтения
  for(i=0;i<len;i++)	                // Записываем в буфер данные на отправку
  {
    I2C_HWADR_BUFFER[i] = HwAdr[i];     // Записываем адрес устройства
    I2C_ADR_BUFFER[i]   = adr[i];  	// Копируем адреса
    I2C_DATA_BUFFER[i]  = 0;	        // Чистим данные
  }
  I2C_SEQ_LEN = len;                    // Записываем длинну последовательности команд
  I2C_COUNT   = 0;			// Сбрасываем счетчик байт
  I2C->CR1   |= I2C_CR1_START;          // Запускаем старт
  return;
}
//*****************************************************************************
uint8_t* I2C_GetData(void)
{
  return I2C_DATA_BUFFER;
}
//*****************************************************************************
uint8_t I2C_GetBusy(void)
{
  return I2C_USER & Busy_msk;
}
//*****************************************************************************





//*****************************************************************************
//************************Interrupt routine************************************
//*****************************************************************************
void I2C2_EV_IRQHandler(void)
{
  return;
}
//*****************************************************************************
void I2C2_ER_IRQHandler(void)
{
  return;
}
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
