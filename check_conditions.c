

void dcCheck(void);
uint8_t dgVoltCheck(void);
uint8_t mainsCheck(void);
uint8_t faultCheck(void);

void updateReadings(void){

//mains readings
   if(process_on.crank==1){
      return;
   }
     
   uint16_t temp16_1, temp16_2;
     
   if(ReadAveADC_10ms(CHANNEL_REMOTE) < 500){
      status.mode = MANUAL_MODE;
      status.remote_mode = 1;
        
//      set_adc_channel(1);   //since RemoteStart is mapped to Analog Channel AN1
      if(!input(PIN_REMOTE_START)){
         status.remote_start = 1;
         status.remote_started = 1;
         status.semiauto = 1;
      }
      else {
         status.remote_start = 0;
      }

   }
   else{
      status.mode = status.mode_rom;
      status.remote_mode = 0;
      status.remote_start = 0;
      status.remote_started = 0;
      status.semiauto = 0;
   }

   if(status.mode == AUTO_MODE){
      MANUAL_LED_OFF();
   }
   else if (status.semiauto == 0){
      MANUAL_LED_ON();
   }

//   temp16_1 = ReadAveADC_10ms(CHANNEL_BANDGAP);
   ratio.bandgapdgbat = 1;
   ratio.bandgapphase = 1;
   ratio.bandgapbts1 = 1;

   status.ebfail = mainsCheck();
     
   status.dgon = dgVoltCheck();
     
   if(status.dgon == 1){
      status.engine_stopped = 0;
      status.engine_running = 1;
   } else{}

   fault.external = faultCheck();
     
   fault.common = fault.external || fault.dgovervolt || fault.dgundervolt|| fault.underspeed || fault.alternator
                  || fault.emstop || fault.overload || fault.overspeed || fault.start || fault.stop;
                                         
   dcCheck();
     
   if((fault.common == 1) || (warning.common == 1))
   {
      FAULT_IND_ON();
   }
   else {
      FAULT_IND_OFF();
   }
   temp16_1 = uptimer.totalrunhr;
   temp16_2 = uptimer.totalrunmin;
   uptimer.totalrunhr = (uint16_t)(uptimer.totalrun/3600U);
   uptimer.totalrunmin = (uint16_t)((uptimer.totalrun%3600U)/60U);
   if(temp16_1 != uptimer.totalrunhr)
   {
      Eprom_Set16(4, uptimer.totalrunhr);   //engine runtime.hour
   }
   if(temp16_2 != uptimer.totalrunmin)
   {
        
      Eprom_Set16(6, uptimer.totalrunmin);   //engine runtime.minute
   }    
     
   Eprom_Read16(10, &temp16_1);   //dummy read to point the address away to an  
                                 //address that is not used.
   amfGetRTCTimeExternal(&amftime);

}


//**************************************************************************************
//!Function DG voltage Check.
//!DG Voltage Check for DG On or not.
//Updated Flags status.dgon, status.dghealthy
//! \return status.dgon
//**************************************************************************************

uint8_t dgVoltCheck(void)
{
   reading.frequency = 502012/(float)counter.freq;
   if(reading.frequency < 17.0){
      reading.frequency =0;
   }else {}

   reading.dgr   = (uint16_t)((float)ReadAveADC_10ms(CHANNEL_DGR)*ratio.dgr*ratio.bandgapphase);
   reading.dgy   = (uint16_t)((float)ReadAveADC_10ms(CHANNEL_DGY)*ratio.dgy*ratio.bandgapphase);
   reading.dgb   = (uint16_t)((float)ReadAveADC_10ms(CHANNEL_DGB)*ratio.dgb*ratio.bandgapphase);

   if(enable.dgsinglephase == 1){
      status.dgon = (reading.dgr>=DG_V_THRESHOLD);
   } else {
      status.dgon = (reading.dgr>=DG_V_THRESHOLD)||(reading.dgy>=DG_V_THRESHOLD)||(reading.dgb>=DG_V_THRESHOLD);
   }

   if(status.dgon == 1){
      status.engine_running = 1;
      status.engine_stopped = 0;
   }


   if(((process_on.crank == 0) && (process_on.stop == 0) && (downcounter.stop_attempts == 0)
         && ((status.dgon == 1) || ((status.mode == AUTO_MODE) && (status.engine_running == 1))))
         || ((status.mode == AUTO_MODE) && (STATUS_DG_CONTACT() == 1)))
   {
/** process_on.stop and process_on.crank included to not to detect fault incorrectly during these intervals
 ** process_on.recool included not to detect faults in between stop attempts -- remove and add status.stop attempts
 ** status.dgon has to be removed since it can be 0 if alternator fails or MCB trips
 */

      if((reading.frequency < para.lowfreq) && (fault.common == 0)){
         if(((status.mode == MANUAL_MODE) && (STATUS_DG_CONTACT() == 1)) || (status.mode == AUTO_MODE))
            if(process_on.enginestartup == 0)
               tempfault.underspeed = 1;
      }
      else {
         tempfault.underspeed = 0;
      }

      if((reading.frequency > para.highfreq) && (fault.common == 0) && (para.highfreq < 100)){
         if(((status.mode == MANUAL_MODE) && (STATUS_DG_CONTACT() == 1)) || (status.mode == AUTO_MODE))
            tempfault.overspeed = 1;
      }
      else{
         tempfault.overspeed = 0;
      }

      if(enable.dgsinglephase == 1){

         status.dghealthy = (reading.dgr >= para.dgundervolt)&&(reading.dgr <= para.dgovervolt)
               && (reading.frequency > para.lowfreq) && (reading.frequency < para.highfreq);

         if((reading.dgr >= para.dgovervolt) && (fault.common == 0) && (para.dgovervolt < 500))
         {
            if(((status.mode == MANUAL_MODE) && (STATUS_DG_CONTACT() == OFF)) != 1){
                  tempfault.dgovervolt = 1;
//                  status.dg_ok_to_load = 0;
            }

         }
         else{
               tempfault.dgovervolt = 0;
         }

         if((reading.dgr <= para.dgundervolt)&& (fault.common == 0))
         {
            if(((status.mode == MANUAL_MODE) && (STATUS_DG_CONTACT() == 1)) || (status.mode == AUTO_MODE))
               if(process_on.enginestartup == 0)
                  tempfault.dgundervolt = 1;
         }
         else{
            tempfault.dgundervolt = 0;
         }

      }
      else{
         if(((reading.dgr >= para.dgovervolt)||(reading.dgy >= para.dgovervolt)||(reading.dgb >= para.dgovervolt))
               && (fault.common == 0) && (para.dgovervolt < 500))
         {
            if(((status.mode == MANUAL_MODE) && (STATUS_DG_CONTACT() == OFF)) != 1){
                  tempfault.dgovervolt = 1;
//                  status.dg_ok_to_load = 0;
            }
         }
         else if(fault.common == 0){
            tempfault.dgovervolt = 0;
         }

         if(((reading.dgr <= para.dgundervolt) || (reading.dgy <= para.dgundervolt) || (reading.dgb <= para.dgundervolt)) && (fault.common == 0) ){
            if(((status.mode == MANUAL_MODE) && (STATUS_DG_CONTACT() == 1)) || (status.mode == AUTO_MODE))
               tempfault.dgundervolt = 1;
         }
         else if(fault.common == 0){
            tempfault.dgundervolt = 0;
         }
      }

      if((fault.dgundervolt == 0) && (fault.dgovervolt == 0) && (fault.underspeed == 0)
            && (fault.overspeed == 0) && (status.dgon == 1)
            && (fault.underspeed == 0) && (fault.overspeed == 0))
      {
         status.dghealthy = 1;
      }
      else{
         status.dghealthy = 0;
      }

      if((((fault.underspeed == 1)|| (tempfault.underspeed == 1))&&(fault.dgundervolt == 1) && (status.engine_running == 1))
            ||(((fault.dgundervolt == 1)|| (tempfault.dgundervolt == 1)) && (fault.underspeed == 1)&& (status.engine_running == 1))
            || (fault.alternator == 1))
      {
         fault.alternator = 1;
         fault.dgundervolt = 0;
         fault.underspeed = 0;
         tempfault.underspeed = 0;
         tempfault.dgundervolt = 0;
      }
   }
   else{               //if dgon = 0 or buildup = 1, ie , engine not running or starting up
//      status.dghealthy = 0; // how can you say dg is not healthy then, if not checked ?
   }

   if (((status.mode == AUTO_MODE) && (STATUS_DG_CONTACT() == ON)) == 1){
      if(((fault.dgundervolt == 0) && (tempfault.dgundervolt == 1))
            || ((fault.underspeed == 0) && (tempfault.underspeed == 1)))
      {
         if(process_on.trip == 0){
            processTrip(TASK_NEW);
         }
      }
   }
   if(((tempfault.overspeed == 1) && (fault.overspeed != 1))
         || ((tempfault.dgovervolt == 1) && (fault.dgovervolt != 1))){
      if(process_on.fault_delay == 0){
         processFaultDelay(TASK_NEW);
      }
   }
   return(status.dgon);

}



//**************************************************************************************
//!Function mainscheck.
//!Mains voltage ok or not
//Updated mains voltages, mainsfail
//! \return status.ebfail
//**************************************************************************************
uint8_t mainsCheck(void)
{
   uint16_t mains_avg16, imb_volt_max;
   int16_t mainsr, mainsy, mainsb;
   uint16_t diff1, diff2, diff3;


   reading.ebr   = (uint16_t)((float)ReadAveADC_10ms(CHANNEL_EBR)*ratio.ebr*ratio.bandgapphase);
   reading.eby   = (uint16_t)((float)ReadAveADC_10ms(CHANNEL_EBY)*ratio.eby*ratio.bandgapphase);
   reading.ebb   = (uint16_t)((float)ReadAveADC_10ms(CHANNEL_EBB)*ratio.ebb*ratio.bandgapphase);

   if(enable.ebsinglephase == 1){      //if single phase enabled
      if(status.ebfail){
         status.ebfail = (reading.ebr<=para.ebundervolt_h)||((reading.ebr>=para.ebovervolt) && (para.ebovervolt < 500));
      }
      else{
         status.ebfail = (reading.ebr<=para.ebundervolt_l)||((reading.ebr>=para.ebovervolt) && (para.ebovervolt < 500));
      }
   }
   else{                                    //if 3 phase enabled
      if(status.ebfail){
         status.ebrfail = (reading.ebr<=para.ebundervolt_h)||((reading.ebr>=para.ebovervolt) && (para.ebovervolt < 500));
         status.ebyfail = (reading.eby<=para.ebundervolt_h)||((reading.eby>=para.ebovervolt) && (para.ebovervolt < 500));
         status.ebbfail = (reading.ebb<=para.ebundervolt_h)||((reading.ebb>=para.ebovervolt) && (para.ebovervolt < 500));
      }
      else{
         status.ebrfail = (reading.ebr<=para.ebundervolt_l)||((reading.ebr>=para.ebovervolt) && (para.ebovervolt < 500));
         status.ebyfail = (reading.eby<=para.ebundervolt_l)||((reading.eby>=para.ebovervolt) && (para.ebovervolt < 500));
         status.ebbfail = (reading.ebb<=para.ebundervolt_l)||((reading.ebb>=para.ebovervolt) && (para.ebovervolt < 500));
      }
      status.ebfail = status.ebrfail || status.ebyfail || status.ebbfail;

      if(status.ebfail != 1){

         if(enable.ebimbalance == 1){

            mains_avg16 = (reading.ebr + reading.ebb + reading.eby)/3;

            if(status.ebimbalance == 1){
               imb_volt_max = ((mains_avg16 * para.imbalance)/100) - IMBALANCE_TOLERANCE;
            }
            else{
               imb_volt_max = ((mains_avg16 * para.imbalance)/100) + IMBALANCE_TOLERANCE;
            }

            mainsr = (int16_t)reading.ebr;   mainsy = (int16_t)reading.eby;   mainsb = (int16_t)reading.ebb;

            diff1 = (uint16_t)abs(mainsr - mainsy);
            diff2 = (uint16_t)abs(mainsb - mainsy);
            diff3 = (uint16_t)abs(mainsb - mainsr);

            if((diff1 > imb_volt_max) || (diff2 > imb_volt_max) || (diff3 > imb_volt_max)){
               status.ebfail = 1;
               status.ebimbalance = 1;
            }
            else{
               status.ebimbalance = 0;
            }
         }
         else{
            status.ebimbalance = 0;
         }
      }
      else{
         status.ebimbalance = 0;
      }
   }

   return(status.ebfail);

}


//****************************************************************************************
//!
//!                  Check external faults
//!Function faultCheck
//!Checks the external faults
//! \return external fault status
//!
//****************************************************************************************

uint8_t faultCheck(void)
{

   if(ReadAveADC_10ms(CHANNEL_LLOP) < 500){
      status.llop = 1;
      if((status.llop_timeout == 1) && (status.dgon == 1) && (process_on.stop == 0)
            && (process_on.crank == 0) && (process_on.recool == 0))
         tempfault.llop = 1 ;
   }
   else{
      tempfault.llop = 0;
      status.llop = 0;
   }


   if(ReadAveADC_10ms(CHANNEL_HT) < 500){
      if ((process_on.stop == 0) && (process_on.crank == 0) && (process_on.recool == 0))
         tempfault.ht = 1 ;
   }
   else{
      tempfault.ht = 0;
   }

   if(ReadAveADC_10ms(CHANNEL_FLT1) < 500){
      if ((process_on.stop == 0) && (process_on.crank == 0) && (process_on.recool == 0))
         tempfault.flt1 = 1 ;
   }
   else{
      tempfault.flt1 = 0;
   }

   if(((tempfault.ht == 1) && (fault.ht == 0)) || ((tempfault.llop == 1) && (fault.llop == 0))
         || ((tempfault.flt1 == 1) && (fault.flt1 == 0))){
      if(process_on.fault_delay == 0){
         processFaultDelay(TASK_NEW);
      }
   }


   if(ReadAveADC_10ms(CHANNEL_EMSTOP) < 500)
      fault.emstop = 1 ;

   if(ReadAveADC_10ms(CHANNEL_FUEL) < 500){
      tempfault.lowfuel = 1 ;
      if(process_on.lfl_delay == 0){
         processLowFuelDelay(TASK_NEW);
      }
   }
   else{
      tempfault.lowfuel = 0;
   }

   fault.external = fault.llop || fault.ht || fault.emstop || fault.lowfuel || fault.flt1;

   return(fault.external);

}

//****************************************************************************************
//!
//!                  Check DC Voltages
//!Function dccheck
//!Checks the external dc inputs
//! \return nothing
//!
//****************************************************************************************


void dcCheck(void)
{
   float trigger_val;

   reading.dgbat = (float)ReadAveADC_10ms(CHANNEL_DGBAT)*ratio.dgbat*ratio.bandgapdgbat;
   reading.btsbat1 = (float)ReadAveADC_10ms(CHANNEL_BTS)*ratio.btsbat1*ratio.bandgapbts1;



   if(warning.lowbat == 1){
      trigger_val = para.dgbatlow + .25;
   }
   else{
      trigger_val = para.dgbatlow;
   }
   if((reading.dgbat<trigger_val) && (flag.log_enable == 1)){
      if((warning.lowbat == 0) && ((process_on.crank ==1) || (process_on.stop == 1))){
         tempwarning.lowbat = 0;
      }else {
         tempwarning.lowbat = 1;
      }
   }
   else if(flag.log_enable == 1){      //to disable battery check during crank and stopping time.
      tempwarning.lowbat = 0;
      warning.lowbat = 0;
   }


   if(warning.highbat == 1){
      trigger_val = para.dgbathigh - 0.25;
   }
   else{
      trigger_val = para.dgbathigh;
   }
   if((reading.dgbat>trigger_val) && (flag.log_enable == 1)){
      tempwarning.highbat = 1;
   }else if(flag.log_enable == 1){
      tempwarning.highbat = 0;
      warning.highbat = 0;
   }

   if(warning.bts1low == 1){
      trigger_val = para.bts1low + .5;
   }
   else{
      trigger_val = para.bts1low;
   }
   if(enable.btsmonitoring == 1){
      if((reading.btsbat1<trigger_val) && (reading.btsbat1 > BTS_V_THRESHOLD)){
         tempwarning.bts1low = 1;
      }
      else{
         tempwarning.bts1low = 0;
         warning.bts1low = 0;
      }
   }
   else{
      warning.bts1low = 0;
      tempwarning.bts1low = 0;
   }

   if(((tempwarning.highbat == 1)&& (warning.highbat == 0)) || ((tempwarning.lowbat == 1) && (warning.lowbat == 0))
         ||((tempwarning.bts1low == 1) && (warning.bts1low == 0)))
   {
      if(process_on.warning_delay == 0){
         processWarningtDelay(TASK_NEW);
      }
   }

   warning.common = warning.lowbat || warning.highbat;
   return;

}




