/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbrtu.h"
#include "mbfunc.h"
#include "init.h"
#include "stm32f10x.h"
/* ----------------------- Static variables ---------------------------------*/
static enum
{
  STATE_RECIEVE,
  STATE_EXECUTE,
  STATE_RESPONSE,
  STATE_REINIT,
} MBState = STATE_RECIEVE;

static unsigned char ListenOnlyMode = 0;
static unsigned char MasterMode     = 0;
static UCHAR ucMBFrame[MB_PDU_SIZE_MAX];        // Статический буфер для хранения фрейма
static UCHAR ucRcvAddress;  

// Массив функций команд на исполнение
static MBFunctionHandlerTable xFuncHandlers[]=
{
  //*******************00*******************
  eMBNoFunction,
  //*******************01*******************
  #if (MB_FUNC_READ_COILS_ENABLED)
    eMBFuncReadCoils,
  #else
    eMBNoFunction,
  #endif
  //*******************02*******************
  #if (MB_FUNC_READ_DISCRETE_INPUTS_ENABLED)
    eMBFuncReadDiscreteInputs,
  #else
    eMBNoFunction,
  #endif
  //*******************03*******************
  #if (MB_FUNC_READ_HOLDING_ENABLED)
    eMBFuncReadHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  //*******************04*******************
  #if (MB_FUNC_READ_INPUT_ENABLED)
    eMBFuncReadInputRegister,
  #else
    eMBNoFunction,
  #endif
  //*******************05*******************
  #if (MB_FUNC_WRITE_COIL_ENABLED)
    eMBFuncWriteCoil,
  #else
    eMBNoFunction,
  #endif
  //*******************06*******************
  #if (MB_FUNC_WRITE_HOLDING_ENABLED)
    eMBFuncWriteHoldingRegister,
  #else
    eMBNoFunction,
  #endif 
  //*******************07*******************
  eMBNoFunction,        // Read Exceprion Sataus
  //*******************08*******************
  eMBNoFunction,        // Read Diagnostic
  //*******************09*******************
  eMBNoFunction,        // 09
  //*******************10*******************
  eMBNoFunction,        // 10
  //*******************11*******************
  eMBNoFunction,        // Get CommEvernCounter
  //*******************12*******************
  eMBNoFunction,        // Get CommEvernLog
  //*******************13*******************
  eMBNoFunction,        // 13
  //*******************14*******************
  eMBNoFunction,        // 14
  //*******************15*******************
  #if (MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED)
    eMBFuncWriteMultipleCoils,
  #else
    eMBNoFunction,
  #endif
  //*******************16*******************
  #if (MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED)
    eMBFuncWriteMultipleHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  //*******************17*******************
  eMBNoFunction, // 17
  //*******************18*******************
  eMBNoFunction, // 18
  //*******************19*******************
  eMBNoFunction, // 19
  //*******************20*******************
  eMBNoFunction, // 20
  //*******************21*******************
  eMBNoFunction, // 21
  //*******************22*******************
  #if (MB_FUNC_MASK_WRITE_REGISTER_ENABLED)
    eMBFuncMaskWriteRegister,
  #else
    eMBNoFunction,
  #endif
  //*******************23*******************
  #if (MB_FUNC_READWRITE_HOLDING_ENABLED)
    eMBFuncReadWriteMultipleHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  //*******************24*******************
  eMBNoFunction, //24
  //*******************25*******************
  eMBNoFunction, //25
  //*******************26*******************
  eMBNoFunction, //26
  //*******************27*******************
  eMBNoFunction, //27
  //*******************28*******************
  eMBNoFunction, //28
  //*******************29*******************
  eMBNoFunction, //29
  //*******************30*******************
  eMBNoFunction, //30
  //*******************31*******************
  eMBNoFunction, //31
  //*******************32*******************
  eMBNoFunction, //32
  //*******************33*******************
  eMBNoFunction, //33
  //*******************34*******************
  eMBNoFunction, //34
  //*******************35*******************
  eMBNoFunction, //35
  //*******************36*******************
  eMBNoFunction, //36
  //*******************37*******************
  eMBNoFunction, //37
  //*******************38*******************
  eMBNoFunction, //38
  //*******************39*******************
  eMBNoFunction, //39
  //*******************40*******************
  eMBNoFunction, //40
  //*******************41*******************
  eMBNoFunction, //41
  //*******************42*******************
  eMBNoFunction, //42
  //*******************43*******************
  eMBNoFunction, //43
  //*******************44*******************
  eMBFuncSetRSParametrs 
};
//******************************************************************************
//**************************FUNCTION********************************************
//******************************************************************************
void ListenOnlyModeEnablae(void)
{
  ListenOnlyMode = 1; 
  return;
}
void ListenOnlyModeDisable(void)
{
  ListenOnlyMode = 0; 
  return;
}
void InitMB( void )
{
  ListenOnlyMode = 0;
  Start0_5Timer();
  return;
}

void SetMasterMode(void)
{  
  MasterMode = 1;
  return;
}

UCHAR IfMasterMode(void)
{
  return MasterMode;
}
/* ----------------------- Start implementation -----------------------------*/

eMBException  eMBNoFunction(UCHAR * pucFrame, UCHAR * usLen)
{
   return MB_EX_ILLEGAL_FUNCTION;
}



UCHAR GetRequestAdress()
{
  return ucRcvAddress;
}


void eMBPoll( void )
{
  //Переменная с адресом устройства на момент прихода фрема
  static UCHAR PDULength;                  //Переменная с длиннй фрема PDU
  static UCHAR ucFunctionCode;             //Код функции
  static eMBException eException;
  //Конечный автомат работы с валидными пакетами
  switch (MBState)
  {
    case STATE_RECIEVE:                                                         // Запрашиваем нижний уровень о наличии валидного фрема для обработки      
      if (MBRTUReceive(&ucRcvAddress,ucMBFrame,&PDULength) == VALID_FRAME )     // Проверяем валидность фрейма и копируем фрейм во входные переменные функции
      {
        MBState = STATE_EXECUTE;                                                // Если валиден, переходим к обработке кода функции
      }
      break;
    case STATE_EXECUTE:                                                         // Проверям код функции
      ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
      if (ucFunctionCode < MAX_FUN_NUM)                                         // Если не вышли за верхний диапазон команд
      {
        eException = xFuncHandlers[ucFunctionCode].pxHandler(ucMBFrame, &PDULength);    // Исполнение команды
      }
      else if (ucFunctionCode == 66)                                            // 
      {
        eException = xFuncHandlers[44].pxHandler(ucMBFrame, &PDULength);
      }
      else
      {
        eException = MB_EX_ILLEGAL_FUNCTION;                                    // Неправильный код функции            
      }
      if (eException != MB_EX_NONE)                                             // Если в процессе выполнения функции была ошибка, увеличиваем внутрений счетчик
      {
        ModBusCounters.SlaveExceptionErrorCount++;
      }  
      MBState = STATE_RESPONSE;        
      if ((ucRcvAddress == MB_ADDRESS_BROADCAST) || ListenOnlyMode)
      { 				
        if  (eException == MB_EX_REINIT)
        {
          MBState = STATE_REINIT;
        }
        else 
        {
          MBState =  STATE_RECIEVE;
        }
      }
      break;                
    case STATE_RESPONSE:                                 
      if ((eException != MB_EX_NONE) && (eException != MB_EX_REINIT))
      {
        /* An exception occured. Build an error frame. */
        PDULength = 0;
        ucMBFrame[PDULength++] = (UCHAR)(ucFunctionCode | MB_FUNC_ERROR);
        ucMBFrame[PDULength++] = eException;                    
      }                  
      MBRTUSend( &ucRcvAddress, ucMBFrame,&PDULength );
      if  (eException == MB_EX_REINIT)
      {
        MBState = STATE_REINIT;
      }
      else
      {
        MBState = STATE_RECIEVE;       
      }
      break;
    case STATE_REINIT:
      Delay_ms(1000);
      MBState = STATE_RECIEVE;
      Init();        
      break;
  } 
}