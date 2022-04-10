#case

//#define amfv34cd4051
#define amfv40cd4051

#ifdef amfv34cd4051
   #undef amfv40cd4051
   #define amfcd4051
#endif

#ifdef amfv40cd4051
   #undef amfv34cd4051
   #define amfcd4051
#endif

//#define pic_k22

#ifndef pic_k22
   #define pic_4620
#endif

//#define V2RELAYCARD

#define HIDE_TIME

#include <main.h>
//#include <virtual_eeprom.c>
#include <stdint.h>
#include <time.h>
#include "time.c"
#include <internal_eeprom.c>
#include "ds1338.c"

#include "flex_lcd.c"
#include "declarations.c"
#include "sw_defines.c"
#include "ioconfig.c"
#include "myfunctions.c"
#include "uifunctions.c"
#include "eventlog_pic.c"
#include "dgfunctions.c"
#include "check_conditions.c"
#include "swinit.c"
#include "events.c"
#include "mainsfunctions.c"
#include "settings.c"
#include "processio.c"
#include "rom.c"

void main()
{
   volatile uint32_t temp32;
   volatile uint16_t temp16;
//!   uint8_t temp8;
//!   uint16_t data[16];
   peripheralInit();
   amfWdogRestart();  
   amf_sw_init();
   amfWdogRestart();  
   BOARD_IND_OFF();

   //dummy read to point the address away to an
   //address that is not used.
   Eprom_Read16(10, &temp16);
   amfFlashRead32(FLASH_EE_START, 152, &temp32);

   while(TRUE)
   {

      BOARD_IND_TOGGLE();
      amfWdogRestart();
      processIO();

   }

}


