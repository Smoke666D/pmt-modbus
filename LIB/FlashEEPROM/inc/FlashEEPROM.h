
#ifndef __FlashEEPROM_H
#define __FlashEEPROM_H

#include "stm32f10x.h"
//****************************Flash Keys***************************************
#define         FLASH_KEY1              ((uint32_t)0x45670123)
#define         FLASH_KEY2              ((uint32_t)0xCDEF89AB)
//**************************Flash main data*************************************
#define         FlashSize               64                                                        // pages = Кб (для STM32F103С8)
#define         PageSize                1024                                                      // байт = 1 Кб = 0x400 (CONST для ARM Cortex M)
#define         FlashStartAdr           0x08000000                                                // Адресс начала флешь памяти, байт (CONST для ARM Cortex M)
#define         FlashEndAdr             ((uint32_t)(FlashStartAdr + FlashSize*PageSize - 1))      // Адресс конца флешь памяти (0x0800FFFF для 64 Кб)
#define         AdrCRC                  (PageSize - 4)
// Переменная под страницу будет выглядить, как:         uint8_t Data[PageSize]
//***************************EEPROM data****************************************
// Задаем последние страницы под EEPROM. 
// NB: Необходимо прописать ограничение использование Flash под ROM в Linker (EEPROMStartAdr - 1) 
#define         EEPROMSize              4                                                         // pages = Кб
#define         EEPROMFirstPage         (FlashSize-EEPROMSize-1)                                  // 
#define         EEPROMStartAdr          ((uint32_t)(FlashEndAdr - (EEPROMSize+1)*PageSize + 1))   // Адресс начала флешь памяти для EEPROM (0x0800EC00)
//****************************Function******************************************
uint8_t CheckPageCRC(uint8_t* PageData);
void FlashPageCRCcalc(uint8_t* PageData);

void FlashPageWrite(uint8_t page, uint8_t* data,unsigned short count);
void FlashPageRead(uint8_t page, uint8_t *output);
uint32_t FlashReadUint32(uint32_t adr);
uint16_t FlashReadUint16(uint32_t adr);
uint8_t FlashReadUint8(uint32_t adr);
void BufferErase(uint8_t *data, uint16_t len);
void InitEEPROM(void);
uint8_t VE_PageWrite(uint8_t VEpage, uint8_t* data, unsigned short count);
uint8_t VE_PageRead(uint8_t VEpage, uint8_t *output);
uint8_t VE_WriteUint8(uint8_t VEpage, uint16_t adr, uint8_t data);
uint8_t VE_ReadUint8(uint8_t VEpage, uint16_t adr);
uint8_t VE_WriteUint32(uint8_t VEpage, uint16_t adr, uint32_t data);
uint32_t VE_ReadUint32(uint8_t page, uint16_t adr);
uint8_t VE_WriteFloat(uint8_t VEpage, uint16_t adr, float fl);
float VE_ReadFloat(uint8_t VEpage, uint16_t adr);
uint16_t VE_ReadUint16(uint8_t VEpage, uint16_t adr);
//******************************************************************************

#endif