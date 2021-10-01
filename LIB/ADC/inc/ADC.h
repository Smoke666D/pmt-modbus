#ifndef __ADC_H
#define __ADC_H
//****************************ADC**********************************************
#define		ADC			ADC1
#define         AIN0       		0x00		// 0-ой канал
#define         AIN1       		0x01		// 1-ый канал
#define		AINT			0x10		// 16-ый канал - температура кристала
#define		AINV			0x11		// 17-ый канал - опорное напряжение
//***************************GPIO**********************************************
#define         AIN0_pin                GPIO_Pin_0
#define         AIN1_pin                GPIO_Pin_1
#define         AIN_port     	        GPIOA
//***************************Define********************************************
#define		ADC_DataMask	        0x0FFF
#define		ADC_CH_NUM		4	        // Кол-во используемых каналов
//***************************OUTPUT********************************************
#define         Vref                    3.3
#define         Kf0                     9.09
#define         Kf1                     2
//***************************TERMO*********************************************
#define         V25                     1.43    // V
#define         AvgSlope                4.3     // mV/'C
//***************************MACROS********************************************
#define		ADC_START		ADC->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG
#define         ADC_SET_CH(ch)          ADC->SQR3 = (ADC->SQR3 & ~ADC_SQR3_SQ1)|(uint32_t)ch;
#define		ADC_FINISH		(ADC->SR & ADC_SR_EOC)
#define		ADC_SR_CLC		ADC->SR = 0x00000000
#define         ADC_CALC_TEMP(AIN)      (((float)AIN*Vref/0x0FFF)-V25)/AvgSlope+25.0   
//****************************Function*****************************************
uint8_t ADC_init(void);			// Инициализация АЦП. 1-успех
void    ADC_StartScan(void);		// Запустить сканирование каналов
uint8_t ADC_ConvEnd(void);		// Возвращает статус конца измернеий 1 - окончены
void    ADC_GetData(uint16_t* data);	// Возвращает массив измерений
//*****************************************************************************
#endif /*__ADC_H */