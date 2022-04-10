/*
For amf v3.4, ie with 4051 addon card.
Changes.
1. S3 is now analog input pin.
   Only s0, s1, s2 used as select pins.
   S3 = RE0/AN5
   and A1 is Remote start input
   Since AN0|AN5 combination does not work, AN5 and AN1 should be swapped.
   ie, by cutting wires.
   So ultimately, 
   Remote start is connected to RE0
2. inputs channels changed.


*/




#define CRANK            PIN_D1

#define DGCONTACTOR      PIN_D2

#define MAINSCONTACTOR   PIN_D3

#define DGSTOP            PIN_E2

#define FUELSOLENOID      PIN_E2

#define RELAY5            PIN_A4      //buzzer

#define FAULTINDICATOR   PIN_D4

#define PIN_BOARD_LED   PIN_E1

#define YELLOW_LED      PIN_C0 

#define S0     PIN_A2

#define S1     PIN_A3

#define S2     PIN_A5

#ifndef amfcd4051
   #define S3     PIN_E0
#endif

#ifdef V2RELAYCARD
	#define EB_CONTACTOR_OFF() output_low(MAINSCONTACTOR)
	#define EB_CONTACTOR_ON()   output_high(MAINSCONTACTOR)
#else
	#define EB_CONTACTOR_ON() output_low(MAINSCONTACTOR)
	#define EB_CONTACTOR_OFF()   output_high(MAINSCONTACTOR)
#endif


#define DG_CONTACTOR_ON()   output_high(DGCONTACTOR)
#define DG_CONTACTOR_OFF()   output_low(DGCONTACTOR)

#define FUEL_SOL_ON()  output_high(FUELSOLENOID)
#define FUEL_SOL_OFF()   output_low(FUELSOLENOID)

#define CRANK_ON()   output_high(CRANK)
#define CRANK_OFF() output_low(CRANK)

#define GContact_GetVal()      input_state(DGCONTACTOR)
#define Relay5_GetVal()         input_state(RELAY5)
#define STATUS_FUEL_SOL()      input_state(FUELSOLENOID)
#define STATUS_EB_CONTACT()   MContact_GetVal()
#define STATUS_DG_CONTACT()   input_state(DGCONTACTOR)

#define BUZZER_ON()   output_high(RELAY5)
#define BUZZER_OFF()   output_low(RELAY5)

#define MANUAL_LED_ON()    output_high(YELLOW_LED)
#define MANUAL_LED_OFF()   output_low(YELLOW_LED)
#define MANUAL_LED_TOGGLE()  output_toggle(YELLOW_LED)

#define FAULT_IND_ON()         output_high(PIN_D4)
#define FAULT_IND_OFF()         output_low(PIN_D4)
#define FAULT_IND_TOGGLE()      output_toggle(PIN_D4)
#define RedLED_GetVal()         input_state(PIN_D4)
#define FAULT_IND_GET_VAL()   input_state(PIN_D4)

#define BOARD_IND_ON()      output_low(PIN_BOARD_LED)
#define BOARD_IND_OFF()      output_high(PIN_BOARD_LED)
#define BOARD_IND_TOGGLE()   output_toggle(PIN_BOARD_LED)
#define BOARD_IND_PUT_VAL(x)   output_bit(PIN_BOARD_LED, x)

#define GreenLED_PutVal(x)   output_bit(PIN_BOARD_LED, x)


uint8_t MContact_GetVal(void){
#ifdef V2RELAYCARD
	return((input_state(MAINSCONTACTOR) == 0)? 0:1);
#else
   return((input_state(MAINSCONTACTOR) == 0)? 1:0);
#endif
}


/**************************** End of LCD definitions ********************************************/
#ifdef amfv34cd4051
     
   #define CHANNEL_HT         3       
   #define CHANNEL_FLT1         0// there is no pin for RSTART , so flt1 is mapped to ht, and rstart mapped to channel 6//6
   #define CHANNEL_DGR         1
   #define CHANNEL_DGY         2
   #define CHANNEL_DGB         5
   #define CHANNEL_EBR         7
   #define CHANNEL_EBY         6
   #define CHANNEL_EBB         4
     
   #define CHANNEL_EMSTOP      12
   #define CHANNEL_LLOP         14
   #define CHANNEL_RSTART      15   //should be  changed to some other unused channel , say 8 or 9
   #define CHANNEL_REMOTE      13
   #define CHANNEL_DGBAT      10
   #define CHANNEL_BTS         8
   #define CHANNEL_FUEL         11
     
#elif defined (amfv40cd4051)

   #define CHANNEL_HT         12       
   #define CHANNEL_FLT1         0// there is no pin for RSTART , so flt1 is mapped to ht, and rstart mapped to channel 6//6
   #define CHANNEL_DGR         1
   #define CHANNEL_DGY         2
   #define CHANNEL_DGB         5
   #define CHANNEL_EBR         7
   #define CHANNEL_EBY         6
   #define CHANNEL_EBB         4
     
   #define CHANNEL_EMSTOP      14
   #define CHANNEL_LLOP         3
   #define CHANNEL_RSTART      13   //should be  changed to some other unused channel , say 8 or 9
   #define CHANNEL_REMOTE      15
   #define CHANNEL_DGBAT      10
   #define CHANNEL_BTS         9
   #define CHANNEL_FUEL         11

#else 

   #define CHANNEL_EBR         5
   #define CHANNEL_EBY         6
   #define CHANNEL_EBB         7
     
   #define CHANNEL_DGR         2
   #define CHANNEL_DGY         3
   #define CHANNEL_DGB         4
     
   #define CHANNEL_RSTART      10   //should be  changed to some other unused channel , say 8 or 9
   #define CHANNEL_FLT1         1// there is no pin for RSTART , so flt1 is mapped to ht, and rstart mapped to channel 6//6
   #define CHANNEL_HT         0
   #define CHANNEL_LLOP         9
   #define CHANNEL_REMOTE      11
   #define CHANNEL_EMSTOP      8
   #define CHANNEL_FUEL         15
     
   #define CHANNEL_BTS         14
   #define CHANNEL_DGBAT      12

#endif

#ifdef amfcd4051
   #define PIN_REMOTE_START   PIN_E0
#else
   #define PIN_REMOTE_START   PIN_A1
#endif
   

//#define CHANNEL_INT_TEMP   22
//#define CHANNEL_BANDGAP      23


#define AC_DIVIDE_RATIO   6.5

#define KEY_MENU            1
#define KEY_SET            1
#define KEY_UP               9
#define KEY_DOWN            5
#define KEY_EXIT            13
#define KEY_MANUAL         13
#define KEY_START            3
#define KEY_STOP            11
#define KEY_DGC            7
#define KEY_MC               15
#define KEY_RESET            16
//key combinations
#define KEY_TIME            25         //reset + up
#define KEY_HARD_RESET      23      //reset + dg
#define KEY_NONE            0

#define KEY_PRESS_DELAY 10      //10ms


#define EEPROM_START   0

//#define FLASH      0x0000FDFFU
//#define FLASH      64000

//#define FLASH_EE_START 0x1000
#define FLASH_EE_START 0xFE00



void select(uint8_t x)
{   
   uint8_t temp8;
   temp8 = x;
   output_bit(S0,shift_right(&x,1,0));
   output_bit(S1,shift_right(&x,1,0));
   output_bit(S2,shift_right(&x,1,0));
   
   #ifdef amfcd4051
   if(temp8 < 8)
     set_adc_channel(0);
   else
     set_adc_channel(1);
   #else
   output_bit(S3,shift_right(&x,1,0));
   #endif

   delay_ms(1);
} 

uint16_t ReadADC_AMF_60us(uint8_t adc_channel)
{
      uint16_t temp_adc_value;
        
      select(adc_channel); 
      temp_adc_value = read_adc();
      delay_us(60);
      return(temp_adc_value);
}

uint16_t ReadAveADC_10ms(uint8_t adc_channel)
{
   uint32_t   temp_adc_value =0;
   uint8_t temp_count;
   select(adc_channel); 
   for(temp_count = 0; temp_count <40; temp_count++){
      temp_adc_value += read_adc();
      delay_us(440);
   }
   temp_adc_value = temp_adc_value/40;
   return((uint16_t)temp_adc_value);
}

uint8_t keyBoardRead(void)
{
   return((input(PIN_B4))*(input(PIN_B4)+ 2*input(PIN_B5)+ 4*input(PIN_B6) + 8*input(PIN_B7)) + 16*(uint8_t)(!(int1)input(PIN_B0)));
}

uint8_t keyBoardRead_inside_int(void)
{
   return((input(PIN_B4))*(input(PIN_B4)+ 2*input(PIN_B5)+ 4*input(PIN_B6) + 8*input(PIN_B7)) + 16*(uint8_t)(!(int1)input(PIN_B0)));
}
