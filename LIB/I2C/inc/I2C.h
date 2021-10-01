#ifndef __I2C_H
#define __I2C_H
//*****************************************************************************
//****************************I2C**********************************************
//*****************************************************************************
#define		I2C_ADR16		        0                       // ���������� � ���������� 8 ��� ��� 16 ���
#define         I2C_IRQ_DIS                     1                       // ��������� ����������� �� ����� ���������� ���������� SPI
#define         I2C_ADR_SEND_MSK                ((uint16_t)0x0001)      // �������� �� ������
#define         I2C_MAX_SPEED                   100000
//***************************I2C_CR********************************************
#define         I2C_CR_MASTER                   0x01
#define         I2C_CR_REMAP                    0x02
//**************************I2C_USER*******************************************
#define		AdrSend_msk		        ((uint8_t)0x01)         // ���� ��������� ����������� ������ ������
#define		DataSend_msk		        ((uint8_t)0x02)         // ���� ��������� �������� ������ �� ������
#define		ReadMode_msk		        ((uint8_t)0x04)         // ���� ������� ������
#define		Busy_msk		        ((uint8_t)0x08)         // ���� ��������� ������
#define         ReStart_msk                     ((uint8_t)0x10)         // ���� ������������� �����������
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
#define         I2C_DebugMode                   1                       // ������� ��������� ���� �������� ����������
#define		I2C_MaxOnLine	                10                      // ������������ ���-�� ��������� �� �����
#define		I2C_BUFFER_SIZE	                13                      // ������ �������
#define         I2C_TIMOUT                      100                     // ���, �������� ����� �����������, ������ � ����� �������
//****************************Function*****************************************
uint8_t  I2C_setup(uint8_t nomber, uint16_t speed, uint8_t master, uint8_t remap);      // ���������� I2C ����������
void     I2C_init(void);                                                                // ������������� I2C
void     I2C_Write(uint8_t* HwAdr, uint16_t* adr, uint8_t* data, uint16_t len);         // ������ ������� ������
void     I2C_Read(uint8_t* HwAdr, uint16_t* adr, uint16_t len);                         // ������ ������� ������
uint8_t* I2C_GetData(void);                                                             // �������� ���������� ������
uint8_t  I2C_GetBusy(void);                                                             // ��������� ������� ��������� ����������
void     I2C_Proc(void);                                                                // �������� �������� - !!!!��������� � ������!!!!
uint8_t  I2C_EE_ByteRead(uint8_t HwAddr, uint16_t ReadAddr);
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
#endif /*__I2C_H */