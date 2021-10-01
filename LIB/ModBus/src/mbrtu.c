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
 * File: $Id: mbrtu.c,v 1.18 2007/09/12 10:15:56 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "user_process.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbrtu.h"
#include "mbcrc.h"
#include "stm32f10x.h"
#include "system.h"
#include "USART.h"
/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */
#define RECIVE_FRAME_BUFFER_MAX 3
/* ----------------------- Static variables ---------------------------------*/
static  UCHAR  ATUSendFrameBuf[MB_SER_PDU_SIZE_MAX];
static  UCHAR  CurrentATUFrameLength = 0;
static  UCHAR  ATUReciveFrameBuf[MB_SER_PDU_SIZE_MAX+2];
static USHORT  SndBufferCount;

MODBUSCOUNTERS ModBusCounters;


extern volatile uint16_t  Byte_Counter; // ???
extern volatile uint8_t * p_rx_buffer;  // ???
volatile uint8_t  FRAME_READY;


/* ----------------------- Start implementation -----------------------------*/

//-----------------------Проверка буфера на комуникационные ошибки-----------
void ParseATURecieverFrameBuffer(UCHAR NOK_counter)
{
  USHORT FrameLength = 0;
  USHORT FrameCRC    = 0;
  UCHAR * pUARTBuffer;
  //****************************************************************************
  WatchDog_reset();
  FrameLength = GetATUByteCounter();
  if (!NOK_counter)
  {     
    if (FrameLength > 3)
    {
      pUARTBuffer = Get_RX_Buffer();                                                            // Копируем буфер приема
      FrameCRC = ((USHORT)pUARTBuffer[FrameLength-1])<<8 | (USHORT)pUARTBuffer[FrameLength-2];  // Забираем CRC из сообщения
      if ( usMBCRC16( pUARTBuffer, FrameLength - 2) == FrameCRC)                                // Сравниваем расчетное CRC и присланное
      {
        for (int i=0;i< FrameLength-2;i++)              //Записываем приемный буфер в промежуточный буфер обработки
        {
          ATUReciveFrameBuf[i] =  pUARTBuffer[i];                
        }
        FRAME_READY           = 1;
        CurrentATUFrameLength = FrameLength-2;
        ModBusCounters.BusMessageCount++;
        return;
      }
    }
  }
  ModBusCounters.BusCommunicationErrorCount++;
  return;
}

MBErrorCode MBRTUReceive(UCHAR *pucRcvAddress, UCHAR *pucFrame, UCHAR *pusLength)
{
  MBErrorCode    eStatus = NO_VALID_FRAME;
  WatchDog_reset();
  if (FRAME_READY)
  {
    if((ATUReciveFrameBuf[0] == GetCurSlaveAdr()) || (ATUReciveFrameBuf[0] == MB_ADDRESS_BROADCAST))
    {  
      *pusLength = CurrentATUFrameLength-1;                                     // Записываем длину фрейма в промежуточный буфер обработки
      *pucRcvAddress = ATUReciveFrameBuf[0];                                    // Записываем адресс устройства в промежуточный буфер обработки
      for (int i=0;i<*pusLength;i++)
      {     
        pucFrame[i] = ATUReciveFrameBuf[i+1];                                   // Записываем фрейм в промежуточный буфер обработки
      }   
      ModBusCounters.SlaveMessageCount++;  
      if (*pucRcvAddress == MB_ADDRESS_BROADCAST )
      {
         ModBusCounters.SlaveNoResponseCount++;    
      }
      eStatus = VALID_FRAME;                
    }
    FRAME_READY = 0;
  }
  return eStatus ;  
}


void MBRTUSend(UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength)
{
  USHORT          usCRC16;   
  //Фрейм для передачи формируем в буфере ATUSendFrameBuf
  ATUSendFrameBuf[0] = * pucRcvAddress;
  SndBufferCount =*pusLength + 1;  
  for (UCHAR i=1 ;i<SndBufferCount;i++)
  {
    ATUSendFrameBuf[i]=pucFrame[i-1];
  }          
  /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
  usCRC16 = usMBCRC16(  ATUSendFrameBuf, SndBufferCount );
  ATUSendFrameBuf[SndBufferCount++] = ( UCHAR )( usCRC16 & 0x00FF );
  ATUSendFrameBuf[SndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );
  put_str_pak(ATUSendFrameBuf,SndBufferCount);
  return;
}

MBErrorCode MBMasterRTUSend(UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength )
{
  USHORT        usCRC16;   
  UCHAR         i;
  MBErrorCode eStatus = MB_ENOERR;

  if ((GetRcvState() == STATE_M_RX_IDLE ) && (GetSendState() == STATE_TX_IDLE )) 
  {
    //Фрейм для передачи формируем в буфере ATUSendFrameBuf
    ATUSendFrameBuf[0] = *pucRcvAddress;
    SndBufferCount =((USHORT)*pusLength) + 1;  
    for (i=1;i<SndBufferCount;i++)
    {
      ATUSendFrameBuf[i]=pucFrame[i-1];
    }          
    /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
    usCRC16 = usMBCRC16(  ATUSendFrameBuf, SndBufferCount );
    ATUSendFrameBuf[SndBufferCount++] = ( UCHAR )( usCRC16 & 0x00FF );
    ATUSendFrameBuf[SndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );      
    put_str_pak_master(ATUSendFrameBuf,SndBufferCount+2);
  }
  else
  {
    eStatus = MB_EIO;
  }
  return eStatus;
}