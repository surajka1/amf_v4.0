/*
 * eventlog.c
 *
 *  Created on: Jul 25, 2016
 *      Author: simcons
 */

#define MAX_EVENT_SAMPLEGROUP 	30			//maximum no of events
#define MAX_NO_OF_EVENTS		84			//46*2

#define EVENT_LOG_START_ADD_FLASH	0x1000
#define EVENT_SECTOR_SIZE_FLASH	512
#define EVENT_SIZE_PER_EVENT		12
#define EVENTS_PER_FLASH_SECTOR		42	//ie, EVENT_SECTOR_SIZE_FLASH/EVENT_SIZE_PER_EVENT

#define EVENT_NO_ADDRESS_EEPROM	18
#define EVENT_STATUS_ADDRESS_START_EEPROM 20
#define EVENT_LOCATION_ADDRESS_START	80

void displayLogDetails(uint16_t position_of_event);

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

static uint8_t erase_required_on_event_sector ;

void initLogStatus(void){

	uint8_t temp_count=0;
	uint16_t value16 = 0, event_last_position;

	for(temp_count = 0; temp_count < 30; temp_count++)
	{
		Eprom_Read16((uint16_t)temp_count*2 + EVENT_STATUS_ADDRESS_START_EEPROM , &value16);
		log_stat[temp_count] =  (uint8_t)value16; //(uint8_t)temp16;
		if(log_stat[temp_count] >1){
			log_stat[temp_count] = 0;
		}
	}

	Eprom_Read16(EVENT_NO_ADDRESS_EEPROM, &event_last_position);
}



void logEventWriteData(uint8_t event_code, uint8_t event_status, uint16_t offset_address){

	uint16_t event_rom_position=0;	//offset address of event position in flash
	uint32_t time_temp;		//time_t is long, ie int32_t

	uint16_t temp16;
	event_rom_position = offset_address;		//offset address, uint16_t

	time_temp = mktime(&amftime);

	disableAmfInterrupts();
	if(erase_required_on_event_sector == 1){

		//FLASH_EraseSector(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address);
		erase_required_on_event_sector = 0;
	}

	if(event_status == 1)	//even clear/not clear is not written due to space constraints.
	{						//will have to read the ROM data to check event clear time is written
//!		if((*(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address)) == 0xFFFFFFFF)
//!			FLASH_Program1LongWord(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address, (uint32_t)event_code);
//!		if((*(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address + 4)) == 0xFFFFFFFF)
//!			FLASH_Program1LongWord(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address + 4, time_temp);
	}
	else
	{
		amfFlashRead16(EVENT_LOG_START_ADD_FLASH, offset_address, &temp16);
		if(temp16 != (uint16_t)event_code){
			enableAmfInterrupts();
			return;
		}
		if((*(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address + 8)) == 0xFFFFFFFF)
			FLASH_Program1LongWord(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset_address + 8, time_temp);
	}
	enableAmfInterrupts();
}

uint16_t eventGetOffsetAddress(uint16_t position){
	uint16_t offsetaddress;
	if(position < 42){
		offsetaddress = (position)*12;
	}
	else if(position < 84){
		offsetaddress = (position)*12 + 8;
	}
	else if(position < 126){
		offsetaddress = (position)*12 + 16;
	}
	else{
		position = 0;
	}
	return (offsetaddress);
}

/*
 * uint8_t checkEventLocation(uint16_t position)
 *
 * returns 1 if location is ok, 0 if location is failure
 *  param - position - is the position of the event to be checked
 *
 */
uint8_t checkEventIsNotCleared(uint16_t position){

	uint16_t currentaddress;
	uint32_t data_temp32;
	uint8_t err_ok = TRUE;

	currentaddress = eventGetOffsetAddress(position);

	data_temp32 = *(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)currentaddress);
	if(data_temp32 > (MAX_EVENT_SAMPLEGROUP -1)){
		err_ok = FALSE;
	}
	data_temp32 = *(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)currentaddress + 8);
	if(data_temp32 != 0xFFFFFFFF){
		err_ok = FALSE;
	}

	return(err_ok);
}

void recordEvent(uint8_t event_code, uint8_t event_status){

	uint16_t event_current_position=0, event_last_position;
	uint16_t address_event_flash_write;


	if(event_code>(MAX_EVENT_SAMPLEGROUP-1))
		return;

	if(event_status == 1){		//write event in new location

		Eprom_Read16(EVENT_NO_ADDRESS_EEPROM, &event_last_position);
		if(event_last_position >= (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR - 1)){	//if flash is erased
			event_current_position = 0;
		}
		else {
			event_current_position = event_last_position + 1;
		}
		address_event_flash_write = eventGetOffsetAddress(event_current_position);
		if((address_event_flash_write%512) == 0){	//erase if entering a new flash sector.
			erase_required_on_event_sector = 1;
		}
		else{
			erase_required_on_event_sector = 0;
		}
		Eprom_Set16(EVENT_NO_ADDRESS_EEPROM, event_current_position);
		Eprom_Set16((uint16_t)event_code*2 + EVENT_LOCATION_ADDRESS_START, event_current_position);	//update the corresponding address map

	}
	else if(event_status == 0){	//event is cleared, only clearing time needs to be written, append to old location

		log_stat[event_code] = 0;
		Eprom_Read16(EVENT_LOCATION_ADDRESS_START + event_code*2,  &event_current_position);
		if(checkEventIsNotCleared(event_current_position) == FALSE){	//if flash is erased
			Eprom_Set16(EVENT_STATUS_ADDRESS_START_EEPROM + event_code*2, 0);
			return;	//ie return if previous event set record cannot be found
		} else {
			address_event_flash_write = eventGetOffsetAddress(event_current_position);
		}
	}
	else{
		return;
	}

	logEventWriteData(event_code, event_status, address_event_flash_write);		//log number not previously calculated. it is below.
	Eprom_Set16((uint16_t)event_code*2 + EVENT_STATUS_ADDRESS_START_EEPROM, (uint16_t)event_status); //update the corresponding log_stat[]
	log_stat[event_code]=event_status;

}


void processLog(void){

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

	for(temp_count = 0 ; temp_count < MAX_EVENT_SAMPLEGROUP; temp_count++)
	{
		stat1 = event_stat[temp_count];
		stat2 = log_stat[temp_count];
		if(stat2 > 1){	//if the sector is erased, consider it status doesn't need to be written
			stat2 = 0;
		}
		if(stat1!=stat2)
		{
			recordEvent(temp_count, event_stat[temp_count]);
		}
	}

}

void clearEventLog(void){
	uint8_t temp8 = 0;
	disableAmfInterrupts();
	FLASH_EraseSector(EVENT_LOG_START_ADD_FLASH);
	FLASH_EraseSector(EVENT_LOG_START_ADD_FLASH + 512);
	FLASH_EraseSector(EVENT_LOG_START_ADD_FLASH + 1024);
	for(temp8 = 0; temp8 < 61; temp8++){
		EEPROM_EraseSector(0x10000000 + 18 + (uint32_t)temp8*2);
	}
	for(temp8 = 0; temp8 < MAX_EVENT_SAMPLEGROUP; temp8++){
		log_stat[temp8] = 0;
	}
	erase_required_on_event_sector = 0;
	enableAmfInterrupts();
}


uint8_t getEventCode(uint16_t log_number){
	uint16_t temp16;
	uint16_t offset_address;
	offset_address = eventGetOffsetAddress(log_number);
	amfFlashRead16(EVENT_LOG_START_ADD_FLASH, offset_address, &temp16);
	if(temp16 > (MAX_EVENT_SAMPLEGROUP -1)){
		temp16 = 0;
	}
	return((uint8_t)temp16);
}

uint16_t getShiftedEventPosition(uint16_t totalrecords, uint16_t endposition, uint16_t currentposition , int8_t shift, uint16_t *event_number)
{
	uint16_t shifted_position;
	if(shift == KEY_DOWN){		//shift down, previous event
		if(endposition >= totalrecords){	//ie, just read 0 to endposition, there are no other records
			if(currentposition == (endposition - totalrecords)){
				shifted_position = endposition;
			}
			else if(currentposition <= endposition){	//ie expected range of currentposition
				shifted_position = currentposition - 1;
			}

			else{	//if current position is unexpted
				shifted_position = currentposition;
			}
			*event_number = endposition - shifted_position + 1;

		}
		else {//erased flash is in between endposition and (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1)
			if(currentposition == 0){
				shifted_position = (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1);
				*event_number = endposition + 2;
			}
			else if(currentposition <= endposition){
				shifted_position = currentposition -1;
				*event_number = endposition - shifted_position + 1;
			}
			else if(currentposition == (EVENTS_PER_FLASH_SECTOR + endposition)){	//added '+1' on 19.01.17
				shifted_position = endposition;
				*event_number = 1;
			}
			else if(currentposition <= (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1)){	//unexptected range
				shifted_position = currentposition - 1;
				*event_number = (endposition + (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR)  + 1) -shifted_position;
			}
			else{		//unexpected position
				shifted_position = currentposition;
				*event_number = (endposition + (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR)  + 1) -shifted_position;
			}
		}
	}
	else if(shift == KEY_UP)	{	//shift up, next position
		if(endposition >= totalrecords){//ie, just read 0 to endposition, there are no other records
			if(currentposition == endposition){
				shifted_position = endposition - totalrecords;
			}
			else if(currentposition < endposition){
				shifted_position = currentposition + 1;
			}
			else{
				shifted_position = currentposition;
			}
			*event_number = endposition - shifted_position + 1;
		}
		else	{//erased flash is in between endposition and (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1)
			if(currentposition == endposition){
				shifted_position = endposition + EVENTS_PER_FLASH_SECTOR;
				*event_number = totalrecords + 1;
			}
			else if(currentposition < endposition){
				shifted_position = currentposition + 1;
				*event_number = endposition- shifted_position + 1;
			}
			else if(currentposition == ((MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1))){
				shifted_position = 0;
				*event_number = endposition -shifted_position+ 1;
			}
			else if((currentposition >= (endposition + EVENTS_PER_FLASH_SECTOR) ) && (currentposition < (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1))){
				shifted_position = currentposition + 1;
				*event_number = (endposition + MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR + 1) - shifted_position;
			}
			else{
				shifted_position = currentposition;
				*event_number = (endposition + MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR + 1) - shifted_position;
			}
		}
	}
	else{		//no shift is passed, yet event number has to be updated.
		shifted_position = currentposition;
		if(endposition >= totalrecords){
			*event_number = endposition - shifted_position + 1;
		}
		else{
			if(shifted_position <= endposition){
				*event_number = endposition- shifted_position + 1;
			}
			else{
				*event_number = (endposition + MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR + 1) - shifted_position;
			}
		}

	}

	return(shifted_position);
}

void logView(void){

	uint8_t choose_big=0, no_record=0;
	uint8_t kb=0;
	uint16_t offset1 = 0, offset2 = 0;
	uint32_t temp_value=0;
	uint8_t shift = 0;
	uint16_t  log_max_no=0, disp_no=1, current_log_position=0, ivar=0;
	uint16_t log_end_position;
	uint16_t temp_position;

	Eprom_Read16(EVENT_NO_ADDRESS_EEPROM, &log_end_position);
	if(log_end_position > (MAX_NO_OF_EVENTS + EVENTS_PER_FLASH_SECTOR -1)){	//review this line for clarity of maximum value of log_max_no
		log_end_position = 0;
	}

	offset1 = eventGetOffsetAddress(log_end_position + 1);	//to see if event records below the current position
	if(offset1/512 == 0){
		offset2 = 512;
	}
	else if(offset1/512 == 1){
		offset2 == 1024;
	}
	else if(offset1/512 == 2){
		offset2 = 0;
	}
	temp_value = *(uint32_t *)(EVENT_LOG_START_ADD_FLASH + offset2);
	if((temp_value != 0xFFFFFFFF))	{	//if old event records can be found(other than position 0 to log_max_no)
		log_max_no = MAX_NO_OF_EVENTS - 1;
		current_log_position = log_end_position;
	}
	else{			// else = not more than log_max_no of events has ever been written, so dont need to check for events in the table below
		current_log_position = log_end_position;
		log_max_no = log_end_position;
	}

	kb=KEY_NONE;
	current_log_position = getShiftedEventPosition(log_max_no, log_end_position, current_log_position, kb, &ivar);
	sprintf(AMFString.lcd_data[0], "%lu.%s",(ivar) , fault_mesg[getEventCode(current_log_position)]);
	temp_position = getShiftedEventPosition(log_max_no,log_end_position, current_log_position, KEY_DOWN, &ivar);
	sprintf(AMFString.lcd_data[1], "%lu.%s",(ivar), fault_mesg[getEventCode(temp_position)]);

	LCD_Init();
	LCD_WriteLineStr(1, AMFString.lcd_data[0]);
	LCD_WriteLineStr(2, AMFString.lcd_data[1]);
	LCD_GotoXY(1,1);
	LCD_BlinkingOn();

	if(log_max_no>=0){
	  while(TRUE) {
			kb = 0;
			kb=TimedKBInput();
			if((kb==KEY_UP) || (kb == KEY_DOWN)){
			   current_log_position = getShiftedEventPosition(log_max_no, log_end_position, current_log_position, kb, &ivar);
			}
			else if(kb==KEY_SET){
				amfDelayLongMs(500);
				current_log_position = getShiftedEventPosition(log_max_no, log_end_position, current_log_position, kb, &ivar);
				displayLogDetails(current_log_position);
				amfDelayLongMs(500);
			}
			else
				break;

			sprintf(AMFString.lcd_data[0], "%lu.%s",(ivar), fault_mesg[getEventCode(current_log_position)]);
			temp_position = getShiftedEventPosition(log_max_no,log_end_position, current_log_position, KEY_DOWN, &ivar);
			sprintf(AMFString.lcd_data[1], "%lu.%s",(ivar), fault_mesg[getEventCode(temp_position)]);

			LCD_Init();
			LCD_WriteLineStr(1, AMFString.lcd_data[0]);
			LCD_WriteLineStr(2, AMFString.lcd_data[1]);
			LCD_GotoXY(1,1);
			LCD_BlinkingOn();
			amfDelayLongMs(500);
		}
	}
	amfDelayLongMs(1000);

}

void displayLogDetails(uint16_t position_of_event){
	uint8_t kb=0;
	uint16_t offset;
	struct tm log_time_start, log_time_end;
	uint32_t time0_32, time1_32;

	offset = eventGetOffsetAddress(position_of_event);

	time0_32 = *(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset + 4);
	time1_32 = *(uint32_t *)(EVENT_LOG_START_ADD_FLASH + (uint32_t)offset + 8);
//	localtime_r(&time0_32, &log_time_start);
//	localtime_r(&time1_32, &log_time_end);

	if(time0_32 == 0xFFFFFFFF)
		sprintf(AMFString.lcd_data[0], "No data");
	else
		sprintf(AMFString.lcd_data[0], "%02u/%02u %02u:%02u",log_time_start.tm_mday, log_time_start.tm_mon + 1, log_time_start.tm_hour, log_time_start.tm_min);

	if(time1_32 == 0xFFFFFFFF)
		sprintf(AMFString.lcd_data[1], "Event not cleard");
	else
		sprintf(AMFString.lcd_data[1], "%02u/%02u %02u:%02u",log_time_end.tm_mday, log_time_end.tm_mon + 1, log_time_end.tm_hour, log_time_end.tm_min);


	LCD_Init();
	LCD_WriteLineStr(1, AMFString.lcd_data[0]);
	LCD_WriteLineStr(2, AMFString.lcd_data[1]);

	while(TRUE)
	{
		kb=TimedKBInput();
		if((kb==KEY_SET)||(kb==KEY_EXIT)||(kb==KEY_RESET))
		break;
	}
	  return;
}
