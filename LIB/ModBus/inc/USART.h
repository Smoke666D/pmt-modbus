#ifndef __UASART_H
#define __USART_H
//******************************************************************************
//*****************************INCLUDE******************************************
//******************************************************************************
#include "port.h"
#include "user_process.h"
#include "DIO.h"
//******************************************************************************
//***************************Define*********************************************
//******************************************************************************

//***************************USART**********************************************
#define         UART_1200       0
#define         UART_2400       1
#define         UART_4800       2
#define         UART_9600       3
#define         UART_14400      4
#define         UART_19200      5
#define         UART_38400      6
#define         UART_56000      7
#define         UART_57600      8
#define         UART_115200     8
#define         UART_128000     9
#define         UART_256000     10

#define         STOP_BIT_2      0
#define         PARITY_EVEN     1           
#define         PARITY_ODD      2

//Типы, описывающие конечные автоматы примника и передатчика
typedef enum
{
    STATE_RX_INIT,      /*!< Receiver is in initial state. */
    STATE_RX_IDLE,      /*!< Receiver is in idle state. */
    STATE_RX_RCV,       /*!< Frame is beeing received. */
    STATE_RX_ERROR,     /*!< If the frame is invalid. */
    STATE_M_RX_INIT,    /*!< Receiver is in initial state. */
    STATE_M_RX_IDLE,    /*!< Receiver is in idle state. */
    STATE_M_RX_RCV,     /*!< Frame is beeing received. */
    STATE_M_RX_ERROR    /*!< If the frame is invalid. */
} eMBRcvState;

typedef enum
{
    STATE_TX_IDLE,      /*!< Transmitter is in idle state. */
    STATE_TX_XMIT,      /*!< Transmitter is in transfer state. */
    STATE_M_TX_IDLE,    /*!< Transmitter is in idle state. */
    STATE_M_TX_XMIT,    /*!< Transmitter is in transfer state. */
    STATE_M_TX_XFWR
} eMBSndState;





//****************************Function****************************************
void PutCharUART2 (UCHAR DATA);
void ResetATUByteCounter(void);
unsigned short GetATUByteCounter(void);
void Start0_5Timer();
uint8_t * Get_RX_Buffer(void);
void Put_Rx_Buffer(void);
void USART1_init(void);                                 // Инициализация USART
uint8_t get_char(uint8_t *data);
void put_str_pak(UCHAR *s, uint16_t c);
void put_str_pak_master(UCHAR *s, uint16_t c);
void put_str(UCHAR *s);
void put_int(int32_t data);
eMBSndState GetSendState(void);
eMBRcvState GetRcvState(void);
void USART1_IRQHandler(void);                           // Прерывания по USART1
extern unsigned long BoundRateTable[12];
//******************************************************************************
void Out_Ind_R(uint8_t data);
void Out_Ind_G(uint8_t data);
void BLINK_INd(void);
void SetPeriod(unsigned long ms);

#if  UART2_INIT > 0
  void USART2_init(void);
#endif

#endif /*__USART_H */