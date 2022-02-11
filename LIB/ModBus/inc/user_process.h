#ifndef _MB_USER_PROCESS_H
#define _MB_USER_PROCESS_H
//*******************************INCLUD*****************************************
#include "port.h"
//******************************FUNCTIONS***************************************
uint8_t GetSwitsh(void);
UCHAR GetCurSlaveAdr(void);

#define         BIG             0x01
#define         BIC             0x02
#define         BKC             0x04
#define         IT14            0x06
#define         TSP_SH          0x07
#define         BKC14           0x08
#define         KMG18           0x09
#define         BKC01           0x0A

#define         device          BKC01
//******************************************************************************
#if (device == 0)

#define         HOLDING_REGISTER_RAM_COUNT      1
#define         HOLDING_REGISTER_ROM_COUNT      1
#define         REGISTER_DEFAULT_VAL            {0, 0}
#endif
//******************************************************************************
#define         RECODING_TIBLE_SIZE             20
//******************************************************************************
#if (device == BIG)
  #define         HOLDING_REGISTER_RAM_COUNT      2
  #define         HOLDING_REGISTER_ROM_COUNT      3 + RECODING_TIBLE_SIZE
  #define         REGISTER_DEFAULT_VAL            {0x0123,0x0000,0x000F,0x000F,0x000F}
#endif
//******************************************************************************
#if (device == BIC)

#define         HOLDING_REGISTER_RAM_COUNT      20
#define         HOLDING_REGISTER_ROM_COUNT      49
#define         REGISTER_DEFAULT_VAL            {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,  0x00FF,    0x004C, 0x3A29, 0x9DF8,    0x004C, 0x4A27, 0x44F8};
//___________________________________________________________________________________________—игменты в дисплее_________________________________________SYS_______________Betta________________________Ro__________
#endif
//******************************************************************************
#if (device == BKC)

#define         HOLDING_REGISTER_RAM_COUNT      2
#define         HOLDING_REGISTER_ROM_COUNT      5
#define         REGISTER_DEFAULT_VAL            {0, 0, 9, 999, 1, 1, 0}
#endif
//******************************************************************************
#if (device == IT14)

#define         HOLDING_REGISTER_RAM_COUNT      8
#define         HOLDING_REGISTER_ROM_COUNT      0
#define         REGISTER_DEFAULT_VAL            {0, 0, 0, 0, 0, 0, 0, 0}

#endif
//******************************************************************************
#if (device == BKC14)

#define         HOLDING_REGISTER_RAM_COUNT      7
#define         HOLDING_REGISTER_ROM_COUNT      0
#define         REGISTER_DEFAULT_VAL            {0, 0, 0, 0, 0, 0, 0}

#endif
//******************************************************************************
#if (device == TSP_SH)

#define         HOLDING_REGISTER_RAM_COUNT      8
#define         HOLDING_REGISTER_ROM_COUNT      0
#define         REGISTER_DEFAULT_VAL            {0, 0, 0, 0, 0, 0, 0, 0}

#endif

//******************************************************************************
#if (device == KMG18)

#define         HOLDING_REGISTER_RAM_COUNT      3
#define         HOLDING_REGISTER_ROM_COUNT      1
#define         REGISTER_DEFAULT_VAL            {0, 0, 0, 0x0FFF}

#endif
//******************************************************************************
#if (device == BKC01)

#define         HOLDING_REGISTER_RAM_COUNT      2
#define         HOLDING_REGISTER_ROM_COUNT      4
#define         REGISTER_DEFAULT_VAL            {0x322B, 0xCC77, 0, 0xFFFF, 0x322B, 0xCC77}

#endif
//******************************************************************************

#endif
