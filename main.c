#include "port.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_conf.h"
#include "init.h"
#include "system.h"
#include "mb.h"
#include "user_process.h"


void  main()
{ 
  Init();
  while(1)
  {   
    WatchDog_reset(); 
    eMBPoll();
  }
}
