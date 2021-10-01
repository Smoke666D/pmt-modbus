#ifndef __REGISTER_H
#define __REGISTER_H
/***************************************************************************
Структура данны слежущая:
Сначала идут RAM регистры, потом идут ROM регистры. При этом логическая адресация сквозная,
первый адрес ROM регистра, это следующий за последним RAM. Это позволяет динамическ

***************************************************************************/
#include "user_process.h"
#include "port.h"

typedef enum
{
  ROM_OK,
  ROM_COUNT_ERORR,
  ROM_CRC_ERROR,
} CHECK_ROM_ERROR;

typedef enum
{
  COIL,
  DISCRET_INPUT,
} BIT_DATA_TYPE;

#define HR_REGISTER_ADR_START           0
#define HR_RAM_COUNT                    HOLDING_REGISTER_RAM_COUNT 
#define HR_ROM_COUNT                    HOLDING_REGISTER_ROM_COUNT 
#define HR_REGISTER_COUNT               (HR_RAM_COUNT+ HR_ROM_COUNT)
#define HR_ROM_OFF                      HR_RAM_COUNT

#define DI_REGISTER_ADR_START           (HR_REGISTER_ADR_START + HR_REGISTER_COUNT)
#define DI_RAM_INPUT_COUNT              48 
#define DI_RAM_COUNT                    3
#define DI_INPUT_COUNT                  (DI_RAM_COUNT )
#define DI_REG_COUNT                    (DI_RAM_INPUT_COUNT )
#define DI_REGISTER_COUNT               (DI_RAM_COUNT)

#define IR_REGISTER_ADR_START           (DI_REGISTER_ADR_START + DI_REGISTER_COUNT)
#define IR_RAM_COUNT                    3
#define IR_REGISTER_COUNT               (IR_RAM_COUNT)

#define CR_REGISTER_ADR_START           (IR_REGISTER_ADR_START + IR_REGISTER_COUNT)
#define CR_RAM_COILS_COUNT              16                   
#define CR_ROM_COILS_COUNT              96
#define CR_RAM_COUNT                    1       
#define CR_ROM_COUNT                    6
#define CR_REGISTER_COUNT               (CR_RAM_COUNT+ CR_ROM_COUNT)
#define CR_COILS_COUNT                  (CR_RAM_COILS_COUNT + CR_ROM_COILS_COUNT)
#define CR_ROM_OFF                      (HR_RAM_COUNT+DI_RAM_COUNT+IR_RAM_COUNT+CR_RAM_COUNT)
//*************************Пользовательские регистры****************************
#define USER_REGISTER_ADR_START         (CR_REGISTER_ADR_START + CR_REGISTER_COUNT)
#define USER_RAM_COUNT                  0
#define USER_ROM_COUNT                  2
#define USER_REGISTER_COUNT             (USER_RAM_COUNT+ USER_ROM_COUNT)
#define USER_ROM_OFF                    (HR_RAM_COUNT+DI_RAM_COUNT+IR_RAM_COUNT+CR_RAM_COUNT+USER_RAM_COUNT)

#define BOUND_RATE_ADR_HI               0
#define BOUND_RATE_ADR_LO               1

#define HR_REGISTER_OFF                 0
#define DI_REGISTER_OFF                 HR_RAM_COUNT    + HR_ROM_COUNT
#define IR_REGISTER_OFF                 DI_REGISTER_OFF + IR_RAM_COUNT   + IR_ROM_COUNT
#define CR_REGISTER_OFF                 IR_REGISTER_OFF + CR_RAM_COUNT   + CR_ROM_COUNT
#define USER_REGISTER_OFF               CR_REGISTER_OFF + USER_RAM_COUNT + USER_ROM_COUNT+DI_REGISTER_COUNT

#define REGISTER_COUNT                  (USER_REGISTER_COUNT+CR_REGISTER_COUNT+IR_REGISTER_COUNT+HR_REGISTER_COUNT+DI_REGISTER_COUNT)  

#define ROM_REGISTER_COUNT              (HR_ROM_COUNT + CR_ROM_COUNT + USER_ROM_COUNT) //Количество регистор, которые необходимо хранить в ROMREGISTER_DEFAULT_VAL

//Внетрение переменные драйвера
#define  ROM_REGISTER_COUNT_ADR         1              //Виртуальные адрес двух байт определяющих коло-во вирутальных ROM регистров
#define  ROM_REGISTER_CRC_ADR           0
#define  ROM_REGISTER_DATA_OFS          1
//******************************************************************************
void    SetInd(USHORT T);
void    CountInd();
void    MBRegisterInit();
void    WriteROMRegister();
void    WriteHolding(USHORT adr, UCHAR *data, USHORT count);
void    WriteShortToHolding(USHORT adr, USHORT data);
void    ResetHoldingModifyFlag(void);
USHORT  GetHoldingModifyFlag(void);
USHORT  ReadHolding(USHORT adr);
USHORT  ReadInput(USHORT adr);
USHORT  ReadUserRegister(USHORT adr);
void    WriteUserRegister(USHORT adr, UCHAR *data,USHORT count);
void    WriteShortToUserRegister(USHORT adr, USHORT data);
void    WriteCoil(USHORT adr, USHORT OutputValue);
void    ReadBitData(USHORT adr, UCHAR *data,USHORT count,BIT_DATA_TYPE DataType);
void    WriteCoils(USHORT adr, UCHAR *data,USHORT count);
//******************************************************************************
extern  USHORT REGISTER [REGISTER_COUNT+2];
//******************************************************************************
#endif