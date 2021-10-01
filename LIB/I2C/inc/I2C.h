#ifndef __I2C_H
#define __I2C_H
//*****************************************************************************
//****************************I2C**********************************************
//*****************************************************************************
#define		I2C_ADR16		        0                       // Адрессация в устройстве 8 бит или 16 бит
#define         I2C_IRQ_DIS                     1                       // Выключать прерываниея во время исполнения прерывания SPI
#define         I2C_ADR_SEND_MSK                ((uint16_t)0x0001)      // Апендикс на запись
#define         I2C_MAX_SPEED                   100000
//***************************I2C_CR********************************************
#define         I2C_CR_MASTER                   0x01
#define         I2C_CR_REMAP                    0x02
//**************************I2C_USER*******************************************
#define		AdrSend_msk		        ((uint8_t)0x01)         // Флаг окончания отправления адресс памяти
#define		DataSend_msk		        ((uint8_t)0x02)         // флаг окончания отправки данных на запись
#define		ReadMode_msk		        ((uint8_t)0x04)         // Флаг команды чтения
#define		Busy_msk		        ((uint8_t)0x08)         // Флаг занятости канала
#define         ReStart_msk                     ((uint8_t)0x10)         // флаг необходимости перезапуска
//*************************I2C_STATUS******************************************
#define         I2C_FLAG_MSK                    0x00FFFFFF
#define         I2C_STAT_BUSY                   0x00030000
#define         I2C_STAT_STOP                   0x00000000
#define         I2C_STAT_START                  0x00030001              // MSL,BUSY             SB
#define         I2C_STAT_HWADR_WRITE_SENT       0x00070082              // MSL,BUSY,TRA         ADDR,TxE
#define         I2C_STAT_HWADR_READ_SENT        0x00030002              // MSL,BUSY             ADDR
#define         I2C_STAT_DATA_SENT              0x00070084              // MSL,BUSY,TRA         BTF,TxE
#define         I2C_STAT_DATA_READY_GET         0x00030044              // MSL,BUSY             BTF,RxNE
#define         I2C_STAT_DATA_HAVE_GET          0x00000040              // MSL,BUSY             RxNE
//***************************Define********************************************
#define         I2C_DebugMode                   1                       // Ведение дебажного лога статусов интерфейса
#define		I2C_MaxOnLine	                10                      // Максимальное кол-во устройств на линии
#define		I2C_BUFFER_SIZE	                13                      // Размер буффера
#define         I2C_TIMOUT                      100                     // мкс, задержка между рестаратами, стопом и новым стартом
//****************************Function*****************************************
uint8_t  I2C_setup(uint8_t nomber, uint16_t speed, uint8_t master, uint8_t remap);      // Объявление I2C устройства
void     I2C_init(void);                                                                // Инициализация I2C
void     I2C_Write(uint8_t* HwAdr, uint16_t* adr, uint8_t* data, uint16_t len);         // Запись массива данных
void     I2C_Read(uint8_t* HwAdr, uint16_t* adr, uint16_t len);                         // Чтение массива данных
uint8_t* I2C_GetData(void);                                                             // Получить полученные данные
uint8_t  I2C_GetBusy(void);                                                             // Получение статуса занятости устройства
void     I2C_Proc(void);                                                                // Контроль рестарта - !!!!запихнуть в таймер!!!!
uint8_t  I2C_EE_ByteRead(uint8_t HwAddr, uint16_t ReadAddr);
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
#endif /*__I2C_H */