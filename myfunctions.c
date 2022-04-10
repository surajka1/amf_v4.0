
#define amfEEpromRead8(x)   read_eeprom(x)

void displayContentUpdate(uint8_t x);
void amfGetRTCTimeExternal(struct tm *nTime);
void enableAmfInterrupts(void);
void disableAmfInterrupts(void);

const char month_abbr[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
              
//#separate                             
void amfDisplay(void)
{
   uint8_t temp8;
   temp8 = 4; //4 is the maximum default no of display screens
   static uint16_t var1, var2, var3, var4, var5;
   flag.scroll_lock_temp = 0;
     
   flag.max_display_items = 4;
   if(fault.dgovervolt == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 1;
   }

   if(fault.dgundervolt == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 2;
   }

   if(fault.emstop  == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 3;
   }

   if(fault.flt1 == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 5;
   }

   if(fault.ht == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 6;
   }

   if(fault.llop == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 7;
   }

   if(fault.lowfuel == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 8;
   }

   if(fault.overload == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 9;
   }

   if(fault.overspeed == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 10;
   }

   if(fault.start == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 11;
   }

   if(fault.stop == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 12;
   }

   if(fault.underspeed == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 13;
   }

   if(warning.bts1low == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 14;
   }

   if(warning.highbat == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 16;
   }

   if(warning.lowbat == 1){
      flag.max_display_items =  ++temp8;
      content.display[temp8] = 17;
   }

   if(warning.lowfuel == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 18;
   }

   if(fault.alternator == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 19;
   }

   if((process_on.mainsfail == 1) && (fault.common == 0) && (downtimer.mainsfail > 0)
         && (status.schedule_cycle == 1) && (status.mode == AUTO_MODE) && (status.dgon == 0)){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 20;
   }
   if(fault.errcode01 == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 21;
   }
   if(fault.errcode02 == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 22;
   }
   if(fault.errcode03 == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 23;
   }
   if(status.auto_scheduled_off == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 24;
   }
   if(fault.errcode04 == 1){   //wdog
      flag.max_display_items = ++temp8;
      content.display[temp8] = 25;
   }
   if(enable.btsmonitoring == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 26;
   }
   if(status.ebimbalance == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 27;
   }
   if(flag.lowvcc == 1){
      flag.max_display_items = ++temp8;
      content.display[temp8] = 28;
   }
     
//------------------ temperory   display items---------------------------------
   if(((status.mode == AUTO_MODE) || (status.semiauto == 1)) && (flag.scroll_lock == 0)){
     
      if(process_on.crank == 1){         //cranking
         if(flag.release_disp_lock[1] == 0){
            if( var1 != downtimer.crank){
               var1 = downtimer.crank;
               flag.displaystat = 1;
            }        
            flag.hold_disp_lock = 1;
            flag.max_display_items = ++temp8;
            content.display[temp8] = 29;
            flag.scroll_lock_temp = 1;
            flag.displaypos = flag.max_display_items;
         }
      }
      else{
         flag.release_disp_lock[1] = 0;
      }
        
      if(process_on.stop == 1){         //stopping
         if(flag.release_disp_lock[2] == 0){
            if( var2 != downtimer.stop){
               var2 = downtimer.stop;
               flag.displaystat = 1;
            }
            flag.hold_disp_lock = 2;
            flag.max_display_items = ++temp8;
            content.display[temp8] = 30;
            flag.scroll_lock_temp = 1;
            flag.displaypos = flag.max_display_items;
         }
      }  
      else{
         flag.release_disp_lock[2] = 0;
      }     
        
      if((process_on.enginestartup == 1) && (process_on.crank == 0)){         //startup
         if(flag.release_disp_lock[3] == 0){
            if( var3 != downtimer.enginestartup){
               var3 = downtimer.enginestartup;
               flag.displaystat = 1;
            }
            flag.hold_disp_lock = 3;
            flag.max_display_items = ++temp8;
            content.display[temp8] = 31;
            flag.scroll_lock_temp = 1;
            flag.displaypos = flag.max_display_items;
         }
      } 
      else{
         flag.release_disp_lock[3] = 0;
      }
        
      if((status.dgon == 0) && (fault.common == 0) //mains fail delay
         && (process_on.mainsfail == 1) && (downtimer.mainsfail < 30)){
         if(flag.release_disp_lock[4] == 0){
            if( var4 != downtimer.mainsfail){
               var4 = downtimer.mainsfail;
               flag.displaystat = 1;
            }
            flag.hold_disp_lock = 4;
            flag.max_display_items = ++temp8;
            content.display[temp8] = 20;
            flag.scroll_lock_temp = 1;
            flag.displaypos = flag.max_display_items;
         }
      } 
      else{
         flag.release_disp_lock[4] = 0;
      }
        
      if(process_on.trip == 1){         //startup
         if(flag.release_disp_lock[5] == 0){
            if( var5 != downtimer.trip){
               var5 = downtimer.trip;
               flag.displaystat = 1;
            }
            flag.hold_disp_lock = 5;
            flag.max_display_items = ++temp8;
            content.display[temp8] = 32;
            flag.scroll_lock_temp = 1;
            flag.displaypos = flag.max_display_items;
         }
      }  
      else{
         flag.release_disp_lock[5] = 0;
      }
   }
//-----------------------------------------------------------------------------     
   if(flag.displaystat ==1)
   {
      switch(flag.displaypos){
         case 0:
            sprintf(AMFString.lcd_data[0], "Mains Voltage");
            if(enable.ebsinglephase == 0)
               sprintf(AMFString.lcd_data[1], "R%03lu Y%03lu B%03lu", reading.ebr, reading.eby, reading.ebb);
            else
               sprintf(AMFString.lcd_data[1], "R %03lu V", reading.ebr);
            break;
         case 1:

            sprintf(AMFString.lcd_data[0], "DG Battery %.1gV", reading.dgbat);
            if((amftime.tm_hour > 23) || (amftime.tm_min > 59) || (amftime.tm_sec > 60) || (amftime.tm_mday > 31) 
                  || (amftime.tm_mday < 1) || (amftime.tm_mon > 11) || (amftime.tm_year > 150)){
               sprintf(AMFString.lcd_data[1], ""); //intially it was "time not set". But since it causes confusion, no display of time.
            }
            else{
            #ifndef HIDE_TIME
            sprintf(AMFString.lcd_data[1], "%02d:%02d:%02d %d %s",amftime.tm_hour, amftime.tm_min, amftime.tm_sec,
                  amftime.tm_mday, month_abbr[amftime.tm_mon]);
            #else
            sprintf(AMFString.lcd_data[1], "");
            #endif
            }
            break;           
         case 2:
            sprintf(AMFString.lcd_data[0], "Engine Voltage");
            if(enable.dgsinglephase == 0)
               sprintf(AMFString.lcd_data[1], "R%03lu Y%03lu B%03lu", reading.dgr, reading.dgy, reading.dgb);
            else
               sprintf(AMFString.lcd_data[1], "R %03lu V", reading.dgr);
            break;
         case 3:
            sprintf(AMFString.lcd_data[0], "DG Freq %.1gHz", reading.frequency);
            sprintf(AMFString.lcd_data[1], "Engine RPM %lu", (uint16_t)(reading.frequency*((float)para.rpmperhz)));
            break;           
         case 4: 
            sprintf(AMFString.lcd_data[0], "RunHour %lu:%02lu", uptimer.totalrunhr, uptimer.totalrunmin);
            if(status.dgon){
               sprintf(AMFString.lcd_data[1], "DGUpTime %lu:%02lu", uptimer.dguptime/3600, (uptimer.dguptime%3600)/60 );
            }else{
               sprintf(AMFString.lcd_data[1], "IdleTime %lu:%02lu", uptimer.dgdowntime/3600, (uptimer.dgdowntime%3600)/60 );
            }
            break;
         default:
            if(flag.displaypos >4){
               displayContentUpdate(flag.displaypos);
            }
            break;
      }
      LCD_Init();
      LCD_WriteLineStr(1, AMFString.lcd_data[0]);
      LCD_WriteLineStr(2, AMFString.lcd_data[1]);
   }
   flag.displaystat = 0;
}

//#separate
void displayContentUpdate(uint8_t x)
{

   switch(content.display[x]){
   case 1: //dg overvoltage
      sprintf(AMFString.lcd_data[0], "DG Over Voltage");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 2: //dg undervoltage
      sprintf(AMFString.lcd_data[0], "DG Under Voltage");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 3: //Emergency stop
      sprintf(AMFString.lcd_data[0], "Emergency Stop");
      sprintf(AMFString.lcd_data[1], " detected");
      break;
   case 5: //flt1
      sprintf(AMFString.lcd_data[0], "FLT01");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 6: //ht
      sprintf(AMFString.lcd_data[0], "High Temperature");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 7: //llop
      sprintf(AMFString.lcd_data[0], "Low Oil Pressure");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 8: //low fuel
      sprintf(AMFString.lcd_data[0], "Low Fuel");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 9: //dg overload
      sprintf(AMFString.lcd_data[0], "Engine Overload");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 10: //dg overspeed
      sprintf(AMFString.lcd_data[0], "DG Over Speed");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 11: //fail to start
      sprintf(AMFString.lcd_data[0], "Engine Failed ");
      sprintf(AMFString.lcd_data[1], " to Start");
      break;
   case 12: //fail to stop
      sprintf(AMFString.lcd_data[0], "Engine Failed");
      sprintf(AMFString.lcd_data[1], " to Stop");
      break;
   case 13: //dg underspeed
      sprintf(AMFString.lcd_data[0], "DG UnderSpeed");
      sprintf(AMFString.lcd_data[1], " fault occurred");
      break;
   case 14: //BTS battery low
      sprintf(AMFString.lcd_data[0], "Warning: BTS");
      sprintf(AMFString.lcd_data[1], " battery low");
      break;
   case 16: //dg battery high
      sprintf(AMFString.lcd_data[0], "Warning: DG");
      sprintf(AMFString.lcd_data[1], "Battery is High");
      break;
   case 17: //DG battery low
      sprintf(AMFString.lcd_data[0], "Warning: DG");
      sprintf(AMFString.lcd_data[1], "Battery is Low");
      break;
   case 18: //fuel low warning
      sprintf(AMFString.lcd_data[0], "Warning: ");
      sprintf(AMFString.lcd_data[1], "Low Fuel");
      break;
   case 19:
      sprintf(AMFString.lcd_data[0], "Alternator Fault");
      sprintf(AMFString.lcd_data[1], " Output fail");
      break;
   case 20:
      sprintf(AMFString.lcd_data[0], "MainsFail. Delay");
      sprintf(AMFString.lcd_data[1], "toStart %02lu:%02lu:%02lu", (downtimer.mainsfail)/3600, ((downtimer.mainsfail)%3600)/60, (downtimer.mainsfail)%60);
      break;
   case 21:
      sprintf(AMFString.lcd_data[0], "Error Code 01");
      sprintf(AMFString.lcd_data[1], "Flash Error %d", fault.rom_error_segment);
      break;
   case 22:
      sprintf(AMFString.lcd_data[0], "Error Code 02");
      sprintf(AMFString.lcd_data[1], "Settings Error");
      break;
   case 23:
      sprintf(AMFString.lcd_data[0], "Error Code 03");
      sprintf(AMFString.lcd_data[1], "RunHr Corrupted");
      break;
   case 24:   //scheduled off
      if(status.cycleon_timeout == 1){
         sprintf(AMFString.lcd_data[0], "Engine Resting");
         sprintf(AMFString.lcd_data[1], "%02d:%02d of %02d:%02d",  (uint8_t)(uptimer.dgdowntime/3600), 
            (uint8_t)((uptimer.dgdowntime%3600)/60) , (uint8_t)para.cycleoffhr, (uint8_t)para.cycleoffmin);
      }
      else if(status.scheduler_ok == 0){
         sprintf(AMFString.lcd_data[0], "Engine scheduled");
         sprintf(AMFString.lcd_data[1], "  to Turn Off");
      }
      break;
   case 25:
      sprintf(AMFString.lcd_data[0], "Error Code W");
      sprintf(AMFString.lcd_data[1], "Reset Error");
      break;
   case 26:   //BTS monitoring is enabled
      sprintf(AMFString.lcd_data[0], "BTS Bat %.1g", reading.btsbat1);
      if((tempwarning.bts1low == 1) || (warning.bts1low == 1)){
         sprintf(AMFString.lcd_data[1], "BTS Battery Low");
      }
      else{
      	if(reading.btsbat1 > BTS_V_THRESHOLD){
         	sprintf(AMFString.lcd_data[1], "BTS Battery OK");
      	}else{
      		sprintf(AMFString.lcd_data[1], "BTS not Connectd");
      	}
      }
      break;
   case 27:
      sprintf(AMFString.lcd_data[0], "Mains voltage");
      sprintf(AMFString.lcd_data[1], "  imbalance");
      break;
   case 28:
      sprintf(AMFString.lcd_data[0], "Low Operating");
      sprintf(AMFString.lcd_data[1], "Voltage:Warning");
      break;
   case 29:
      sprintf(AMFString.lcd_data[0], "Cranking Engine");
      sprintf(AMFString.lcd_data[1], "%2lu...", downtimer.crank);
      break;
   case 30:
      sprintf(AMFString.lcd_data[0], "Stopping Engine");
      sprintf(AMFString.lcd_data[1], "%2lu...", downtimer.stop);     
      break;
   case 31:
      sprintf(AMFString.lcd_data[0], "Engine Build-up");
      sprintf(AMFString.lcd_data[1], "Delay %2lu...", downtimer.enginestartup);
      break;
   case 32:
      sprintf(AMFString.lcd_data[0], "Tripping");
      sprintf(AMFString.lcd_data[1], "%2lu...", downtimer.trip);
      break;
   default:
      break;
   }
}

//#separate
void amfGetRTCTimeExternal(struct tm *nTime){

   uint8_t day, month, year, weekday, hour, min, sec;

   rtc_get_date(day, month, year, weekday);
   rtc_get_time(hour, min , sec);
     
   nTime->tm_mon = month - 1;   nTime   ->tm_wday = weekday -1;
   nTime->tm_mday = day; nTime->tm_year = (uint16_t)year + 100;
   nTime->tm_hour = hour; nTime->tm_min = min, nTime->tm_sec = sec;
     
}

#separate
void amfSetRTCTimeExternal(struct tm *nTime){

   rtc_set_datetime(nTime->tm_mday, nTime->tm_mon + 1, nTime->tm_year - 100, nTime->tm_wday + 1, nTime->tm_hour, nTime->tm_min);
   //rtc_init();  
   amfDelayMs(1);
}

void amfExtRTCInit(void){

   amfGetRTCTimeExternal(&amftime);
   if(amftime.tm_sec > 59){
      amftime.tm_sec = 0;
      rtc_init();      //from ds1338.c library.
      amfSetRTCTimeExternal(&amftime);
   }
     
}

#separate
void amfDelayLongMs(uint16_t msdelay){
   uint16_t temp_count;
   if(msdelay < 50){
      amfDelayMs(msdelay);
   }
   else{
      for(temp_count = 0; temp_count < msdelay; temp_count += 50)
      {
         amfWdogRestart();
         amfDelayMs(50);
      }
   }
   amfWdogRestart();
}

#separate
void amfFlashRead16(uint32_t StartAddress, uint16_t OffsetAddress, uint16_t* data){
   if((StartAddress + (uint32_t)OffsetAddress)%4 != 0)
      return;
   *data = read_program_eeprom(StartAddress + (uint32_t)OffsetAddress);
}

#separate
void amfFlashRead32(uint32_t StartAddress, uint16_t OffsetAddress, uint32_t* data){
     
   uint16_t temp_h, temp_l;
     
   if((StartAddress + (uint32_t)OffsetAddress)%4 != 0)
      return;
        
   temp_h = read_program_eeprom(StartAddress + (uint32_t)OffsetAddress + 2);
   temp_l = read_program_eeprom(StartAddress + (uint32_t)OffsetAddress);
   *data = make32(temp_h, temp_l);
     
}

#separate
void amfFlash_Set32(uint32_t rom_start_address, uint16_t offset, uint32_t data)
{
   uint16_t temp_h, temp_l;

   if(offset%4){
      return;
   }
   temp_l =(uint16_t)( data & 0x0000FFFF);
   temp_h = (uint16_t)(data >> 16);
     
  disableAmfInterrupts();
   if(flag.lowvcc == 0){
      write_program_eeprom(rom_start_address + offset + 2, temp_h);
      write_program_eeprom(rom_start_address + offset, temp_l);
   }
   enableAmfInterrupts();
   return;
}


void Eprom_Read16(uint16_t offset_address, uint16_t* data){

   *data = read_int16_eeprom(offset_address);
}

void Eprom_Set16(uint16_t address, uint16_t data){
  disableAmfInterrupts();
  if(flag.lowvcc == 0){
      write_int16_eeprom(address, data);
  }
  enableAmfInterrupts();
}

void disableAmfInterrupts(void){
  disable_interrupts(GLOBAL);
}

void enableAmfInterrupts(void){
  enable_interrupts(GLOBAL);
}


void amfEEpromWrite8(uint16_t address, uint8_t data)
{
  disableAmfInterrupts();
  if(flag.lowvcc == 0){
      write_eeprom(address, data);
  }
  enableAmfInterrupts();
}


