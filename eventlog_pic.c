
#define MAX_EVENT_SAMPLEGROUP 	30			//maximum no of events

#define FLTLOG_START   200      //start of Log Details in EEPROM
#define MAX_EVENTS 60   	//Maximum number of events to be displayed.
#define EVENT_NO_ADDRESS_EEPROM	18			//ie the position of the last event recorded
#define EVENT_STATUS_ADDRESS_START_EEPROM 20		//start of log_stat[]
#define EVENT_LOCATION_ADDRESS_START	70			//start of address

const char fault_mesg[30][15]=
{

   "NoEvent       ", 	//0
   "DGFailsToStart", 	//1      //in cdc
   "LowOilPressure", 	//2      //in cdc
   "Fault O1      ", 	//3      //in cdc
   "HighTemperatre", 	//4      //in cdc
   "Low Fuel Level", 	//5      //in cdc
   "AlternatrFault", 	//6      //in cdc
   "EmergencyStop ", 	//7      //in cdc
   "DG OverVoltage", 	//8      //in cdc
   "DG UnderSpeed ", 	//9      //in cdc
   "DG Over Speed ", 	//10      //in cdc
   "DG FailsToStop", 	//11      //in cdc
   "DGUnderVoltage", 	//12      //in cdc
   "CabinTempHigh ", 	//13      //in cdc
   "Mains Fail    ", 	//14      //in cdc
   "DG On         ", 	//15      //in cdc
   "BTSBatteryLow ", 	//16      //in cdc
   "DGBatteryLow  ",    //17      //in cdc
   "DGBatteryHigh ", 	//18      //in cdc
   "Manual Mode   ", 	//19       //in cdc
   "ErrorCode 01  ", 	//20      //in cdc
   "ErrorCode 02  ", 	//21 is empty
   "ScheduledOff  ", 		//22 is empty
   "Engine Rest   ",    	//23 is empty
   "ErrorCode 03  ", 		//24 is
   "ErrorCode W   ", 		//25 is wdog
   "              ", 		//26 is empty
   "              ", 		//27 is empty
   "              ", 		//28 is empty
   "              "		//29 is empty
};


static uint8_t log_stat[MAX_EVENT_SAMPLEGROUP]	=	{0};

static uint8_t event_stat[MAX_EVENT_SAMPLEGROUP]	=	{0};



void init_log_status()
{
	uint8_t tempcount=0;
	for(tempcount=0;tempcount<MAX_EVENT_SAMPLEGROUP;tempcount++)
	{
		log_stat[tempcount]=amfEEpromRead8(EVENT_STATUS_ADDRESS_START_EEPROM + tempcount);
		delay_ms(1);
	}   
  	
}

void log_fault(uint8_t event_code, int1 event_fault_stat, uint8_t log_number)
{
  	
	uint16_t event_rom_position=0;
  	
	event_rom_position = (uint16_t)(log_number)*10 + FLTLOG_START;
  	
	amfGetRTCTimeExternal(&amftime);
  	
	if(event_fault_stat)
	{

		amfEEpromWrite8(event_rom_position, event_code);
		//amfEEpromWrite8(event_rom_position+1, new_or_clear);
		amfEEpromWrite8(event_rom_position+1, amftime.tm_min);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+2, amftime.tm_hour);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+3, amftime.tm_mday);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+4, amftime.tm_mon);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+5, 10);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+6, 0);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+7, 0);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+8, 0);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+9, 0);
		delay_ms(1);	  	
	  	
	}
	else
	{
		if(amfEEpromRead8(event_rom_position)!=event_code)
			return;
		//amfEEpromWrite8(event_rom_position+6, new_or_clear);
		amfEEpromWrite8(event_rom_position+6, amftime.tm_min);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+7, amftime.tm_hour);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+8, amftime.tm_mday);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+9, amftime.tm_mon);
		delay_ms(1);
		amfEEpromWrite8(event_rom_position+5, 5);
		delay_ms(1);  	
	}
}

void event_record(uint8_t event_code, uint8_t event_fault_stat)
{

	  	
	uint8_t log_number = 0 , event_previous_position=0;
	uint16_t temp_value=0;
  	
	if(event_code>MAX_EVENT_SAMPLEGROUP)
		return;
	  	
	log_stat[event_code] = amfEEpromRead8((uint16_t)event_code + EVENT_STATUS_ADDRESS_START_EEPROM);
  	
	log_number=amfEEpromRead8(EVENT_NO_ADDRESS_EEPROM);
  	
	if(log_number>=(MAX_EVENTS))
		log_number=0; 	

	if(event_fault_stat)
	{
		event_previous_position = amfEEpromRead8(EVENT_LOCATION_ADDRESS_START + event_code);
		temp_value = (uint16_t)event_previous_position*10 + (uint16_t)FLTLOG_START + (uint16_t)5;
		if(amfEEpromRead8(temp_value)==10)
			return; 	
		log_number=log_number+1;
		amfEEpromWrite8(EVENT_LOCATION_ADDRESS_START+(uint16_t)event_code, log_number);
		amfEEpromWrite8(EVENT_NO_ADDRESS_EEPROM,log_number);	
		delay_ms(1);
	}
	else
	{
		log_number = amfEEpromRead8((uint16_t)event_code+EVENT_LOCATION_ADDRESS_START);  	
	}
  	
	log_fault(event_code,event_fault_stat, log_number);		//log number not previously calculated. it is below.
	amfEEpromWrite8((uint16_t)event_code +EVENT_STATUS_ADDRESS_START_EEPROM, event_fault_stat);
	delay_ms(1);
	log_stat[event_code]=event_fault_stat;
	  	
}

void disp_log_details(uint8_t position_of_event)
{
	uint8_t kb=0;
	uint16_t  base_add =  (uint16_t)position_of_event*10 + FLTLOG_START;
	uint8_t esr_hr=0, esr_min=0,esr_month =0, esr_day=0;
	uint8_t eso_hr=0, eso_min=0,eso_month =0, eso_day=0;
	char text[17];
  	
	esr_min = amfEEpromRead8(base_add+1);
	esr_hr =amfEEpromRead8(base_add+2);
	esr_day =amfEEpromRead8(base_add+3);
	esr_month =amfEEpromRead8(base_add+4) + 1;
  	
	eso_min = amfEEpromRead8(base_add+6);
	eso_hr =amfEEpromRead8(base_add+7);
	eso_day =amfEEpromRead8(base_add+8);
	eso_month =amfEEpromRead8(base_add+9) + 1;
  	

	if(amfEEpromRead8(base_add+5)!=5)
		sprintf(text, "Event not cleard");
	else
		sprintf(text, "%02u/%02u %02u:%02u",eso_day, eso_month, eso_hr, eso_min);
	lcd_init();
	printf(lcd_putc, "%02u/%02u %02u:%02u to", esr_day, esr_month, esr_hr, esr_min);
	LCD_WriteLineStr(2, text);
	while(TRUE)
	{
		kb=TimedKBInput();
		if((KEY_SET)||(KEY_EXIT)||(KEY_RESET))
			break;
	} 	
	  return; 
}

void processLog(void)
{
  	
	uint8_t temp_count=0;
	uint8_t stat1=0, stat2=0;
  	
	event_stat[0]	=	0;		//no event
	event_stat[1]	=	fault.start;
	event_stat[2]	=	fault.llop;
	event_stat[3]	=	fault.flt1;
	event_stat[4]	=	fault.ht;
	event_stat[5]	=	fault.lowfuel;
	event_stat[6]	=	fault.alternator;
	event_stat[7]	=	fault.emstop;
	event_stat[8]	=	fault.dgovervolt;
	event_stat[9]	=	fault.underspeed;
	event_stat[10]	=	fault.overspeed;
	event_stat[11]	=	fault.stop;
	event_stat[12]	=	fault.dgundervolt;
	event_stat[13]	=	fault.overload; // osc_fail;	//unused
	event_stat[14]	=	status.ebhealthy?0:1;
	event_stat[15]	=	status.dgon;
	event_stat[16]  =	warning.bts1low;
	event_stat[17]	=	warning.lowbat;
	event_stat[18]	=	warning.highbat;
	event_stat[19]	=	status.mode;
	event_stat[20]	=	fault.errcode01;
	event_stat[21]	=	fault.errcode02;
	event_stat[22]	=	(((status.scheduler_ok == 0) && (status.auto_scheduled_off == 1)) ? 1:0);
	event_stat[23]	=	(((status.cycleon_timeout == 1)) ? 1:0);
	event_stat[24]	=	fault.errcode03;
	event_stat[25]	=	fault.errcode04;	//wdog
	event_stat[26]	=	0;
	event_stat[27]	=	0;
	event_stat[28]	=	0;
	event_stat[29]	=	0;
  	
	for(temp_count=0;temp_count<MAX_EVENT_SAMPLEGROUP;temp_count++){
		delay_ms(1);
		stat1=event_stat[temp_count];
		stat2=log_stat[temp_count];
		if(stat1!=stat2){
			event_record(temp_count, event_stat[temp_count]);
			//log_stat[temp_count] = event_stat[temp_count]; //it is already updated in event_record()
		}
	}
	  	
	return;
}



uint8_t get_error_code(uint8_t log_number)
{
   return(amfEEpromRead8((uint16_t)log_number*10 + FLTLOG_START));
}

void logView()
{
   int1 choose_big=0;
	uint8_t kb=0;
	char text[17];
   uint16_t temp_value=0, log_max_no=0, disp_no=1, current_log_position=0, ivar=0;
   log_max_no= amfEEpromRead8(EVENT_NO_ADDRESS_EEPROM);
   temp_value = (log_max_no+1)*10 + FLTLOG_START+5;
   if((amfEEpromRead8(temp_value)==10)||(amfEEpromRead8(temp_value)==5)){
         current_log_position = amfEEpromRead8(EVENT_NO_ADDRESS_EEPROM);
         log_max_no = MAX_EVENTS;
         ivar =log_max_no-current_log_position;
         choose_big=1;
   }
   
   lcd_init();
   
start:  
   
	LCD_Init();
   printf(lcd_putc, "%lu.%s",disp_no, fault_mesg[get_error_code(log_max_no-ivar)]);
   lcd_gotoxy(1,1);
   lcd_send_byte(0,LCD_DISP_ON_BLINK);

   if(!choose_big){
      if((log_max_no-ivar)==1)
         sprintf(text, "%lu.%s",1, fault_mesg[get_error_code(log_max_no)]);
      else
         sprintf(text, "%lu.%s",disp_no+1, fault_mesg[get_error_code(log_max_no-ivar-1)]);
      LCD_WriteLineStr(2, text);
   }
   else{
      if((log_max_no-ivar)==(current_log_position+1))
         sprintf(text, "%lu.%s",1, fault_mesg[get_error_code(current_log_position)]);
      else if((log_max_no-ivar)==1)          
         sprintf(text, "%lu.%s",disp_no+1, fault_mesg[get_error_code(log_max_no)]);      
      else
         sprintf(text, "%lu.%s",disp_no+1, fault_mesg[get_error_code(log_max_no-ivar-1)]);
      LCD_WriteLineStr(2, text);
   }
   
   lcd_gotoxy(1,1);
   if(log_max_no>1) {
      while(TRUE){
           kb=TimedKBInput();
           if(kb == KEY_UP){    
              if(ivar>0)
                 ivar=ivar-1;
              else ivar=(log_max_no-1);
    
              if(disp_no>1)
                    disp_no=disp_no-1;
              else disp_no = log_max_no;
           }
           else if(kb == KEY_DOWN){
              if(ivar<(log_max_no-1))
                    ivar=ivar+1;
              else ivar=0;
              
              if(disp_no<(log_max_no))
                    disp_no=disp_no+1;
              else disp_no=1;
           }
           else if(kb == KEY_SET){
              //lcd_send_byte(0,LCD_DISP_ON);
              LCD_Init();
              amfDelayLongMs(500);
              disp_log_details(log_max_no-ivar);
              kb=0;
              amfDelayLongMs(500);
              goto start;
           }
           else 
              break;
   
   		LCD_Init();
         printf(lcd_putc, "%lu.%s",disp_no, fault_mesg[get_error_code(log_max_no-ivar)]);
         lcd_gotoxy(1,1);
         lcd_send_byte(0,LCD_DISP_ON_BLINK);

         if(!choose_big){
            if((log_max_no-ivar)==1)
               sprintf(text, "%lu.%s",1, fault_mesg[get_error_code(log_max_no)]);
            else
               sprintf(text, "%lu.%s",disp_no+1, fault_mesg[get_error_code(log_max_no-ivar-1)]);
            LCD_WriteLineStr(2, text);   
         }
         else{
            if((log_max_no-ivar)==(current_log_position+1))
               sprintf(text, "%lu.%s",1, fault_mesg[get_error_code(current_log_position)]);
            else if((log_max_no-ivar)==1)          
               sprintf(text, "%lu.%s",disp_no+1, fault_mesg[get_error_code(log_max_no)]);
            else
               sprintf(text, "%lu.%s",disp_no+1, fault_mesg[get_error_code(log_max_no-ivar-1)]);            
            LCD_WriteLineStr(2, text);   
         }

         lcd_gotoxy(1,1);
         kb=0;    
         amfDelayLongMs(500);
      }
   }
   amfDelayLongMs(500);
 
}

void clearEventLog(void)
{
	uint16_t temp_count;
	amfEEpromWrite8(EVENT_NO_ADDRESS_EEPROM, 0);
  
  for(temp_count = 0; temp_count < MAX_EVENT_SAMPLEGROUP; temp_count++){
		amfEEpromWrite8(EVENT_STATUS_ADDRESS_START_EEPROM + temp_count, 0);
		amfEEpromWrite8(EVENT_LOCATION_ADDRESS_START + temp_count, 0);
		log_stat[temp_count] = 0;
  }
  
  for(temp_count = FLTLOG_START; temp_count < ( FLTLOG_START + MAX_EVENTS*10 + 11 ); temp_count++){
	  amfEEpromWrite8(temp_count, 0);
  }
}
