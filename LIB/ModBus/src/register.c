//****************************INCLUDE******************************************
#include "stm32f10x.h"
#include "Register.h"
#include "FlashEEPROM.h"
#include "mbcrc.h"
#include "system.h"
#include "USART.h"
//***************************LOCAL VAR*****************************************
static USHORT   HoldingModifyFlag = 0;
static USHORT   REGISTER [REGISTER_COUNT+2];
//*****************************CONST*******************************************
#if HR_REGISTER_COUNT  > 0
  const USHORT HoldingRegisterDefault[HR_REGISTER_COUNT] = REGISTER_DEFAULT_VAL;
#endif
#if DI_REGISTER_COUNT  > 0
  const USHORT DIRegisterDefault[DI_REGISTER_COUNT]={0,0x7733,0xF0F0};
#endif
#if CR_REGISTER_COUNT  > 0
  const USHORT CRRegisterDefault[CR_REGISTER_COUNT]={0,0x7733,0xF0F0,0x7733,0x0F0F,0,0};
#endif
#if USER_REGISTER_COUNT  > 0
  const USHORT USERRegisterDefault[USER_REGISTER_COUNT]={UART_115200,PARITY_EVEN};
#endif
//Интерфейсная часть, сюда стыковать драйвер настошего ROM


//Функция чтения слова. Парамерт addr - адрес слова, для драйвера ROM это массив слов
unsigned short ReadROMShort(USHORT addr)
{
  return VE_ReadUint16(0, addr*2);
}

//Функция проверки целостности ROM
//исходя из того, что первое слово CRC это кол-во байт, второе слово колво байт

CHECK_ROM_ERROR CheckROM(void)
{
  CHECK_ROM_ERROR ErrorCode = ROM_OK;  
  if ( ReadROMShort(1) != ROM_REGISTER_COUNT)
  {
    ErrorCode = ROM_COUNT_ERORR;
  }
  else
  {
    for (int i=0;i<ROM_REGISTER_COUNT+1;i++) 
    {
      REGISTER[i] = ReadROMShort(i+ROM_REGISTER_DATA_OFS);    
    }
    if (usMBCRC16((unsigned char*)REGISTER, ROM_REGISTER_COUNT+1 )!=ReadROMShort(ROM_REGISTER_CRC_ADR))
    {
      ErrorCode = ROM_CRC_ERROR;
    }
  }   
  return ErrorCode;
}

void WriteROMRegister(void)
{
  unsigned short BufferData[ROM_REGISTER_COUNT+2];
  BufferData[1] = ROM_REGISTER_COUNT;     
  for (int i=0;i<REGISTER_COUNT;i++)
  {
    #if HR_REGISTER_COUNT  > 0    
      if ((i >= HR_REGISTER_ADR_START + HR_RAM_COUNT ) &&  (i < DI_REGISTER_ADR_START ))
      {
        BufferData[2+i-HR_ROM_OFF]= REGISTER[i];
      }
    #endif                   
    #if CR_REGISTER_COUNT  > 0
      if ((i >= CR_REGISTER_ADR_START + CR_RAM_COUNT ) &&  (i < USER_REGISTER_ADR_START ))
      {
        BufferData[2+i-CR_ROM_OFF]= REGISTER[i];
      }
    #endif
    #if USER_REGISTER_COUNT  > 0
      if (i >= USER_REGISTER_ADR_START + USER_RAM_COUNT ) 
      {
        BufferData[2+i-USER_ROM_OFF]= REGISTER[i];
      }
    #endif
    WatchDog_reset();
  }  
  BufferData[0] = usMBCRC16((UCHAR*)(BufferData+1), ROM_REGISTER_COUNT+1 );
  VE_PageWrite(0,(UCHAR*)BufferData, ROM_REGISTER_COUNT+2); 
  return;
}

void MBRegisterInit()
{
  //Проверка блока данных флеш.
  //При инициализации,обнуляем регистры
  InitEEPROM(); //Инициализация драйвера виртуальнго EEPROM
  WatchDog_reset();
  ResetHoldingModifyFlag();
  
  if ((CheckROM() != ROM_OK) || (GetSwitsh() == 0))             //Если флеш не проходит проверку, проводим иницалазацию флеша данными по умолчанию
  {
    for (int i=0;i<REGISTER_COUNT;i++)
    {
      #if HR_REGISTER_COUNT  > 0    
        if ((i < HR_REGISTER_ADR_START + HR_REGISTER_COUNT) &&  (i >= HR_REGISTER_ADR_START ))
        {
          REGISTER[i] = HoldingRegisterDefault[i-HR_REGISTER_ADR_START];
        }
      #endif         
      #if CR_REGISTER_COUNT  > 0
        if ((i < CR_REGISTER_ADR_START + CR_REGISTER_COUNT) &&  (i >= CR_REGISTER_ADR_START ))
        {
          REGISTER[i] = CRRegisterDefault[i-CR_REGISTER_ADR_START];
        }
      #endif
      #if USER_REGISTER_COUNT  > 0
        if ((i < USER_REGISTER_ADR_START + USER_REGISTER_COUNT) &&  (i >= USER_REGISTER_ADR_START ))
        {
          REGISTER[i] = USERRegisterDefault[i-USER_REGISTER_ADR_START];
        }
      #endif
      WatchDog_reset();
    }
    WriteROMRegister();        
  }  
  WatchDog_reset();      
  for (int i=0;i<REGISTER_COUNT;i++)
  {
    #if HR_REGISTER_COUNT  > 0    
      if ((i >= HR_REGISTER_ADR_START + HR_RAM_COUNT ) &&  (i < DI_REGISTER_ADR_START ))
      {
        REGISTER[i] = ReadROMShort(i-HR_ROM_OFF+2);
      }
      else
    #endif                   
    #if CR_REGISTER_COUNT  > 0
      if ((i >= CR_REGISTER_ADR_START + CR_RAM_COUNT ) &&  (i < USER_REGISTER_ADR_START ))
      {
        REGISTER[i] = ReadROMShort(i-CR_ROM_OFF+2);
      }
      else
    #endif
    #if USER_REGISTER_COUNT  > 0
      if (i >= USER_REGISTER_ADR_START + USER_RAM_COUNT ) 
      {
        REGISTER[i] = ReadROMShort(i-USER_ROM_OFF+2);
      }
      else
    #endif 
    REGISTER[i]=0; 
  }
  WatchDog_reset();
  return;
}


void ResetHoldingModifyFlag(void)
{
  HoldingModifyFlag = 0;
  return;
}
unsigned short GetHoldingModifyFlag(void)
{
  return HoldingModifyFlag;
}

void WriteHolding(USHORT adr, UCHAR *data, USHORT count)
{  
  for (unsigned short i=0;i<count;i++)  
  {
    REGISTER[HR_REGISTER_ADR_START +adr + i ] = (((USHORT)data[i*2])<<8) | (USHORT)data[i*2+1]; 
  }
  WatchDog_reset();
  if ((adr + count) > HR_RAM_COUNT)
  {
    WriteROMRegister();
  }
  HoldingModifyFlag = adr + 1 ; 
  return;
}

void WriteShortToHolding(USHORT adr, USHORT data)
{  
  REGISTER[HR_REGISTER_ADR_START +adr] = data;  
  if (adr > HR_RAM_COUNT) 
  {
    WriteROMRegister(); 
  }
  HoldingModifyFlag = adr + 1 ;
  return;
}

unsigned short ReadInput(USHORT adr)
{
  return REGISTER[IR_REGISTER_ADR_START+adr];
}

unsigned short ReadHolding(USHORT adr)
{  
  return REGISTER[HR_REGISTER_ADR_START+adr];
}

//*******************Функции для работы с пользовательскими регистрами******************************************
//Чтение пользовательского регистра
unsigned short ReadUserRegister(USHORT adr)
{
  return REGISTER[USER_REGISTER_ADR_START+adr];
}
//Запись пользовательского регистра, при этом данные приходят ввиде маасива char старышим байтом вперед
void WriteUserRegister(USHORT adr, UCHAR *data, USHORT count)
{
  unsigned short i = 0;

  for (i=0;i <count; i++) 
  { 
    REGISTER[USER_REGISTER_ADR_START + adr + i ] =  ((USHORT)data[i*2])<<8 | (ULONG)data[i*2 + 1];
    WatchDog_reset();
  }          
  if ( (adr + count) > USER_RAM_COUNT)
  {
    WriteROMRegister();  
  }
  return;  
}

void WriteShortToUserRegister(USHORT adr, USHORT data)
{
  REGISTER[USER_REGISTER_ADR_START + adr] =  data; 
  if ( adr >= USER_RAM_COUNT)
  {
    WriteROMRegister();      
  }
  return;
}

void WriteCoils(USHORT adr, UCHAR *data, USHORT count)
{
  UCHAR register_offset;
  UCHAR *current_pointer;
  UCHAR current_offset = 0;
  UCHAR data_mask;
  USHORT current_adr;
  USHORT i;
  //****************************************************************************
  current_adr = adr;
  current_pointer  = (UCHAR*) & REGISTER[CR_REGISTER_ADR_START];
  current_pointer += (adr >> 3);
  for (i=count;i>=0;i=i-8)
  {
    register_offset =current_adr & 0x0007; 
    if (i>=8)
    {                        
      current_pointer[current_offset]=(current_pointer[current_offset] & (0xFF>>(8-register_offset))) | (data[current_offset]<<register_offset);
      current_pointer[current_offset+1]=(current_pointer[current_offset+1] & (0xFF<<register_offset+1)) | (data[current_offset]>>(8-register_offset));          
      current_offset++;
      current_adr += 8;
    }
    else
    {
      if (i >(8-register_offset))
      {
        current_pointer[current_offset]=(current_pointer[current_offset] & (0xFF>>(8-register_offset))) | (data[current_offset]<<register_offset);
        current_pointer[current_offset+1]=(current_pointer[current_offset+1] & (0xFF<<(i-(8-register_offset)))) | (data[current_offset]>>(8-(i-register_offset)));                    
      }
      else
      {
        data_mask =(0xFF<<(register_offset+i)) | (0xFF>>(8-register_offset));
        current_pointer[current_offset]= (current_pointer[current_offset] & data_mask) | ((data[current_offset]<<register_offset)  &(~data_mask)) ;
      }                                                                                                                                                                                                                                                                                                                                     
    }    
  }
  if (((adr+count)>>4) >= CR_RAM_COUNT)
  {
    WriteROMRegister();  
  }
  return;
} 

void WriteCoil(USHORT adr, USHORT OutputValue)
{
  UCHAR register_offset;
  UCHAR *current_pointer;
  UCHAR data_mask;  
  current_pointer  = (UCHAR*) & REGISTER[CR_REGISTER_ADR_START];
  current_pointer += (adr >> 3); 
  register_offset  = adr & 0x0007;    
  data_mask =( 0xFF << (register_offset +1 ) ) | ( 0xFF >> ( 8-register_offset ) );  
  if ( OutputValue )
  {
    current_pointer[0] = (current_pointer[0] | (~data_mask));    
  }
  else 
  {
    current_pointer[0]=(current_pointer[0] & data_mask); 
  }
  WatchDog_reset();    
  if ((adr>>4) >= CR_RAM_COUNT)
  {
    WriteROMRegister();  
  }
  return;
}
//Функция чтения битовых данных из регистров
void ReadBitData(USHORT adr, UCHAR *data, USHORT count, BIT_DATA_TYPE DataType)
{
  UCHAR register_offset;
  UCHAR *current_pointer;
  UCHAR  current_offset = 0;
  USHORT current_adr;
  USHORT i;
  //***************************************************************************
  current_adr = adr;
  switch  (DataType) //Выбор типа данных
  {
    case DISCRET_INPUT:
      current_pointer =(UCHAR*) & REGISTER[DI_REGISTER_ADR_START];
      break;
    case COIL:
      current_pointer =(UCHAR*) & REGISTER[CR_REGISTER_ADR_START];
       break;
  }   
  current_pointer +=(adr >> 3);  //Установка адреса байта регистра в которм расположен первый бит с нужны адресом
  for (i=count;i>0;i=i-8)
  {
    register_offset = current_adr & 0x0007; 
    data[current_offset]=(current_pointer[current_offset]>>register_offset) | current_pointer[current_offset+1]<<(8-register_offset);                 
    current_offset++;
    current_adr += 8;
    if (i<8)      //Если число битов не кратно байту, обнуляем не валидные биты
    {
      data[current_offset] = data[current_offset]& (0xFF>>i);    
    }
  }       
  WatchDog_reset();
  return;
}