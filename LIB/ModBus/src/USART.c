//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "USART.h"
#include "init.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "system.h"
#include "register.h"
#include "port.h"
#include "mbrtu.h"
//#include "user_process.h"
//************Определение глобальных переменных********************************
static eMBSndState eSndState;
static  eMBRcvState eRcvState;
unsigned long TimerResponceTimeOut =0;

eMBSndState GetSendState()
{
  return eSndState;
}
eMBRcvState GetRcvState()
{
  return eRcvState;
}


//***************************Global VAR****************************************
//                                 0     1   2    3     4    5     6     7      8     9      10      
unsigned long BoundRateTable[]={1200,2400,4800,9600,14400,19200,38400,56000,115200,128000,256000};

static uint8_t Count_errRead=0;

volatile uint8_t  Timer_NOK_Counter = 0;
volatile uint8_t  Timer_0_5_Counter = 0;

// USART Receiver buffer
#define RX_BUFFER_SIZE 260
static  uint8_t rx_buffer[RX_BUFFER_SIZE];

volatile uint16_t rx_wr_index=0,rx_rd_index=0;
volatile uint16_t rx_counter=0;
volatile uint8_t rx_buffer_overflow=0;

// USART Transmitter buffer
#define TX_BUFFER_SIZE 260
static uint8_t  tx_buffer[TX_BUFFER_SIZE];
static uint16_t tx_wr_index=0,tx_rd_index=0;
static uint16_t tx_counter=0;
static uint16_t Byte_Counter;

void Usart1_Send_symbol(uint8_t); //Объявление функции передачи символа
void Usart1_Send_String(char* str); //Объявление функции передачи строки

//***************************Global STRUCT*************************************
NVIC_InitTypeDef        NVIC_InitStructure;
//**************************Функциии работы со счетчиком байтов****************

//**************************Получение счетчика принетыx байтов фрейма 
unsigned short GetATUByteCounter(void)
{
  unsigned short CurrentByteCounter;
  CurrentByteCounter = Byte_Counter;
  return CurrentByteCounter;
}
//*****************************************************************************
void USART1_init(void)
{ 
  //*****************************Struct*****************************************
  USART_InitTypeDef     USART_InitStruct;       // Структура инициализирующих данных USART    
  //********************************RCC*****************************************
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);                // Включаем тактирование USART1
  //*******************************USART****************************************
  USART_InitStruct.USART_BaudRate = BoundRateTable[ReadUserRegister(0)];        // Выбираем скорость передачи данных
  
  switch (ReadUserRegister(1))
  {
    case STOP_BIT_2:
      USART_InitStruct.USART_WordLength         =  USART_WordLength_8b;             // Длина слова 8 бит
      USART_InitStruct.USART_StopBits            = USART_StopBits_2;                // 1 стоп-бит
      USART_InitStruct.USART_Parity              = USART_Parity_No;
      break;
    case PARITY_EVEN:
      USART_InitStruct.USART_WordLength          = USART_WordLength_9b;             // Длина слова 8 бит
      USART_InitStruct.USART_StopBits            = USART_StopBits_1;                // 1 стоп-бит
      USART_InitStruct.USART_Parity              = USART_Parity_Even;    
       break;
    case PARITY_ODD:
      USART_InitStruct.USART_WordLength          = USART_WordLength_9b;             // Длина слова 8 бит
      USART_InitStruct.USART_StopBits            = USART_StopBits_1;                // 1 стоп-бит
      USART_InitStruct.USART_Parity              = USART_Parity_Odd;    
       break;
  }               
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // Без аппаратного контроля
  USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   // Включен передатчик и приемник USART1
  USART_Init(USART1, &USART_InitStruct);                                        // Заданные настройки сохраняем в регистрах USART1
  USART_Cmd(USART1, ENABLE);                                                    // Включаем USART1
  RS485_nRE;                                                                    // Готов к приему по RS485
  USART1->CR1  |= USART_CR1_RXNEIE;                                             // Разрешаем прерывание по приему данных  
  NVIC_SetPriority(USART1_IRQn, 0);                                             // Устанавливаем уровень приоритета прерывания
  NVIC_EnableIRQ (USART1_IRQn);                                                 // Включаем прерывания по каналу
  WatchDog_reset();                                                             // Сброс вачдога
  Timer_0_5_Counter = 0;                                                        // Сбрасываем счетчик пол чара
  
  for (int k=0;k<TX_BUFFER_SIZE;k++)                                            // Инициализируем буфер
  {
    tx_buffer[k] = 0x55;
  }
  Byte_Counter = 0;
  rx_counter   = 0;  
  tx_counter   = 0;   
  eRcvState    = STATE_RX_INIT;    
  eSndState    = STATE_TX_IDLE;  
  return;
}


#if  UART2_INIT > 0
void USART2_init(void)
{  
  USART_InitTypeDef       USART_InitStruct;       // Структура инициализирующих данных USART     
  //Инициализация USART2
  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);                        // Включаем тактирование USART1   
  USART_InitStruct.USART_BaudRate            = 19200; 
  USART_InitStruct.USART_WordLength          = USART_WordLength_9b;             // Длина слова 8 бит
  USART_InitStruct.USART_StopBits            = USART_StopBits_1;                // 1 стоп-бит
  USART_InitStruct.USART_Parity              = USART_Parity_Even ;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // Без аппаратного контроля
  USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   // Включен передатчик и приемник USART1
  USART_Init(USART2, &USART_InitStruct);                                        // Заданные настройки сохраняем в регистрах USART1
  USART_Cmd(USART2, ENABLE);     
  WatchDog_reset();    
  return;
}
#endif
//*****************************************************************************
//*******************************USART*****************************************
//*****************************************************************************
uint8_t get_char(uint8_t * data)
{
  if (rx_counter)
  {
    __disable_irq ();    
    *data = rx_buffer[rx_rd_index];
    if (++rx_rd_index == RX_BUFFER_SIZE)
    {
      rx_rd_index = 0;
    }
    --rx_counter;
    __enable_irq ();    
    return 1;
  }
  else
  {
    return 0;
  }
}

//#define _ALTERNATE_PUTCHAR_

//Вернул как было чутка подправив включение на передачу

void put_str_pak_master(UCHAR *s, uint16_t c)
{
  WatchDog_reset();
  __disable_irq ();
  tx_rd_index=0;  
  tx_counter = c-2;
  eSndState = STATE_TX_XMIT; 
  for (unsigned int i=0;i<c-1;i++)
  {  
    tx_buffer[i]=s[i+1];
  }
  RS485_DE; 
  USART_ClearITPendingBit(USART1, USART_IT_TC); 
  USART1->DR   = s[0];  
  USART1->CR1 |= USART_CR1_TCIE;                        //разрешаем прерывание  по передаче  
  //***********************12.03 добавил*****************************
  USART1->CR1  &= ~USART_CR1_RXNEIE; 
  __enable_irq ();  
return;
}

void PutCharUART2 (UCHAR DATA)
{
  WatchDog_reset();
  USART2->DR = DATA;  
  return;
}


void put_char(uint8_t c) 
{  
  WatchDog_reset();
  __disable_irq ();       
  switch (eSndState)
  {          
    case STATE_TX_XMIT:     
      tx_counter++;  
      tx_buffer[tx_wr_index++] = c;         
      USART_ITConfig(USART1,USART_IT_TC, ENABLE);          
      break;
    case STATE_TX_IDLE: 
      RS485_DE;
      eSndState    = STATE_TX_XMIT;  
      tx_rd_index  = 0;      
      tx_counter++;
      tx_wr_index  = 0;
      USART1->DR   = c;  
      USART1->CR1 |= USART_CR1_TCIE; //разрешаем прерывание  по передаче  
      //***********************12.03 добавил*********************
      USART1->CR1  &= ~USART_CR1_RXNEIE; //запрещаем прерывание  по приему
      //***********************      
      break;
  }
  __enable_irq ();
  return;
}

uint8_t*  Get_RX_Buffer(void)
{
 return rx_buffer;
}

void put_str_pak(UCHAR *s, uint16_t c)
{
  for (uint16_t k=0; k < c; k++)
  {
    put_char(*s++); 
  }
  return;
}

//И функцию передачи целого числа
void put_int(int32_t data)
{
  UCHAR temp[10],count=0;
  if (data<0) 
  {
    data=-data;
    put_char('-');
  }         
  if (data)
  {
    while (data)
    {
      temp[count++]=data%10+'0';
      data/=10;                       
    }               
    while (count)   
    {
      put_char(temp[--count]);                
    }
  }
  else 
  {
    put_char('0'); 
  }
  return;      
}

void Start0_5Timer()
{ 
  TIM1->CNT  = 0;               // Сбрасываем пересчитываемое значение
  TIM1->CR1 |= TIM_CR1_CEN;     // Включаем счетчик
  Timer_0_5_Counter = 0;        // Сбрасываем счетчик прерываний
  return;
}

void USART1_IRQHandler(void)
{  
  WatchDog_reset();  
  __disable_irq ();  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)   // Проверка прерывание по приему байта  
  { 
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);     // Сбрасываем флаг причины прерывания
    //  Дело было не в флагах всё ломал гетчар
    //if ((USART1->SR & (USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_ORE)) == 0)
    
    if ((USART1->SR & (USART_SR_PE|USART_SR_FE|USART_SR_NE|USART_SR_ORE)) == 0)
    {      
      switch (eRcvState)
      {                   
        case STATE_M_RX_IDLE:                                   
          eSndState = STATE_TX_IDLE;
          break;
        case STATE_RX_IDLE:                                     // Приемник в режиме подготовки к приему
          eRcvState    = STATE_RX_RCV;                          // Переводим в режим приема
          rx_buffer[0] = USART1->DR & 0xFF;
          Byte_Counter = 1;                                     // Сбрасываем счетчик байтов
          rx_counter   = 1;                                     // Сбрасываем счетчик буфера
          
          break;
        case STATE_RX_RCV:                                      // Приемник в режиме приема
          Byte_Counter++;                                       // Инкрементируем счетчик байтов
          rx_buffer[rx_counter++] = USART1->DR & 0xFF;          // Записываем очередной входной байт в буфер
          Start0_5Timer();                                      // Запускаем таймер
          break;
        case STATE_RX_ERROR:
          Count_errRead++;
          break;
        case STATE_RX_INIT:                                     // Приемник в стартовом состоянии инициализации
          USART_ReceiveData(USART1);                            // Затираем данные на прием
          Start0_5Timer();                                      // Запускаем таймер
          break;                                    
      }
    }
    else
    {
      USART_ReceiveData(USART1);
    }
  }
  if(USART_GetITStatus(USART1, USART_IT_TC) == SET)             // Проверка прерывание по передаче байта
  {            
    USART_ClearITPendingBit(USART1, USART_IT_TC);               // Соброс бита признака прирывания 
    switch (eSndState)
    {
      case STATE_TX_IDLE:    
        eRcvState = STATE_RX_IDLE;            
        break;                         
      case STATE_TX_XMIT:
        if (--tx_counter)
        {                     
          USART1->DR = tx_buffer[tx_rd_index];
          tx_rd_index++;                
        }
        else
        {
          tx_counter = 0;
          eSndState  = STATE_M_TX_XFWR; 
          RS485_nRE;                            // Переключаем на прием
          USART1->CR1  &= ~USART_CR1_TCIE;      // Запрещаем прерывание  по передаче   
          USART1->CR1  |= USART_CR1_RXNEIE;     // Разрешаем  прерывание  по приему                                    
          Start0_5Timer();
        }
        break;                  
    }    
  }  
  __enable_irq ();
  return;
}




void TIM1_UP_IRQHandler(void)
{ 
  UCHAR RespondTimeOut = 0;
  TIM1->SR &= ~TIM_SR_UIF;              // Сбрасываем флажек прерывания  
  __disable_irq ();                     // Глобальный запрет прерываний
  WatchDog_reset();                     // Сбрасываем вачдог  
  Timer_0_5_Counter++;                  // Инкрементируем таймер
  if (Timer_0_5_Counter == 7)           // Если интервал между сообщениями больше 3,5 char
  {         
    switch (eRcvState)                                          // Анализируем состояние
    {
      case STATE_RX_INIT:                                       // Если приемник в состоянии инициализации (стартовое состояние)
        break;                                                  // Ни чего не делаем
      case STATE_M_RX_INIT:
        USART_ReceiveData(USART1);
        break;
      case STATE_RX_RCV:                                        // Если приемник был в режиме приема сообщения                                        
        ParseATURecieverFrameBuffer(Timer_NOK_Counter-1);       // Парсим сообщение
        break;
      case STATE_RX_ERROR:
        break;
      case STATE_M_RX_ERROR: 
        break;        
    }       
    eRcvState = STATE_RX_IDLE;                                  // Переводим приемник в режим подготовки к приему
    if (!RespondTimeOut)                                        
    {
      eSndState  = STATE_TX_IDLE;       
      TIM1->CR1 &= ~TIM_CR1_CEN;                                // Останавливаем счёт
      TIM1->CNT  = 0;        
      Timer_NOK_Counter = 0;
    } 
  }
  __enable_irq ();
  if (Timer_0_5_Counter == 3) 
  {
    Timer_NOK_Counter++;  
  }
  return;
}

/*
void SetPeriod(unsigned long ms)
{
  unsigned long T,T1,T3;
  T = GetPeriodT();
  T1 = (unsigned int)((ms*0.001)/(0.000025));
  if (T1>=T)
  {
    T3= T1/T + 1;
  } 
}
*/