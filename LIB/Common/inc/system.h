#ifndef __sysyem_H
#define __sysyem_H
//***************************MACROS********************************************
#define         INVERT_BYTE(a)   ((a&1)<<7)|((a&2)<<5)|((a&4)<<3)|((a&8)<<1)|((a&16)>>1)|((a&32)>>3)|((a&64)>>5)|((a&128)>>7)
#define         FLAOT2PF(fl)     (uint32_t*)(&fl)
#define         PF2FLOAT(data)   *((float*)(&data))
//****************************Function*****************************************
void Delay_ms(uint32_t ms);     // Задержка простая
void WatchDog_start(void);      // Запуск ватчдога
void WatchDog_reset(void);      // Сброс ватчдога
//*****************************************************************************
#endif /*__sysyem_H */