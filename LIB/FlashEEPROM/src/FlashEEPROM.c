#include "FlashEEPROM.h"
#include "stm32f10x_flash.h"
#include "stm32f10x.h"
#include "CRC.h"

//***************************VAR***********************************************
FLASH_Status            res;                    // Переменная с сообщениями от функций
uint8_t                 Data[PageSize];         // Буфер для работы со страницами памяти
//*****************************************************************************
//****************************Function*****************************************
//*****************************************************************************


//*****************************FLASH*******************************************
void FlashPageCRCcalc(uint8_t* PageData)
{
  uint32_t crc=0, i;
  crc = crc32_8block(PageData , AdrCRC);                                // Считаем CRC с 0-ого по 1019-ый адрес страницы
  for(i=0;i<4;i++) PageData[AdrCRC+i] = (uint8_t)(crc >> 8*(3-i));      // Записываем СRC32 в 1020-1023 адресов памяти
  return;
}

uint8_t CheckPageCRC(uint8_t* PageData)
{
  uint8_t i;
  uint32_t crc=0,crc32=0;
  
  for(i=0;i<4;i++) crc = crc + (((uint32_t)(PageData[AdrCRC + i])) << (8*(3-i))); // Собираем СRC32 из 1020-1023 адресов памяти
  crc32 = crc32_8block(PageData, AdrCRC);
  if (crc == crc32) return 1;                          // Считаем CRC и сравниваем с записью на старнице
  else return 0;
}

void FlashPageWrite(uint8_t page, uint8_t* data,unsigned short count)
{
  uint32_t i, adr;
  
 // FlashPageCRCcalc(data);               // Вносим в массив данные байт контрольной суммы
  adr = FlashStartAdr + page*PageSize;  // Расчитываем адресс начала страницы во Flash
  FLASH->KEYR = FLASH_KEY1;             // Снимаем защиту с записи флеши №1
  FLASH->KEYR = FLASH_KEY2;             // Снимаем защиту с записи флеши №2
  while((FLASH->SR&FLASH_SR_BSY));      // Ждем пока контроллер флешь не занят
  if (FLASH->SR&FLASH_SR_EOP){          // Если флажок окончания операции выставлен...
      FLASH->SR = FLASH_SR_EOP;}        // Сбрасываем его записью еденицы
  FLASH->CR |= FLASH_CR_PER;            // Настраиваем на стирание контроллер флешь
  FLASH->AR  = adr;                     // Задаем адресс страницы
  FLASH->CR |= FLASH_CR_STRT;           // Стираем страницу
  while(!(FLASH->SR&FLASH_SR_EOP));     // Ждем пока контроллер флешь не сотрет страницу (не выставит флажок окончания операции)
  FLASH->SR = FLASH_SR_EOP;             // Сбрасываем его записью еденицы
  FLASH->CR &= ~FLASH_CR_PER;           // Выходим из режима стирания флешь
  while((FLASH->SR&FLASH_SR_BSY));      // Ждем пока контроллер флешь не занят
  if (FLASH->SR&FLASH_SR_EOP){          // Если флажок окончания операции выставлен...
      FLASH->SR = FLASH_SR_EOP;}        // Сбрасываем его записью еденицы
  FLASH->CR |= FLASH_CR_PG;             // Переводим контроллер флеши в режим записи
 for(i=0;i<count;i+=2)              // Пробегаемся по всей странице, но запись идет по 16 бит :(
 {
   *(__IO uint16_t*)(adr + i) = ((uint16_t)(data[i+1])<<8)+data[i];      // Пишим
   while(!(FLASH->SR&FLASH_SR_EOP));   // Ждем пока контроллер флешь не сотрет страницу (не выставит флажок окончания операции)
   FLASH->SR = FLASH_SR_EOP;           // Сбрасываем его записью еденицы
 }
 FLASH->CR &= ~FLASH_CR_PG;            // Выходим из режима записи
 FLASH->CR |= FLASH_CR_LOCK;           // Востонавливаем защиту записи флеши 
 return;
}

void FlashPageRead(uint8_t page, uint8_t *output)
{
  uint32_t i, adr=0;
  
  adr = FlashStartAdr + page*PageSize;                          // Высчитываем адрес начала страницы
  for(i=0;i<PageSize;i++) output[i] = FlashReadUint8(adr+i);    // Пробегаем по всей странице
  return;                                                       // Возвращаем указатель на массив
}

uint32_t FlashReadUint32(uint32_t adr)
{
  return (*(__IO uint32_t*) adr);               // Возвращаем данные из флешь по адрессу
}

uint16_t FlashReadUint16(uint32_t adr)
{
  return (uint16_t)((*(__IO uint32_t*) adr));   // Возвращаем данные из флешь по адрессу
}

uint8_t FlashReadUint8(uint32_t adr)
{
  return (uint8_t)((*(__IO uint32_t*) adr));    // Возвращаем данные из флешь по адрессу
}

void BufferErase(uint8_t *data, uint16_t len)
{
  uint16_t i;
  for(i=0;i<len;i++) 
  {
    data[i]=0;
  }
  return;
}

//*************************VIRTUAL EEPROM***************************************
uint32_t VEpages[EEPROMSize];
//**********PAGE**********
void InitEEPROM(void)
{
  uint32_t i;
  
  for(i=0;i<EEPROMSize;i++) 
  {
    VEpages[i] = EEPROMFirstPage + i;   // Расчитываем абсолютный номер вирткульной страницы в страницах всей флешь памяти
  }
  return;
}

uint8_t VE_PageWrite(uint8_t VEpage, uint8_t* data, unsigned short count)
{
  if (VEpage > EEPROMSize) 
  {
    return 0;                                                   // Проверяем превышение размера памяти
  }
  FlashPageWrite((EEPROMFirstPage + VEpage), data,count*2);     // Записываем данные в страницу памяти
  return 1;                                                     // Возвращаем флажок успешности
}

uint8_t VE_PageRead(uint8_t VEpage, uint8_t *output)
{
  if (VEpage > EEPROMSize) 
  {
    return 0;
  }
  FlashPageRead((EEPROMFirstPage + VEpage), output);
  return 1;
}

//**********UINT8*********

/*uint8_t VE_WriteUint8(uint8_t VEpage, uint16_t adr, uint8_t data)
{  
  if (VEpage > EEPROMSize) return 0;            // Проверяем превышение размера памяти
  FlashPageRead((EEPROMFirstPage + VEpage), Data);       // Читаем страницу памяти 
  Data[adr] = data;                           // Вносим изменения в страницу
  FlashPageWrite((EEPROMFirstPage + VEpage), Data);      // Перезаписываем страницу памяти
  return 1;                                     // Возвращаем флажок успешности
}
*/
uint8_t VE_ReadUint8(uint8_t VEpage, uint16_t adr)
{
  return (VEpage > EEPROMSize)? 0: FlashReadUint8(EEPROMStartAdr + VEpage*PageSize + adr);
}

//**********UINT16************
uint16_t VE_ReadUint16(uint8_t VEpage, uint16_t adr)
{
  return (VEpage > EEPROMSize)? 0: FlashReadUint16(EEPROMStartAdr + VEpage*PageSize + adr);
}



//*********Uint32*********

/*uint8_t VE_WriteUint32(uint8_t VEpage, uint16_t adr, uint32_t data)
{
  uint8_t i;
  uint32_t FlashAdr = 0;
  
  if (VEpage > EEPROMSize) return 0;                            // Проверяем превышение размера памяти
  FlashPageRead((EEPROMFirstPage + VEpage), Data);              // Читаем страницу памяти 
  for(i=0;i<4;i++) Data[adr+i] = (uint8_t)(data >> (8*i));      // Записываем последовательно 4 байта
  FlashAdr = EEPROMFirstPage + VEpage;
  FlashPageWrite(FlashAdr, Data);                               // Перезаписываем страницу памяти
  return 1;                                                     // Возвращаем флажок успешности
}
*/
uint32_t VE_ReadUint32(uint8_t page, uint16_t adr)
{
  uint32_t FlashAdr = 0;
  FlashAdr = EEPROMStartAdr + page*PageSize + adr;
  return (page > EEPROMSize)? 0: FlashReadUint32(FlashAdr);
}

//*********Float**********

uint8_t VE_WriteFloat(uint8_t VEpage, uint16_t adr, float fl)
{
  uint32_t *pf=0, data=0;
  
  if (VEpage > EEPROMSize) return 0;            // Проверяем превышение размера памяти
  pf = (uint32_t*)(&fl);                        // Получаем указатель на побитовое представление float
  data = *pf;                                   // Выципляем 32-битное представление float
  return VE_WriteUint32( VEpage, adr, data);   // Перезаписываем страницу памяти
}

float VE_ReadFloat(uint8_t VEpage, uint16_t adr)
{
  uint32_t data=0;
  
  if (VEpage > EEPROMSize) return 0xFF;         // Проверяем превышение размера памяти
  data = VE_ReadUint32(VEpage, adr);   // Читаем 32 бита закодированного float
  return *((float*)(&data));                    // Возращаем раскодированную переменную float
}

//************************