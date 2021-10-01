#include "stm32f10x.h"
#include "system.h"
#include "stm32f10x_rcc.h"
//****************************DELAY********************************************
void Delay_ms(uint32_t ms)
{
  volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);
  nCount=(RCC_Clocks.HCLK_Frequency/100000)*ms;
  for (; nCount!=0; nCount--);
}
//****************************WATCH DOG*****************************************  
void WatchDog_start(void)
{
  RCC->CSR |= RCC_CSR_LSION;     // ¬ключаем LSI - RC часы, тактирующие вачдог
  IWDG->KR = 0xCCCC;             // ¬ключаем программно вачдог
  return;
}
void WatchDog_reset(void)
{
  IWDG->KR = 0xAAAA;             // —брасываем счетчик ватчдога
  return;
}
//******************************************************************************