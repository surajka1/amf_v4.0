#ifdef pic_4620
   #include <18F4620.h>
#endif

#ifdef pic_k22
   #include <18F46k22.h>
#endif

#device ADC=10
#use delay(internal=16MHz)

#fuses PUT
#fuses NOPBADEN                 //PORTB pins are configured as digital I/O on RESET
#fuses NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
//#FUSES NOXINST                  //Extended set extension and Indexed Addressing mode disabled (Legacy mode)
#fuses PROTECT
#fuses NOBROWNOUT
#ZERO_RAM

#use i2c(Master,Slow,sda=PIN_C4,scl=PIN_C3,stream = DS1338, restart_wdt)
