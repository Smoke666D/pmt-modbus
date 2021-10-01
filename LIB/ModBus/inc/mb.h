#ifndef _MB_H
#define _MB_H
#include "port.h"

#define MAX_FUN_NUM             44

#define STARTING_ADDRESS_HI     1
#define STARTING_ADDRESS_LO     2
#define QUANTITY_OF_OUTPUTS_HI  3
#define QUANTITY_OF_OUTPUTS_LO  4
#define OUTPUT_VALUE_HI         3
#define OUTPUT_VALUE_LO         4
#define BYTE_COUNT              5
#define AND_MASK_HI             3
#define AND_MASK_LO             4
#define OR_MASK_HI              5
#define OR_MASK_LO              6
#define SUB_FUNCTION_HI         1
#define SUB_FUNCTION_LO         2
#define SUB_FUNCTION_PARAM      3

void    eMBPoll( void );
void    InitMB(void);
UCHAR   GetRequestAdress(void);
UCHAR   IfMasterMode(void);
void    SetMasterMode(void);
void    ListenOnlyModeEnablae(void);
#endif
