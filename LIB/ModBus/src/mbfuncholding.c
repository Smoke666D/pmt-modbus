/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbfuncholding.c,v 1.12 2007/02/18 23:48:22 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "stm32f10x.h"
#include "system.h"
/* ----------------------- Platform includes --------------------------------*/

#include "port.h"
#include "USART.h"
#include "register.h"
#include "init.h"
/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"

/* ----------------------- Defines ------------------------------------------*/

#define MB_HOLDING_REG_START  0
#define MB_COILS_REG_START    0
#define MB_INPUT_REG_START    0
#define MB_DI_REG_START       0

#define MB_PDU_FUNC_READ_WRITE_ADDR_OFF          0x01
#define MB_PDU_FUNC_READ_REGCNT_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE                    0x05
#define MB_PDU_FUNC_READ_REGCNT_MAX             ( 0x007B )
#define MB_PDU_FUNC_READ_BITCNT_MAX             ( 0x07B0 )

#define MB_PDU_FUNC_WRITE_VALUE_OFF             0x03
#define MB_PDU_FUNC_WRITE_SIZE                  0x05
#define MB_PDU_FUNC_MASK_WRITE_SIZE             (7)
   
#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF          ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF        ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN          ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX        ( 0x0078 )

#define MB_PDU_FUNC_READWRITE_READ_ADDR_OFF     ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF    ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF  ( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_FUNC_READWRITE_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 8 )
#define MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF  ( MB_PDU_DATA_OFF + 9 )
#define MB_PDU_FUNC_READWRITE_SIZE_MIN          ( 9 )


/* ----------------------- Start implementation -----------------------------*/
//Регистр для индикатора
#if (MB_FUNC_MASK_WRITE_REGISTER_ENABLED)
eMBException eMBFuncMaskWriteRegister(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT        usRegAddress;
  USHORT        usData;
  USHORT        And_Mask;
  USHORT        Or_Mask;  
  eMBException  eStatus = MB_EX_NONE;
  //Проверка валидности адресса
  usRegAddress = (USHORT)( pucFrame[STARTING_ADDRESS_HI] <<8) | (USHORT)pucFrame[STARTING_ADDRESS_LO];         
  if (  usRegAddress < HR_REGISTER_COUNT  )              
  {         
    if ( *usLen == MB_PDU_FUNC_MASK_WRITE_SIZE )
    {
      And_Mask = (((USHORT) pucFrame[AND_MASK_HI]) << 8 ) | (USHORT)pucFrame[AND_MASK_LO]; 
      Or_Mask  = (((USHORT) pucFrame[OR_MASK_HI]) << 8 ) | (USHORT)pucFrame[OR_MASK_LO];                                 
      usData =  ( ReadHolding (usRegAddress) & And_Mask )  |  (  Or_Mask & ( ~And_Mask) );
      usData = (usData >> 8) | (usData<<8);
      ENTER_CRITICAL_SECTION();                 
      WriteHolding(usRegAddress, (UCHAR*)&usData ,1);
      EXIT_CRITICAL_SECTION(); 
    }    
    else    
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;          
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
  }
  return eStatus;
}
#endif   
   
#if (MB_FUNC_WRITE_HOLDING_ENABLED)
//Функция записи 16-битного регистра
eMBException eMBFuncWriteHoldingRegister(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT        usRegAddress;
  eMBException  eStatus = MB_EX_NONE;
  if(*usLen ==  MB_PDU_FUNC_WRITE_SIZE)
  {
    usRegAddress = (USHORT)( pucFrame[STARTING_ADDRESS_HI] << 8 ) | (USHORT)pucFrame[STARTING_ADDRESS_LO];           
    if (usRegAddress <  HR_REGISTER_COUNT) 
    { 
      ENTER_CRITICAL_SECTION();                 
      WriteHolding(usRegAddress, &pucFrame[3],1);
      EXIT_CRITICAL_SECTION();    
    } 
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif

#if (MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED)
eMBException eMBFuncWriteMultipleHoldingRegister(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT        usRegAddress;
  USHORT        usRegCount;
  UCHAR         ucRegByteCount;
  eMBException  eStatus = MB_EX_NONE;
   
  if( *usLen >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
  {
    //Проверка валидности адресса    
    usRegAddress = ( USHORT )( pucFrame[STARTING_ADDRESS_HI] << 8 ) | ( USHORT )pucFrame[STARTING_ADDRESS_LO]; 
    usRegCount   = ( USHORT )( pucFrame[3] << 8 ) | ( USHORT )pucFrame[4];
    if (  (usRegCount + usRegAddress) <= HR_REGISTER_COUNT) 
    {
      ucRegByteCount = pucFrame[5];
      if( ( usRegCount >= 1 )  && (usRegCount <= MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX ) && ( ucRegByteCount == ( UCHAR ) ( 2 * usRegCount ) ) )
      {
        ENTER_CRITICAL_SECTION(); 
        WriteHolding(usRegAddress, &pucFrame[6],ucRegByteCount/2);                
        EXIT_CRITICAL_SECTION();          
        WatchDog_reset();                 
        *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;  
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

#endif

#if (MB_FUNC_READ_HOLDING_ENABLED)
eMBException eMBFuncReadHoldingRegister(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT          usRegAddress = 0;
  USHORT          usRegCount   = 0;
  UCHAR           *pucFrameCur;
  UCHAR           i;
  eMBException    eStatus = MB_EX_NONE;
      
  if(*usLen == MB_PDU_FUNC_READ_SIZE)
  {
    usRegAddress = (USHORT)(pucFrame[1] << 8) | (USHORT)pucFrame[2];            // 
    usRegCount   = (USHORT)(pucFrame[3] << 8) | (USHORT)pucFrame[4];            // 
    if ((usRegCount + usRegAddress) <= HR_REGISTER_COUNT)                       // Проверка валидности адресса 
    {         
      if ((usRegCount >= 1) && (usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX))
      {
        pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];               /* Set the current PDU data pointer to the beginning. */
        *pucFrameCur++ = MB_FUNC_READ_HOLDING_REGISTER;         /* First byte contains the function code. */
        *pucFrameCur++ = (UCHAR)(usRegCount*2);                 /* Second byte in the response contain the number of bytes. */
        for (i=0;i<usRegCount;i++)
        {
          *pucFrameCur++ =  (UCHAR)(ReadHolding (i+ usRegAddress)>>8);
          *pucFrameCur++ =  (UCHAR)(ReadHolding(i+ usRegAddress) & 0x00FF);
        }            
        *usLen = 2 + usRegCount*2;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else    
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;    
  }
  return eStatus;
}
#endif


#if (MB_FUNC_READ_INPUT_ENABLED)
eMBException    eMBFuncReadInputRegister(UCHAR* pucFrame, UCHAR* usLen )
{
  USHORT        usRegAddress;
  USHORT        usRegCount;
  UCHAR         *pucFrameCur;
  UCHAR         i;
  eMBException  eStatus = MB_EX_NONE;
      
  if(*usLen == MB_PDU_FUNC_READ_SIZE)
  {
    usRegAddress = (USHORT)( pucFrame[1] << 8 ) | ( USHORT )pucFrame[2];      
    usRegCount   = (USHORT)( pucFrame[3] << 8 ) | ( USHORT )pucFrame[4]; 
    //Проверка валидности адресса  
    if ((usRegAddress  >= MB_INPUT_REG_START) && (usRegAddress < ( MB_INPUT_REG_START + IR_REGISTER_COUNT)) && ((usRegCount + usRegAddress) <= ( MB_INPUT_REG_START + IR_REGISTER_COUNT)))
    {         
      if((usRegCount >= 1) && (usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX))
      {
        pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];               /* Set the current PDU data pointer to the beginning. */
        *pucFrameCur++ = MB_FUNC_READ_INPUT_REGISTER;           /* First byte contains the function code. */
        *pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );          /* Second byte in the response contain the number of bytes. */
        for (i=0;i<usRegCount;i++)
        {
          *pucFrameCur++ = (UCHAR)(ReadInput(i+ usRegAddress) >> 8);
          *pucFrameCur++ = (UCHAR)(ReadInput(i+ usRegAddress) & 0x00FF);
        }                
        *usLen = 2 + usRegCount*2;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;      
    }
  }
  else    
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;    
  }
  return eStatus;
}
#endif

#if (MB_FUNC_READ_DISCRETE_INPUTS_ENABLED)
eMBException    eMBFuncReadDiscreteInputs(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT          usRegAddress;
  USHORT          usRegCount;
  UCHAR           *pucFrameCur;
  eMBException    eStatus = MB_EX_NONE;
      
  if(*usLen == MB_PDU_FUNC_READ_SIZE)
  {
    usRegAddress = (USHORT)(pucFrame[1] << 8) | (USHORT)pucFrame[2];      
    usRegCount   = (USHORT)(pucFrame[3] << 8) | (USHORT)pucFrame[4]; 
    //Проверка валидности адресса  
    if ((usRegAddress >= MB_DI_REG_START) && (usRegAddress < (MB_DI_REG_START + DI_REG_COUNT)) && ((usRegCount + usRegAddress) <= (MB_DI_REG_START + DI_REG_COUNT)))
    {         
      if((usRegCount >= 1) && (usRegCount <= MB_PDU_FUNC_READ_BITCNT_MAX))
      {
        pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];       /* Set the current PDU data pointer to the beginning. */
        *pucFrameCur++ = MB_FUNC_READ_DISCRETE_INPUTS;  /* First byte contains the function code. */
        if (usRegCount & 0x07)                          /* Second byte in the response contain the number of bytes. */
        {
          *pucFrameCur++ = (UCHAR)((usRegCount >> 3) + 1 );
          *usLen  = 2 + (UCHAR)((usRegCount >> 3) + 1);
        }
        else
        {
          *pucFrameCur++ = (UCHAR)(usRegCount >> 3);
          *usLen  = 2 + (UCHAR)(usRegCount >> 3);
        }
        ReadBitData(usRegAddress, pucFrameCur , usRegCount, DISCRET_INPUT) ;                             
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;      
    }
  }
  else    
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;    
  }
  return eStatus;
}
#endif

#if (MB_FUNC_READ_COILS_ENABLED)
eMBException    eMBFuncReadCoils(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT        usRegAddress;
  USHORT        usRegCount;
  UCHAR         *pucFrameCur;  
  eMBException  eStatus = MB_EX_NONE;
      
  if(*usLen == MB_PDU_FUNC_READ_SIZE)
  {
    usRegAddress = (USHORT)(pucFrame[1] << 8) | (USHORT)pucFrame[2];      
    usRegCount   = (USHORT)(pucFrame[3] << 8) | (USHORT)pucFrame[4]; 
    //Проверка валидности адресса  
    if ((usRegAddress >= MB_COILS_REG_START) && (usRegAddress < (MB_COILS_REG_START + CR_COILS_COUNT)) && ((usRegCount + usRegAddress) <= (MB_COILS_REG_START + CR_COILS_COUNT)))
    {         
      if((usRegCount >= 1) && (usRegCount <= MB_PDU_FUNC_READ_BITCNT_MAX))
      {
        pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];       /* Set the current PDU data pointer to the beginning. */
        *pucFrameCur++ = MB_FUNC_READ_COILS;            /* First byte contains the function code. */
        if (usRegCount & 0x07)                          /* Second byte in the response contain the number of bytes. */
        {
          *pucFrameCur++ = (UCHAR)((usRegCount >> 3) + 1);
          *usLen  = 2 + (UCHAR)((usRegCount >> 3) + 1);
        }
        else
        {
          *pucFrameCur++ = (UCHAR)(usRegCount >> 3);
          *usLen  = 2 + (UCHAR)(usRegCount >> 3);
        }
        ReadBitData(usRegAddress, pucFrameCur , usRegCount, COIL) ;                                                                  
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;      
    }
  }
  else    
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;    
  }
  return eStatus;
}
#endif

//Обработчик функции записи еденичного COIL

#if (MB_FUNC_WRITE_COIL_ENABLED)
eMBException    eMBFuncWriteCoil(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT        usRegAddress;
  USHORT        OutputValue;
  eMBException  eStatus = MB_EX_NONE;  
  //****************************************************************************
  OutputValue =(USHORT)(pucFrame[OUTPUT_VALUE_HI] << 8) | (USHORT)(pucFrame[OUTPUT_VALUE_LO]);                          // Проверка валидности выходных данных
  if ((*usLen == MB_PDU_FUNC_WRITE_SIZE) && ((OutputValue == 0x0000) || (OutputValue == 0xFF00)))
  {       
    usRegAddress = (USHORT)(pucFrame[STARTING_ADDRESS_HI] << 8) | (USHORT)(pucFrame[STARTING_ADDRESS_LO]);              // Проверка валидности адресса
    if ((usRegAddress >= MB_COILS_REG_START) && (usRegAddress < (MB_COILS_REG_START + CR_COILS_COUNT)))
    { 
      ENTER_CRITICAL_SECTION();                             
      WriteCoil(usRegAddress, OutputValue);
      EXIT_CRITICAL_SECTION();    
    } 
    else        
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;                
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif

// Обработчик записи большого кол-ва COIL

#if (MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED)
eMBException    eMBFuncWriteMultipleCoils(UCHAR* pucFrame, UCHAR* usLen)
{
  USHORT        usRegAddress;
  USHORT        usRegCount;
  USHORT        ucRegByteCount=0;
  eMBException  eStatus = MB_EX_NONE;
  //****************************************************************************
  usRegCount   = (USHORT)(pucFrame[QUANTITY_OF_OUTPUTS_HI] << 8) | (USHORT)(pucFrame[QUANTITY_OF_OUTPUTS_LO]);
  //Проверка валидности количества регистров          
  ucRegByteCount = usRegCount>>3;        // Согласно требования протокола, вычисляем необходимое кол-во байт для передчи 
  if (usRegCount & 0x0007)
  {
    ucRegByteCount++; //Указанного в PDU количества регистров     
  }
  if ((*usLen >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN )) &&
     (( usRegCount >= 1) && (usRegCount <= MB_PDU_FUNC_READ_BITCNT_MAX) && 
      ( pucFrame[BYTE_COUNT] == ucRegByteCount)))
  {
    //Проверка валидности адресса  
    usRegAddress = (USHORT)(pucFrame[STARTING_ADDRESS_HI] << 8) | (USHORT)(pucFrame[STARTING_ADDRESS_LO]);
    if ((usRegAddress >= MB_COILS_REG_START) && (usRegAddress < (MB_COILS_REG_START + CR_COILS_COUNT)) && ((usRegCount + usRegAddress) <= (MB_COILS_REG_START + CR_COILS_COUNT)))
    {                                
      ENTER_CRITICAL_SECTION(); 
      WriteCoils(usRegAddress, &pucFrame[6],usRegCount);                
      EXIT_CRITICAL_SECTION();          
      WatchDog_reset();                 
      *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;       
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;  
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;              
}
#endif

   
#if (MB_FUNC_DIAGNOSTICS_ENEBLED)
eMBException eMBFuncDiagnostics(UCHAR* pucFrame, UCHAR* usLen)
{  
  USHORT        SubFunctionCode = 0;
  eMBException  eStatus = MB_EX_NONE;  
  
  SubFunctionCode = (USHORT)(pucFrame[SUB_FUNCTION_HI] << 8) | (USHORT)(pucFrame[SUB_FUNCTION_LO]);
  switch (SubFunctionCode)
  {  
    case  RETURN_QUERY_DATA:                    
      break;
    case  RESTART_COMUNICATION_OPTION:
      eStatus = MB_EX_REINIT;
      break;            
    case  RETURN_DIAGNOSTIC_REGISTER:          
      break;
    case  FORCE_LISTEN_ONLY_MODE:
      ListenOnlyModeEnablae();
      break;    
    default:
      eStatus = MB_EX_ILLEGAL_FUNCTION;
      break;
  }
  return eStatus;  
}
#endif

//Функция изменения параметров канала RS
eMBException eMBFuncSetRSParametrs(UCHAR* pucFrame, UCHAR* usLen)
{
  eMBException  eStatus = MB_EX_REINIT;
  USHORT        Temp =0;
  Temp = (USHORT)(pucFrame[SUB_FUNCTION_HI] << 8) | (USHORT)(pucFrame[SUB_FUNCTION_LO]);
  switch (Temp)
  {  
    case SET_BOUNDRATE:
      // Проверяем, что заничение скорости предачи валидно, если да, то переписыаем новое значение в регистр инициализации и устанавливае флаг
      // по которому параметры канала будут измнены в оснвоном цикле
      Temp = pucFrame[SUB_FUNCTION_PARAM];
      if (Temp < 12 ) 
      {              
        WriteShortToUserRegister(0,Temp);                        
      }
      else 
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
      break;
    case  SET_STOP_BIT:
      Temp= pucFrame[SUB_FUNCTION_PARAM ];
      if (Temp < 3 ) 
      {                                                           
        WriteShortToUserRegister(1,Temp);
      }
      else 
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;          
      }
      break;
    default:
      eStatus = MB_EX_ILLEGAL_FUNCTION;
      break;
  }
  return eStatus;
}

