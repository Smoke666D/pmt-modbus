//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "init.h"
#include "USART.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "system.h"
#include "register.h"
#include "mb.h"

#if (device == BIC)
  #include "BIC.h"
#endif
#if (device == BKC)
  #include "BKC.h"
#endif
#if (device == BSS)
  #include "BSS.h"
#endif
#if (device == BIG)
  #include "BIG.h"
#endif
#if (device == IT14)
  #include "IT14.h"
#endif
#if (device == TSP_SH)
  #include "TSP_SH.h"
#endif

#if (device == BKC01)
  #include "BKC01.h"
#endif

//***************************Global STRUCT*************************************
//static unsigned long CountConstant;
//static uint8_t temp_hse=0;
//*****************************************************************************
//***************************Initialization************************************

//*****************************************************************************

uint32_t rcc_init(void)
{ 
  RCC_DeInit();
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);                          //Включаем HSE
  /* Ждем пока HSE не выставит бит готовности либо не выйдет таймаут*/
   do 
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while( (HSEStatus == 0) && (StartUpCounter != HSEStartUp_TimeOut));  
  if ( (RCC->CR & RCC_CR_HSERDY) != RESET)  HSEStatus = (uint32_t)0x01;
  else  HSEStatus = (uint32_t)0x00;
  if ( HSEStatus == (uint32_t)0x01)                             // Если HSE запустился нормально
  {  
    FLASH->ACR |= FLASH_ACR_PRFTBE;                             // Включаем буфер предвыборки FLASH 
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);     // Конфигурируем Flash на 2 цикла ожидания
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;                // Это нужно потому, что Flash не может работать на высокой частоте 
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;                  // HCLK = SYSCLK    
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;                 // PCLK2 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;                 // PCLK1 = HCLK 
    /* Конфигурируем множитель PLL configuration: PLLCLK = HSE * 9 = 72 MHz     */
    /* При условии, что кварц на 8МГц!                                          */
    /* RCC_CFGR_PLLMULL9 - множитель на 9. Если нужна другая частота, не 72МГц  */
    /* то выбираем другой множитель.                                            */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));                                        
    RCC->CFGR |= (uint32_t)((uint32_t)0x00010000 | RCC_CFGR_PLLMULL9);
    RCC->CR |= RCC_CR_PLLON;                                            // Включаем PLL
    while((RCC->CR & RCC_CR_PLLRDY) == 0) {}                            // Ожидаем, пока PLL выставит бит готовности  
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));                  // Выбираем PLL как источник системной частоты
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;                             // Выбираем PLL как источник системной частоты
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08) {}   // Ожидаем, пока PLL выберется как источник системной частоты 
    
  }
  else 
  { 
   /* Все плохо... HSE не завелся... Чего-то с кварцем или еще что...
      Надо бы как то обработать эту ошибку... Если мы здесь, то мы работаем
      от HSI! */
  }
  return HSEStatus;
}


//Функция инициализации периферии
void Init()
{
  __disable_irq ();             // Запрещаем глобально прерывания
  AFIO->MAPR |= 0x2000000;
  rcc_init();                   // Инициализируем тактирование
  gpio_init();                  // Инициализируем пины 
  
  MBRegisterInit();
  TIM1_init();                  // Инициализируем таймер 1
                  
  #if (device == BIG) 
    TIM3_init();                // Инициализируем таймер 3
  #endif
  
  
  TIM2_init();                  // Инициализируем таймер 2
  USART1_init();                // Инициализируем USART
  
  
  #if (device == BIC) 
    BIC_Init();                 // Инициализируем пины имитатора + USART2
  #endif
  

  #if (device == BKC)
    BKC_Init();
  #endif
    
  #if (device == BKC)
    BKC_Init();
  #endif   
    
  #if (device == IT14)
    IT14_Init();
  #endif     
  
  #if (device == TSP_SH)
    TSP_SH_Init();
  #endif    
    
  #if (device == BKC14)
    BKC14_Init();
  #endif
    
  #if (device == KMG18)
    KMG18_Init();
  #endif  
    
  #if (device == BKC01)
    BKC01_Init();
  #endif
    
  #if (WatchDog_OnOff == 1)     // Если разрешена работа ВачДога
    WatchDog_start();           // Инициализируем ВачДог
    WatchDog_reset();           // Сбрасываем ВачДог    
  #endif                        // Закончили работу с ВачДогу
          
  __enable_irq ();              // Разрешаем глобально прирывания
  
  InitMB();
  
  
  #if (device == BKC01)
    BKC01_FunInit();
  #endif
    
   
   
  
  return;
}



void gpio_init(void)
{  
  //*****************************Struct*****************************************
  GPIO_InitTypeDef         GPIO_InitStruct;               // Структура инициализирующих данных GPIO
  //****************************************************************************
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Включаем тактирование GPIOА
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Включаем тактирование GPIOB
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // Включаем тактирование GPIOC
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE); // Включаем тактирование альтернативных функций
  //********************************GPIO****************************************
  //Инициализации вывода USART1_Tx
  GPIO_InitStruct.GPIO_Pin   = RS485_TX_pin;            // Настройки вывода 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // Скорость порта максимальная
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;         // Режим альтернативной функции, выход Push-Pull
  GPIO_Init(RS485_port, &GPIO_InitStruct);              // Заданные настройки сохраняем в регистрах  
  //Инициализации вывода USART1_Rx
  GPIO_InitStruct.GPIO_Pin   = RS485_RX_pin;            // Настройки вывода 
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;           // GPIO_Mode_IN_FLOATING
  GPIO_Init(RS485_port, &GPIO_InitStruct);              // Заданные настройки сохраняем в регистрах
  DOUT_Init(RS485_DE_pin, RS485_port);
  RS485_nRE;
  
  
   //******************************7-Seg****************************************
  //Инициализации вывода A-DP
  GPIO_InitStruct.GPIO_Pin   = IND_A | IND_B | IND_C | IND_D | IND_E | IND_F | IND_G | IND_DP;  //Настройки вывода
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;                                                //Режим выхода
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                                                //Скорость порта максимальная
  GPIO_Init(INDPort, &GPIO_InitStruct);                                                         //Заданные настройки сохраняем в регистрах  
  //Инициализации вывода SEG1-4
  GPIO_InitStruct.GPIO_Pin   = SEG1 | SEG2;       //Настройки вывода
  //  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;               //Режим выхода
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                  //Режим альтернативной функции, выход Push-Pull
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                //Скорость порта максимальная
  GPIO_Init(SEGPort, &GPIO_InitStruct);                         //Заданные настройки сохраняем в регистрах
  //*************************DIP_SWITCH*****************************************
  #if ( DSW_ENB > 0 )
    #if ( DSW_SHORT == 0 )
      GPIO_InitStruct.GPIO_Pin   = DSW8;                    //Настройки вывода
      GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;           //Режим выхода
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        //Скорость порта максимальная
      GPIO_Init(GPIOA, &GPIO_InitStruct);     
      GPIO_InitStruct.GPIO_Pin   = DSW7|DSW6|DSW5|DSW4|DSW3|DSW2|DSW1;            // Настройки вывода
      GPIO_Init(GPIOB, &GPIO_InitStruct);                                         // Заданные настройки сохраняем в регистрах  
      //AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;                                // Ремапип пины, относящиеся к джетагу
    #else
      GPIO_InitStruct.GPIO_Pin   = DSW4|DSW3|DSW2|DSW1;
      GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStruct);
    #endif
  #endif
  //*************************REMAP**********************************************
  if ((RS485_TX_pin == GPIO_Pin_6) && (RS485_RX_pin == GPIO_Pin_7))     // Если ноги под ремап
  {
    AFIO->MAPR |=  AFIO_MAPR_USART1_REMAP;                              // Ремапим ноги
  }   
  //****************************************************************************
  return;
}
//Запускаем предделитель на 25мкс

void TIM1_init(void)
{ 
  //****************************************************************************
  #define  MinTicPeriod         0.000025  //25mks 
  #define  Temp_Preddelitelja   (uint16_t)(MinTicPeriod*SystemCoreClock);
  #define  Trebuemi_interval    0.00025 //В секундах
  //*******************************Var******************************************
  float CHAR_0_5_INTERVAL = 0;
  float Temp_Delitelja    = 0;
  //*****************************Struct*****************************************
  RCC_ClocksTypeDef     RCC_Clock;
  //****************************************************************************
  if (ReadUserRegister(0) > UART_14400 )                                        // Если скорость канала выше 14400, таймаут между пакетами одинаковый
  {
    CHAR_0_5_INTERVAL  = 0.00025;                                               // Выставляем интервал "пол-чар"
  }
  else                                                                          // При скорости канала меньше 14400...
  { 
    CHAR_0_5_INTERVAL  = (5/(float)BoundRateTable[ReadUserRegister(0)]);        // Высчитываем интервал
  }
  Temp_Delitelja = (CHAR_0_5_INTERVAL/MinTicPeriod);                            // Высчитываем делитель
  RCC_GetClocksFreq(&RCC_Clock);                                                // Получаем значения тактовых частот шин 
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);                        // Тактирование таймера TIM1
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                                           // Включаем тактирование таймера TIM1 
  TIM1->PSC     = (uint16_t)(RCC_Clock.PCLK2_Frequency*MinTicPeriod-1);         // Настройка предделителя таймера
  //TIM1->PSC     = Temp_Preddelitelja;
  TIM1->ARR     = (uint32_t)Temp_Delitelja;                                     // Загружаем число миллисекунд в регистр автоперезагрузки
  TIM1->DIER   |= TIM_DIER_UIE;                                                 // Разрешаем прерывание при переполнении счетчика
  //TIM1->CR1  |= TIM_CR1_CEN;                                                  // Запускаем счет
   TIM1->CR1   &= ~TIM_CR1_CEN;                                                 // Останавливаем счёт
  // TIM1->CNT = 0; 
  // Счёт запускаем по приёму 1 байта
  NVIC_SetPriority(TIM1_UP_IRQn, 1);                                            // Выставляем саксимальный приоритет прерывания
  NVIC_EnableIRQ(TIM1_UP_IRQn);                                                 // Включаем прерывания по переполнению
  //CountConstant = (uint32_t)Temp_Delitelja;
  return;
}


unsigned long GetPeriodT()
{
  return TIM1->ARR;
}


void TIM2_init(void)
{   
  //GPIO_InitTypeDef              GPIO_InitStruct;        // Структура инициализирующих данных GPIO
  //TIM_TimeBaseInitTypeDef       TIM_BaseConfig;         // Структура инициализации часов
  //TIM_OCInitTypeDef             TIM_OCConfig;           // Конфигурация выхода таймера
  
  #define  Period_t2_Preddel      0.0025                                          // 25mks   
  #define  Temp_T2_Preddelitelja  (uint16_t)(Period_t2_Preddel*SystemCoreClock);  
  #define  Period_t2_Out          0.0025                                          // В секундах
  #define  Temp_T2_Out            (uint16_t)(Period_t2_Out/Period_t2_Preddel);    
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                      // Тактирование таймера TIM2  
  TIM2->CR1   = TIM_CR1_URS;                               // Сброс настроек 1
  TIM2->CR2   = 0;                                         // Сброс настроек 2
  TIM2->CNT   = 0;                                         // Сброс счетчика
  TIM2->PSC   = Temp_T2_Preddelitelja;                     // 18000 Настройка предделителя таймера
  TIM2->ARR   = Temp_T2_Out;                               // 800 Загружаем число миллисекунд в регистр автоперезагрузки
  TIM2->SR    = 0;                                         // Сброс статусов
  TIM2->DIER |= TIM_DIER_UIE;                              // Разрешаем прерывание при переполнении счетчика
  NVIC_SetPriority(TIM2_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  // Выставляем приоритет прерывания
  NVIC_EnableIRQ(TIM2_IRQn);                               // Включаем прерывания по переполнению
  TIM2->EGR   = TIM_EGR_UG;                                // Считаем вверх 
  TIM2->CR1  |= TIM_CR1_CEN;                               // Запускаем счет
  return;
}

void TIM3_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_BaseConfig; 
    TIM_OCInitTypeDef TIM_OCConfig;                             // Конфигурация выхода таймера
    // Запускаем таймер на тактовой частоте в 72000 kHz
    TIM_BaseConfig.TIM_Prescaler = 43;                          //(uint16_t) (SystemCoreClock / 7200) - 1;// 
    TIM_BaseConfig.TIM_ClockDivision = 0;
    TIM_BaseConfig.TIM_Period = 1670;                           // ****** //23907;     //Частота шима 100Гц
    
    TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;        // Отсчет от нуля до TIM_Period
    TIM_TimeBaseInit(TIM3, &TIM_BaseConfig);                    // Инициализируем таймер №3 (его выходы как раз на порту B)
    
    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;                  // Конфигурируем выход таймера, режим - PWM1   
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;      // Собственно - выход включен
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;          // Полярность => пульс - это единица (+3.3V)
    
    TIM_OCConfig.TIM_Pulse = 0;                                 // Пульс длинной xxx тактов => xxx/233 = xx%     
    TIM_OC3Init(TIM3, &TIM_OCConfig);                           // Инициализируем 3 выход таймера №3 ( это PB0)

    // Конфигурируем второй выход таймера
    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;  
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;          // TIM_OCPolarity_Low;// Ради эксперемента, меняем полярность (пульс - 0V).   
    TIM_OCConfig.TIM_Pulse = 0;                                 // Пульс длинной 30 тактов => 30/150 = 20%
    TIM_OC4Init(TIM3, &TIM_OCConfig);                           // Инициализируем 4 выход таймера №3 (PB1)

    TIM3->DIER |= TIM_DIER_UIE;                                 //Разрешаем прерывание при переполнении счетчика
  //TIM1->CR1 |= TIM_CR1_CEN;//Запускаем счет
  //Счёт запускаем по приёму 1 байта
  NVIC_SetPriority(TIM3_IRQn, 1); 
  NVIC_EnableIRQ(TIM3_IRQn);                         // Включаем прерывания по переполнению
  
    
    
 //   Как я понял - автоматическая перезарядка таймера, если неправ - поправте.
 //   TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
 //   TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
 //   TIM_ARRPreloadConfig(TIM3, ENABLE);
 //   Это необходимость буферизировать мгновенное состояние счетчика по некому событию,
 //   дабы потом считать его не останавливая таймер от его работы.
 //   В данном примере не несет никакой пользы и можно пропустить. 

    // Включаем таймер
    TIM_Cmd(TIM3, ENABLE);

  return;
}


void TIM4_init(void)
{   

  #define  Period_t4_Preddel  0.00025  //250mks   
  #define  Temp_T4_Preddelitelja  (uint16_t)(Period_t4_Preddel*SystemCoreClock); 
  #define  Period_t4_Out 0.0005 //В секундах
  #define  Temp_T4_Out  (uint16_t)(Period_t4_Out/Period_t4_Preddel);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    //Тактирование таймера TIM2 // так не работает почему?   RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  TIM4->CR1 = TIM_CR1_URS;
  TIM4->CR2 = 0;
  TIM4->CNT = 0; 
  TIM4->PSC = Temp_T4_Preddelitelja; //18000//Настройка предделителя таймера
  TIM4->ARR = Temp_T4_Out; //800 Загружаем число миллисекунд в регистр автоперезагрузки
  TIM4->SR  = 0;
  TIM4->DIER |= TIM_DIER_UIE; //Разрешаем прерывание при переполнении счетчика
  NVIC_SetPriority(TIM4_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
  NVIC_EnableIRQ(TIM4_IRQn);  // Включаем прерывания по переполнению
  TIM4->EGR = TIM_EGR_UG; 
  TIM4->CR1 |= TIM_CR1_CEN;//Запускаем счет
  return;
}

/*init_TIM3*/


