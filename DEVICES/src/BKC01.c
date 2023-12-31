//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "stm32f10x.h"
#include "BKC01.h"
#include "I2C.h"
#include "HD44780.h"
#include "Register.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "system.h"
//******************************************************************************
//*******************************GLOBAL VAR*************************************
//******************************************************************************
static uint16_t         SysReg       = 0;
static uint8_t          LastEvent    = 0;
static uint32_t         SW_timer     = 0;       // ������� ��������� ������
static uint32_t         SW_HoldTimer = 0;
static uint8_t          SW_acc       = 0;       // ������ ��������� ������
static uint8_t          BIP_count    = 0;       // ������� ������
static uint8_t          BIP_num      = 0;       // ���-�� ������
static uint8_t          BIP_state    = 0;       // 0 - �����, 1 - ������
static uint16_t         LED_Br       = 0xFFFF;  // ������� ��������� �������
//*****************************��������*****************************************
static uint32_t         Change_time = 0;        // ������� �������, ���������� � ���������� ��������� ������� 
static uint32_t         Ltime       = 0;        // ��������� ����� 
static uint8_t          SW_ActCount = 0;        // ������� ����������� �������� ��� ��������� ������
static uint8_t          SW_blockAct = 0;
static uint32_t         BIP_time    = 0;        // ������� ������� ����
static uint32_t         LED_time    = 0;
static uint32_t         LCD_time    = 0;
//******************************�������*****************************************
static float            SPtr            = 0.0;      // �� ��� ���������� 
static float            SPtrTemp        = 0.0;      // �� ��� ���������� 
static uint32_t         SetPointPos     = 0;        // ��� ������ ����� �������� �������
static uint32_t         SetPointPosTemp = 0;        // ��� ������ ����� �������� �������
//******************************************************************************
//*********************************CONST****************************************
//******************************************************************************
static const float SetPointVal[SumUpSetPointNom] = 
{
  1E-8, 2E-8, 3E-8, 4E-8, 5E-8, 6E-8, 7E-8, 8E-8, 9E-8, 
  1E-7, 2E-7, 3E-7, 4E-7, 5E-7, 6E-7, 7E-7, 8E-7, 9E-7,
  1E-6, 2E-6, 3E-6, 4E-6, 5E-6, 6E-6, 7E-6, 8E-6, 9E-6,
  1E-5, 2E-5, 3E-5, 4E-5, 5E-5, 6E-5, 7E-5, 8E-5, 9E-5,
  1E-4, 2E-4, 3E-4, 4E-4, 5E-4, 6E-4, 7E-4, 8E-4, 9E-4,
  1E-3, 2E-3, 3E-3, 4E-3, 5E-3, 6E-3, 7E-3, 8E-3, 9E-3,
  1E-2, 2E-2, 3E-2, 4E-2, 5E-2, 6E-2, 7E-2, 8E-2, 9E-2,
  1E-1, 2E-1, 3E-1, 4E-1, 5E-1, 6E-1, 7E-1, 8E-1, 9E-1,
  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0,
  11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0,
  21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0,
  31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0,
  41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 48.0, 49.0, 50.0,
  51.0, 52.0, 53.0, 54.0, 55.0, 56.0, 57.0, 58.0, 59.0, 60.0,
  61.0, 62.0, 63.0, 64.0, 65.0, 66.0, 67.0, 68.0, 69.0, 70.0,
  71.0, 72.0, 73.0, 74.0, 75.0, 76.0, 77.0, 78.0, 79.0, 80.0,
  81.0, 82.0, 83.0, 84.0, 85.0, 86.0, 87.0, 88.0, 89.0, 90.0,
  91.0, 92.0, 93.0, 94.0, 95.0, 96.0, 97.0, 98.0, 99.0, 100.0,
  101.0, 102.0, 103.0, 104.0, 105.0, 106.0, 107.0
}; // ������ ����� �������� �������, ������ �� ����������� ������
//******************************************************************************
//*******************************FUNCTIONS**************************************
//******************************************************************************
uint8_t ScanSwitch(void);               // ������������ ������� ������, ���������� ����� ������
uint8_t CheckSwitch(uint8_t sw);        // �������� ������������ ������� �����
void    ProcSw(void);                   // ����� ��� ������
void    Bip(uint8_t OnOff);             // ����� (1-���, 0-����)
void    Led(uint8_t OnOff);             // ��������� (1-���, 0-����)
void    TIM4_IRQHandler(void);          // ���������� ������� 4
//******************************************************************************
//******************************************************************************
//******************************************************************************
uint8_t ScanSwitch(void)
{
  uint8_t input = 0;
  //**********************READ SW************************
  if (!KL1_READ)
  {
    input |= 0x01;
  }
  if (!KL2_READ)
  {
    input |= 0x02;
  }
  if (!KL3_READ)
  {
    input |= 0x04;
  }
  if (!KL4_READ)
  {
    input |= 0x08;
  }
  if (!KL5_READ)
  {
    input |= 0x10;
  }
  //*******************SWITCH PARSING********************
  switch (input)
  {
    case 0x00:          // �� ������ �� ���� ������
      return 0;
    case 0x01:
      return 0x01;      // 1-�� ������
    case 0x02:
      return 0x02;      // 2-�� ������
    case 0x04:
      return 0x03;      // 3-�� ������
    case 0x08:
      return 0x04;      // 4-�� ������
    case 0x10:
      return 0x05;      // 5-�� ������
    default:
      return 0x0F;      // ������������� ������� - ������
  }
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
uint8_t CheckSwitch(uint8_t sw)
{
  uint8_t out = 0;
  
  if (SW_acc != sw)             // ���� ������ ����� ������
  {
    SW_acc   = sw & SW_msk;     // ���������� �� � �����������, �������� ������ ������
    out      = 0;               // ���������� � ����� ������ ��������
    SW_timer = 0;               // ���������� �������
  }
  else                          // ���� ������ �������������
  {
    if (SW_timer > KeyDownTimout)// ���� ������� �� �������� �������...
    {
      out = SW_acc;             // �������, ��� ������ ���������
    }
    if (SW_timer > KeyHoldTimeout)// ���� ������� �� ���������...
    {
      out |= SW_Hold;           // ��������, ��� ������ �������������
    }
  }
  return out;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
void ProcSw(void)
{
  Ltime   = 0;                  // ������ ����� ��� ������������������ ������ (�����)
  SysReg |= SysRegBIPx1;        // ������� ������
  Led(1);                       // ��� ���������
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
void Bip(uint8_t OnOff)
{
  if (OnOff)
  {
    //PWM_tim->CCR4 = ZUMMER_freq;
    PWM_tim->CCR4 = PWM_tim->CNT/2;
  }
  else
  {
    PWM_tim->CCR4 = 0x0000;
  }
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
void Led(uint8_t OnOff)
{
  if (OnOff)
  {
    //DOUT_SET(LED_port,LED_pin); // �������� ���������
    PWM_tim->CCR3 = LED_Br;
    SysReg |= SysRegLed;        // ���������� ���� ��� ��������� ��������
  }
  else
  {
    //DOUT_RESET(LED_port,LED_pin);
    PWM_tim->CCR3 = 0;
  }
  return;
}
//******************************************************************************
uint32_t uFloat2pos ( float input )
{
  uint32_t res = 0U;
  uint32_t i   = 0U;
  if ( input <= SetPointVal[0U] )
  {
    res = 0U;
  }
  else if ( input >= SetPointVal[SumUpSetPointNom - 1] )
  {
    res = SumUpSetPointNom - 1U;
  }
  else
  {
    for ( i=1U; i<SumUpSetPointNom; i++ )
    {
      if ( ( SetPointVal[i-1U] < input ) && ( SetPointVal[i] >= input ))
      {
        res = i;
      }
    }
  }
  return res;
}
//******************************************************************************
uint32_t uInitSetPoint ( void )
{
  uint16_t buf[2U] = { 0U };
  float    temp    = 0.0f;
  buf[0U] = ReadHolding(memory1Adr);
  buf[1U] = ReadHolding(memory0Adr);
  WriteShortToHolding( SetPoint1Adr, buf[0U] );
  WriteShortToHolding( SetPoint0Adr, buf[1U] );
  temp = *( float* )( buf );
  return uFloat2pos( temp );
}
//******************************************************************************
uint32_t uReadSetPoint ( void )
{
  uint16_t buf[2U] = { 0U };
  float    temp    = 0.0f;
  buf[0U] = ReadHolding(SetPoint1Adr);
  buf[1U] = ReadHolding(SetPoint0Adr);
  WriteShortToHolding( memory1Adr, buf[0U] );
  WriteShortToHolding( memory0Adr, buf[1U] );
  temp = *( float* )( buf );
  return uFloat2pos( temp );
}
//******************************************************************************
void vWriteSetPoint ( float* data )
{
  uint16_t buf[2U] = { 0U };
  *( float* )buf = *data;
  WriteShortToHolding( SetPoint1Adr, buf[0U] );
  WriteShortToHolding( SetPoint0Adr, buf[1U] );
  WriteShortToHolding( memory1Adr, buf[0U] );
  WriteShortToHolding( memory0Adr, buf[1U] );
  return;
}
//******************************************************************************
//*******************************INIT*******************************************
//******************************************************************************
void BKC01_Init(void)
{
  GPIO_InitTypeDef              GPIO_InitStruct;        // ��������� ���������������� ������ GPIO
  TIM_OCInitTypeDef             TIM_OCConfig;           // ������������ ������ �������
  TIM_TimeBaseInitTypeDef       TIM_BaseConfig;         // ��������� ������������� �����
  //********************************RCC*****************************************
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB1ENR |= RCC_APB1Periph_TIM2;
  //******************************DIN-DOUT**************************************
  DIN_Init(KL1_pin,KL1_port);           // ������������� ������
  DIN_Init(KL2_pin,KL2_port);           // ������������� ������
  DIN_Init(KL3_pin,KL3_port);           // ������������� ������
  DIN_Init(KL4_pin,KL4_port);           // ������������� ������
  DIN_Init(KL5_pin,KL5_port);           // ������������� ������
  DIN_Init(KEY_pin,KEY_port);           // ������������� ������  
  //DOUT_Init(LED_pin,LED_port);          // ��������� ������������� ��������� �� �������
  //****************************************************************************
  //*********************************PWM****************************************
  //****************************************************************************
  //********************************GPIO****************************************
  GPIO_InitStruct.GPIO_Pin     = ZUM_pin;               // ��������� ������ PB11
  GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_AF_PP;       // ����� �������������� �������
  GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_50MHz;      // �������� ����� ������������
  GPIO_Init(ZUM_port, &GPIO_InitStruct);                // �������� ��������� ��������� � ��������� GPIOB
  AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP;                   // ������� ������ CH1-PA15,CH2-PB3, CH3-PB10, CH4-PB11
  //***************************LCD_CONTRAST*************************************
  GPIO_InitStruct.GPIO_Pin     = VO_pin;                // ��������� ������ PB11
  GPIO_Init(VO_port, &GPIO_InitStruct);                 // �������� ��������� ��������� � ��������� GPIOB
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;          // ������� ��� �������
  //*****************************LCD_LED****************************************
  GPIO_InitStruct.GPIO_Pin     = LED_pin;               // ��������� ������ PB11
  GPIO_Init(LED_port, &GPIO_InitStruct);                // �������� ��������� ��������� � ��������� GPIOB
  //******************************TIM_2*****************************************
  TIM_OCStructInit(&TIM_OCConfig);                      // ���������� ������������� ��������� �������������
  TIM_TimeBaseStructInit(&TIM_BaseConfig);              // ���������� ������������� ��������� �������������
  TIM_BaseConfig.TIM_Prescaler     = 720;               // ������������ 1 
  TIM_BaseConfig.TIM_ClockDivision = TIM_CKD_DIV1;      // �������� ������ 1
  TIM_BaseConfig.TIM_Period        = 100;              // ������� ����
  TIM_BaseConfig.TIM_CounterMode   = TIM_CounterMode_Up;// ������ �� ���� �� TIM_Period
  TIM_TimeBaseInit(PWM_tim, &TIM_BaseConfig);           // �������������� ������ �2
  //******************************TIM_CH****************************************
  TIM_OCConfig.TIM_OCMode      = TIM_OCMode_PWM1;       // ������������� ����� �������, ����� - PWM1   
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;// ���������� - ����� �������
  TIM_OCConfig.TIM_OCPolarity  = TIM_OCPolarity_High;   // ���������� => ����� - ��� ������� (+3.3V)
  TIM_OCConfig.TIM_Pulse       = 0x0000;                // ����� �������
  TIM_OC4Init(PWM_tim, &TIM_OCConfig);                  // �������������� 4 ����� �������
  TIM_OCConfig.TIM_Pulse       = 0;                     // ����� �������
  TIM_OC1Init(PWM_tim, &TIM_OCConfig);                  // �������������� 1 ����� �������
  TIM_OC3Init(PWM_tim, &TIM_OCConfig);                  // �������������� 3 ����� �������
  TIM_CtrlPWMOutputs(PWM_tim, ENABLE);                  // �������� ��� ������  
  TIM_Cmd(PWM_tim, ENABLE);                             // �������� ������
  //****************************************************************************
  //********************************TIM4****************************************
  //****************************************************************************
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;                   // ������������ ������� TIM4
  TIM4->CR1     = TIM_CR1_URS;                          // ����� �������� 1
  TIM4->CR2     = 0;                                    // ����� �������� 2
  TIM4->CNT     = 0;                                    // ����� ��������
  TIM4->PSC     = 72;                                   // ��������� ������������ �������
  TIM4->ARR     = 100;                                  // ��������� ����� ����������� � ������� ����������������
  TIM4->SR      = 0;                                    // ����� ��������
  TIM4->DIER   |= TIM_DIER_UIE;                         // ��������� ���������� ��� ������������ ��������
  NVIC_EnableIRQ(TIM4_IRQn);                            // �������� ���������� �� ������������
  TIM4->EGR     = TIM_EGR_UG;                           // ������� ����� 
  TIM4->CR1    |= TIM_CR1_CEN;                          // ��������� ����
  //****************************************************************************
  //****************************************************************************
  //****************************************************************************
  return;
}

void BKC01_FunInit (void)
{
  HD_init();                                            // �������������� �������
  PWM_tim->CCR1   = (PWM_tim->CNT)*(ReadHolding(LCD_ContrastAdr) & 0xF)/0xF;
  LED_Br          = (PWM_tim->CNT)*(ReadHolding(LCD_BrightAdr)   & 0xF)/0xF;
  SetPointPos     = uInitSetPoint();
  SetPointPosTemp = SetPointPos;
  ResetHoldingModifyFlag(); 
  SPtr = SetPointVal[SetPointPos];                      // �������������� �������
  SPtrTemp = SPtr;
  HD_Send_Float(SPtr);                                  // ������� �� ������� ����������� �������
  return;
}


//******************************************************************************
//********************************FSA*******************************************
//******************************************************************************
//       |---|
//       | 1 |
//       |---|
// |---| |---| |---|
// | 3 | | 5 | | 4 |   ������������ ������ �� �������
// |---| |---| |---|
//       |---|
//       | 2 |
//       |---|
void BKC01_FSA(void)
{
  uint8_t  SwTrg   = 0;                    // ����������� ������
  
  if (GetHoldingModifyFlag())
  {
    SetPointPos     = uReadSetPoint();
    SetPointPosTemp = SetPointPos;                      // �������� ��������� �������
    SPtr            = SetPointVal[SetPointPos];         // ��������� �������� �������
    SPtrTemp        = SPtr;                             // �������� ��������� �������
    HD_Send_Float(SPtr);                                // ������� �� ������� ����������� �������
    //vWriteSetPoint( &SPtr );
    PWM_tim->CCR1   = (PWM_tim->CNT)*(ReadHolding(LCD_ContrastAdr) & 0xF)/0xF;
    LED_Br          = (PWM_tim->CNT)*(ReadHolding(LCD_BrightAdr)   & 0xF)/0xF;
    
    ResetHoldingModifyFlag();                           // ���������� ���� ������ ����� ������
  }
  else
  {
    if (!KEY_READ)                              // ���� ���� ��������
    {
      if (!(SysReg & SysRegKey))                // ���� ������ ���������
      {
        Led(1);                                 // �������� ���������
        //����� ��������� �������
        SysReg |= SysRegKey;                    // ��������, ��� ���� ������������� � ���������
      }
      SwTrg   = CheckSwitch(ScanSwitch());      // ��������� ������� ������ � �������� ������� ���������
      if(LastEvent != SwTrg)                    // ���� ���������� ������� �� ������������
      {
        LastEvent = SwTrg;                      // ���������� ����� �������
        SysReg   |= SysRegSwDone;               // ���������� ������ ������ �������
      }
      if ( ( ( LastEvent & SW_msk ) == 0x0F ) &&
           ( ( SwTrg     & SW_msk ) != 0    ) &&
           ( ( SwTrg     & SW_msk ) != 0x0F ) )
      {
        SwTrg = 0x0A;
      }
      switch( SwTrg )                                     // ��������� ������
      {
        case 0x00:                                      // ��������� ������ �� ���������
          break;
        case 0x0F:                                      // ������ �������������� �������
          if (SysReg & SysRegSwDone)
          {
            SysReg |= SysRegBIPx3;                      // �������
            SysReg &= ~SysRegSwDone;
          }
          break;
        //**********************************************************************  
        case 0x01:                                      // ������ �1 - �����
          if (SysReg & SysRegSwDone)                    // ���� ��������� �� ���� ���������
          {
            if (SysReg & SysRegBlink)                   // ���� ����� ��������� �������
            {
              SW_ActCount = 0;
              SW_blockAct = 0;
              SysReg &= ~SysRegBlink;                   // ��������� �������
              if(SetPointPosTemp < (SumUpSetPointNom-1))// ���� ���� ���� ����������� �������
              {
                Ltime   = 0;                            // ������ ����� ��� ������������������ ������ (�����)
                SysReg |= SysRegBIPx1;                  // ������� ������
                Led(1);                                 // ��� ���������
                SetPointPosTemp++;                      // �������������� �������
                SPtrTemp = SetPointVal[SetPointPosTemp];// ��������� �������
                SysReg  |= SysRegChange;                // ������ ������ ��������� �������
                Change_time = 0;                        // ���������� ������� ������� ���������� ���������
                HD_Send_Float(SPtrTemp);                // ������� ������� �� �������
                SysReg |= SysRegBlink;                  // ��������� �������
              }
              else
              {
                Ltime   = 0;                            // ������ ����� ��� ������������������ ������ (�����)
                SysReg |= SysRegBIPx2;                  // ������� ������
                Led(1);                                 // ��� ���������
              }
            }
            else
            {
              Ltime   = 0;                              // ������ ����� ��� ������������������ ������ (�����)
              SysReg |= SysRegBIPx1;                    // ������� ������
              Led(1);                                   // ��� ���������
            }
            SysReg &= ~SysRegSwDone;                    // ������� ���� ������ �������
          }
          break;
        case 0x11:                                      // ������ �1 � ���������� 
          if (((SW_ActCount < ShortToLongCount) && (SW_HoldTimer > KeyHoldLongTimeout)) || ((((SW_ActCount+1) > ShortToLongCount) && (SW_HoldTimer > KeyHoldShortTimeout))))
          {
            SW_ActCount++;
            SW_HoldTimer = 0;
            Ltime   = 0;                                // ������ ����� ��� ������������������ ������ (�����)
            Led(1);                                     // ��� ���������
            if(SetPointPosTemp < (SumUpSetPointNom-1))  // ���� ���� ���� ����������� �������
            {
              SetPointPosTemp++;                        // �������������� �������
              SPtrTemp = SetPointVal[SetPointPosTemp];  // ��������� �������
              SysReg  |= SysRegChange;                  // ������ ������ ��������� �������
              Change_time = 0;                          // ���������� ������� ������� ���������� ���������
              HD_Send_Float(SPtrTemp);                  // ������� ������� �� �������
              SysReg |= SysRegBlink;                    // ��������� �������
            }
            else if (!SW_blockAct)
            {
              Ltime   = 0;                              // ������ ����� ��� ������������������ ������ (�����)
              SysReg |= SysRegBIPx2;                    // ������� ������
              Led(1);                                   // ��� ���������
              SW_blockAct = 1;
            }
          }
          break;
        //**********************************************************************  
        case 0x02:                                      // ������ �2 - ����
          if (SysReg & SysRegSwDone)                    // ���� ��������� �� ���� ���������
          {
            if (SysReg & SysRegBlink)                   // ���� ����� ��������� �������
            {
              SW_ActCount = 0;
              SW_blockAct = 0;
              SysReg &= ~SysRegBlink;                   // ��������� �������
              if(SetPointPosTemp > 0)                   // ���� ���� ���� ��������� �������
              {
                Ltime   = 0;                            // ������ ����� ��� ������������������ ������ (�����)
                SysReg |= SysRegBIPx1;                  // ������� ������
                Led(1);                                 // ��� ���������
                SetPointPosTemp--;                      // �������������� �������
                SPtrTemp = SetPointVal[SetPointPosTemp];// ��������� �������
                SysReg  |= SysRegChange;                // ������ ������ ��������� �������
                Change_time = 0;                        // ���������� ������� ������� ���������� ���������
                HD_Send_Float(SPtrTemp);                // ������� ������� �� �������
                SysReg |= SysRegBlink;                  // ��������� �������
              }
              else
              {
                Ltime   = 0;                            // ������ ����� ��� ������������������ ������ (�����)
                SysReg |= SysRegBIPx2;                  // ������� ������
                Led(1);                                 // ��� ���������
              }
            }
            else
            {
              Ltime   = 0;                              // ������ ����� ��� ������������������ ������ (�����)
              SysReg |= SysRegBIPx1;                    // ������� ������
              Led(1);                                   // ��� ���������
            }
            SysReg &= ~SysRegSwDone;                    // ������� ���� ������ �������
          }
          break;
        case 0x12:                                      // ������ �2 � ����������
          if (((SW_ActCount < ShortToLongCount) && (SW_HoldTimer > KeyHoldLongTimeout)) || ((((SW_ActCount+1) > ShortToLongCount) && (SW_HoldTimer > KeyHoldShortTimeout))))
          {
            SW_ActCount++;
            SW_HoldTimer = 0;
            Ltime        = 0;                           // ������ ����� ��� ������������������ ������ (�����)
            Led(1);                                     // ��� ���������
            if(SetPointPosTemp > 0)                     // ���� ���� ���� ����������� �������
            {
              SetPointPosTemp--;                        // �������������� �������
              SPtrTemp = SetPointVal[SetPointPosTemp];  // ��������� �������
              SysReg  |= SysRegChange;                  // ������ ������ ��������� �������
              Change_time = 0;                          // ���������� ������� ������� ���������� ���������
              HD_Send_Float(SPtrTemp);                  // ������� ������� �� �������
              SysReg |= SysRegBlink;                    // ��������� �������
            }
            else if (!SW_blockAct)
            {
              Ltime   = 0;                              // ������ ����� ��� ������������������ ������ (�����)
              SysReg |= SysRegBIPx2;                    // ������� ������
              Led(1);                                   // ��� ���������
              SW_blockAct = 1;
            }
          }
          break;
        //**********************************************************************  
        case 0x03:                                      // ������ �3 - �����
          if (SysReg & SysRegSwDone)                    // ���� ��������� �� ���� ���������
          {
            ProcSw();                                   // �������, ������ ��������� �������
            SysReg     &= ~SysRegBlinkStat;             // ��������� � �������
            SysReg     ^= SysRegBlink;                  // ������ ����� ��������� �������
            SysReg     &= ~SysRegSwDone;                // ������� ���� ������ �������
            Change_time = 0;                            // ���������� ������� ������� ���������� ���������
            if (!(SysReg & SysRegBlink))
            {
              SetPointPosTemp = SetPointPos;            // ������� ����� �������
              SPtrTemp = SPtr;                          // ������� ��������� �������
              HD_Send_Float(SPtr);                      // ������� ������� �� ���������
            }
          }
          break;
        case 0x13:                                      // ������ �3 � ����������
          break;
        //**********************************************************************  
        case 0x04:                                      // ������ �4 - ������
          if (SysReg & SysRegSwDone)                    // ���� ��������� �� ���� ���������
          {
            ProcSw();                                   // �������, ������ ��������� �������
            SysReg &= ~SysRegBlinkStat;                 // ��������� � �������
            SysReg ^= SysRegBlink;                      // ������ ����� ��������� �������
            SysReg &= ~SysRegSwDone;                    // ������� ���� ������ �������
            Change_time = 0;                            // ���������� ������� ������� ���������� ���������
            if (!(SysReg & SysRegBlink))                // ���� ��������� ����� �� ������ �������
            {
              SetPointPosTemp = SetPointPos;            // ������� ����� �������
              SPtrTemp = SPtr;                          // ������� ��������� �������
              HD_Send_Float(SPtr);                      // ������� ������� �� ���������
            }
          }
          break;
        case 0x14:                                      // ������ �4 � ����������
          break;
        //**********************************************************************  
        case 0x05:                                              // ������ �5 - �����
          if (SysReg & SysRegSwDone)                            // ���� ��������� �� ���� ���������
          {
            ProcSw();                                           // �������, ������ ��������� �������
            SetPointPos = SetPointPosTemp;
            SysReg &= ~SysRegBlink;                             // ��������� �������
            SPtrTemp = SetPointVal[SetPointPos];                    // ���������� � ������� �������
            SPtr = SPtrTemp;
            vWriteSetPoint( &SPtr );
            ResetHoldingModifyFlag();
            HD_Send_Float(SPtr);                                // ������� ������� �� ���������
            SysReg &= ~SysRegSwDone;                            // ������� ���� ������ �������
          }
          break;
        case 0x15:                                      // ������ �5 � ����������
          break;
      }
    } //if (KEY_READ) 
    else
    {
      if (SysReg & SysRegKey)
      {
        SysReg &= ~(SysRegKey | SysRegBlink);
        SetPointPosTemp = SetPointPos;            // ������� ����� �������
        SPtrTemp = SPtr;                          // ������� ��������� �������
        HD_Send_Float(SPtr);                      // ������� ������� �� ���������
        Led(0);
      }
    }
  }
  return;
}


void TIM4_IRQHandler(void)
{
  WatchDog_reset();
  if(TIM4->SR & TIM_SR_UIF)
  {
    __disable_irq ();
    //********************************SW****************************************
    SW_timer++;                 // ������� ����� ������� ������
    SW_HoldTimer++;
    //********************************LED***************************************
    if(SysReg & SysRegLed)
    {
      SysReg &= ~SysRegLed;
      LED_time = 0;
    }
    LED_time++;
    if (LED_time >= LED_TIMEOUT)
    {
      Led(0);
    }    
    //*******************************CHANGE*************************************
    Change_time++;
    if ((Change_time > WaitChTimeout) && (SysReg & SysRegBlink))
    {
      SysReg &= ~SysRegBlink;           // ������� �� ������ ���������
      SetPointPosTemp = SetPointPos;
      SPtrTemp = SPtr;
      HD_Send_Float(SPtr);              // ������� �� ������� ����������� �������
    }
    //*******************************LCD****************************************
    LCD_time++;
    if ((SysReg & SysRegBlink) && (LCD_time > LCD_TIMEOUT) && (SysReg & SysRegBlinkStat))
    {
      HD_WriteString("                ");      // ������� �������
      SysReg ^= SysRegBlinkStat;
      LCD_time = 0;
    }
    if ((SysReg & SysRegBlink) && (LCD_time > (LCD_TIMEOUT/1.5)) && (!(SysReg & SysRegBlinkStat)))
    {
      HD_Send_Float(SPtrTemp);                  // ������� ������� �� �������
      SysReg ^= SysRegBlinkStat;
      LCD_time = 0;
    }
    //*******************************BIP****************************************
    if (SysReg & SysRegBIPx1)
    {
      BIP_num   = 1;            // ������� ����� �� ������
      BIP_count = 0;            // �������� ������� ������
      SysReg &= ~SysRegBIPx1;   // ���������� ���� �������
      BIP_time  = 0;            // ���������� ������� ��������� �����/���������
      BIP_state = 0;            // �������� � �����
      Bip(1);                   // �������� ������
    }
    if (SysReg & SysRegBIPx2)
    {
      BIP_num   = 2;            // ������� ����� �� ����
      BIP_count = 0;            // �������� ������� ������
      SysReg &= ~SysRegBIPx2;   // ���������� ���� �������
      BIP_time  = 0;            // ���������� ������� ��������� �����/���������
      BIP_state = 0;            // �������� � �����
      Bip(1);                   // �������� ������
    }
    if (SysReg & SysRegBIPx3)
    {
      BIP_num   = 3;            // ������� ����� �� ����
      BIP_count = 0;            // �������� ������� ������
      SysReg &= ~SysRegBIPx3;   // ���������� ���� �������
      BIP_time  = 0;            // ���������� ������� ��������� �����/���������
      BIP_state = 0;            // �������� � �����
      Bip(1);                   // �������� ������
    }
    if (BIP_count < BIP_num)    // ���� �� ��������� ������ ���-�� ������
    {
      BIP_time++;                                       // �������������� ������� ��
      if((!BIP_state) && (BIP_time > BIP_TIMEOUT))      // ���� ���������� ����� ��������...
      {
        BIP_state = 1;                                  // ��������� � ����� ��������
        Bip(0);                                         // ���������� ������
        BIP_time = 0;                                   // ���������� �������
      }
      if((BIP_state) && (BIP_time > BIP_TIMEOUT))
      {
        BIP_state = 0;                                  // ��������� � ����� �����
        BIP_time = 0;                                   // ���������� �������
        BIP_count++;                                    // �������������� ���-�� ������
        if(BIP_count < BIP_num)                         // �������� ������ ����
        {       
          Bip(1);                                       // �������� ������
        }
      }
    }
    //******************HD****************************
    HD_Proc();    // ��������� �������
    //*****************************I2C****************
    //I2C_Proc();   // ��������� ����������
    //************************************************
    __enable_irq ();
  }
  TIM4->SR = 0;
  return;
}
//******************************************************************************
//******************************************************************************
//******************************************************************************