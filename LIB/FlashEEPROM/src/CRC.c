#include "stm32f10x.h"
#include "stm32f10x_crc.h"
#include "CRC.h"


uint32_t crc32_32block(uint32_t *data, uint32_t length)
{
  uint32_t i = 0;
  
  CRC->CR = CRC_CR_RESET;                       // —брасываем CRC модуль
  for(i=0;i<length;i++) CRC->DR = data[i];      // —читаем последовательность (аккумул¤ци¤ автоматическа¤)
  return (CRC->DR);                             // ¬озвращаем посчитанное значение
}

uint32_t crc32_8block(uint8_t *data, uint32_t length)
{
  uint32_t i = 0;
  
  CRC->CR = CRC_CR_RESET;                       // —брасываем CRC модуль
  for(i=0;i<length;i++) CRC->DR = (uint32_t)data[i];      // —читаем последовательность (аккумул¤ци¤ автоматическа¤)
  return (CRC->DR);                             // ¬озвращаем посчитанное значение
}

void CRC_init(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
  return;
}

