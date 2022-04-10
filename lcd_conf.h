/*
 * lcd_conf.h
 *
 *  Created on: Jul 13, 2016
 *      Author: simcons
 */

#ifndef SOURCES_LCD_CONF_H_
#define SOURCES_LCD_CONF_H_

#define RS1_ClrVal()       output_low(PIN_B3)            /* RS=0: command mode */
#define RS1_SetVal()       output_high(PIN_B3)             /* RS=1: data mode */
#define EN1_ClrVal()       output_low(PIN_B2)            /* EN=0 */
#define EN1_SetVal()       output_low(PIN_B2)             /* EN=1 */

#define DB41_SetOutput()               /* set data port as output */
#define DB51_SetOutput()                 /* set data port as output */
#define DB61_SetOutput()                /* set data port as output */
#define DB71_SetOutput()                /* set data port as output */

#define DB41_SetInput()               /* set data port as output */
#define DB51_SetInput()               /* set data port as output */
#define DB61_SetInput()               /* set data port as output */
#define DB71_SetInput()               /* set data port as output */

#define LCD_DB4   PIN_B1
#define LCD_DB5   PIN_D7
#define LCD_DB6   PIN_D6
#define LCD_DB7   PIN_D5

#define LCD_E     PIN_B2
#define LCD_RS    PIN_B3


uint8_t DB71_GetVal(void){
  return(input(PIN_D5));
}

uint8_t DB61_GetVal(void){
	 return(input(PIN_D6));
}

uint8_t DB51_GetVal(void){
	 return(input(PIN_D7));
}

uint8_t DB41_GetVal(void){
	 return(input(PIN_B1));
}

uint8_t DB71_PutVal(uint8_t val){
	output_bit(PIN_D5, val);
}

uint8_t DB61_PutVal(uint8_t val){
	output_bit(PIN_D6, val);
}

uint8_t DB51_PutVal(uint8_t val){
	output_bit(PIN_D7, val);
}

uint8_t DB41_PutVal(uint8_t val){
	output_bit(PIN_B1, val);
}



#endif /* SOURCES_LCD_CONF_H_ */
