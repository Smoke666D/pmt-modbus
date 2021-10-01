#ifndef _PORT_H
#define _PORT_H
//--------------------------------Определения типов данны---------------------
#define         ARM_base

#ifdef ARM_base
  #include      "stdint.h"                      // Определение ARM типов
  #define       BOOL            uint8_t;
  #define       UCHAR           uint8_t
  #define       CHAR            int8_t
  #define       USHORT          uint16_t
  #define       SHORT           int16_t
  #define       ULONG           uint32_t
  #define       LONG            int32_t
#else                                           // Определение стандартных типов
  typedef       char            BOOL;
  typedef       unsigned char   UCHAR;
  typedef       char            CHAR;
  typedef       unsigned short  USHORT;
  typedef       short           SHORT;
  typedef       unsigned long   ULONG;
  typedef       long            LONG;
#endif
/* ----------------------- Platform includes --------------------------------*/
/* ----------------------- Defines ------------------------------------------*/
#define MB_ADDRESS_BROADCAST                    0       /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN                          1       /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX                          247     /*! Biggest possible slave address. */
#define MB_FUNC_NONE                            0
#define MB_FUNC_READ_COILS                      1
#define MB_FUNC_READ_DISCRETE_INPUTS            2
#define MB_FUNC_WRITE_SINGLE_COIL               5
#define MB_FUNC_WRITE_MULTIPLE_COILS            15
#define MB_FUNC_READ_HOLDING_REGISTER           3
#define MB_FUNC_READ_INPUT_REGISTER             4
#define MB_FUNC_WRITE_REGISTER                  6
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS        16
#define MB_FUNC_MASK_WRITE_REGISTER             22
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS    23
#define MB_FUNC_DIAG_READ_EXCEPTION             7
#define MB_FUNC_DIAGNOSTICS                     8
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT          11
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG          12
#define MB_FUNC_OTHER_REPORT_SLAVEID            17
#define MB_FUNC_SET_COMMUNICATION_PARAMETRS     25
#define MB_FUNC_ERROR                           128
#define MB_FUNC_SET_RS_PARAMETRS                66

//Коды SUB-FUNCTION
#define RETURN_QUERY_DATA                       0
#define RESTART_COMUNICATION_OPTION             1
#define RETURN_DIAGNOSTIC_REGISTER              2
#define FORCE_LISTEN_ONLY_MODE                  4
#define SET_BOUNDRATE                           0
#define SET_STOP_BIT                            1
/* ----------------------- Type definitions ---------------------------------*/
#define MB_FUNC_HANDLERS_MAX                    17
#define MB_FUNC_OTHER_REP_SLAVEID_BUF           32


#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED       0       /*! \brief If the <em>Report Slave ID</em> function should be enabled. */
#define MB_FUNC_READ_INPUT_ENABLED              1       /*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MB_FUNC_READ_HOLDING_ENABLED            1       /*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MB_FUNC_WRITE_HOLDING_ENABLED           1       /*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  1       /*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MB_FUNC_READ_COILS_ENABLED              1       /*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_COIL_ENABLED              1       /*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED    1       /*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED    1       /*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MB_FUNC_READWRITE_HOLDING_ENABLED       0       /*! \brief If the <em>Read/Write Multiple Registers</em> function should be enabled. */
#define MB_FUNC_MASK_WRITE_REGISTER_ENABLED     1
#define MB_FUNC_DIAGNOSTICS_ENEBLED             1

 typedef enum
{
    MB_EX_NONE                 = 0x00,
    MB_EX_ILLEGAL_FUNCTION     = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE   = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE          = 0x05,
    MB_EX_SLAVE_BUSY           = 0x06,
    MB_EX_MEMORY_PARITY_ERROR  = 0x08,
    MB_EX_GATEWAY_PATH_FAILED  = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED   = 0x0B,
    MB_EX_REINIT               = 0xFF,
} eMBException;

typedef eMBException(*pxMBFunctionHandler) (UCHAR* pucFrame, UCHAR* pusLength);

typedef struct
{
    pxMBFunctionHandler pxHandler;
} MBFunctionHandlerTable;


/* ----------------------- Defines ------------------------------------------*/
#define UART_RECIEVE_BUF_SIZE   2

#define MB_PDU_SIZE_MAX         253     /*!< Maximum size of a PDU. */
#define MB_PDU_SIZE_MIN         1       /*!< Function Code */
#define MB_PDU_FUNC_OFF         0       /*!< Offset of function code in PDU. */
#define MB_PDU_DATA_OFF         1       /*!< Offset for response data in PDU. */
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

#define ENTER_CRITICAL_SECTION( )   
#define EXIT_CRITICAL_SECTION( )    
#define F_CPU		        16000000UL

#define assert( x )

#ifndef TRUE
  #define TRUE            1
#endif

#ifndef FALSE
  #define FALSE           0
#endif

#endif