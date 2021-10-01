#ifndef _DIO_H
#define _DIO_H
//********************************MACROS****************************************
#define         DIN_READ(port,pin)      (port->IDR&pin)
#define         DOUT_SET(port,pin)      port->BSRR |= pin
#define         DOUT_RESET(port,pin)    port->BRR  |= pin
#define         DOUT_INVERT(port,pin)   port->ODR  ^= pin
//******************************FUNCTIONS***************************************
void DOUT_Init(uint16_t pin, GPIO_TypeDef* port);
void DIN_Init(uint16_t pin, GPIO_TypeDef* port);
//******************************************************************************
#endif