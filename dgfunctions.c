uint8_t processRecool(uint8_t task_type);

void crankTimerClear(void){
   set_timer0(0);
   clear_interrupt(INT_TIMER0);
   return;
}

uint8_t scheduler(void)
{
   uint16_t time_current_16;

   time_current_16 = (uint16_t)amftime.tm_hour * 60  + (uint16_t)amftime.tm_min ;

   if(enable.cycletimer == 1){                  //if cycle timer is enabled
      if(status.cycleon_timeout == 0){         //if cycle timer is in run mode
         if(uptimer.dguptime > prog_delay.cycleon){
            status.cycleon_timeout = 1;
            status.cycleoff_timeout = 0;
         }
         else if(uptimer.dgdowntime > prog_delay.cycleoff){
            status.cycleoff_timeout = 1;
         }
      }
      else{                              //if cycle timer is in stop mode
         if(uptimer.dgdowntime > prog_delay.cycleoff){
            status.cycleon_timeout = 0;
            status.cycleoff_timeout = 1;
         }
         else{
            status.cycleoff_timeout = 0;
         }
      }
   }
   else{                  //if cycle timer is disabled
      status.cycleon_timeout = 0;
      status.cycleoff_timeout = 1;
   }

   if((enable.btsbypassrest == 1) && (enable.btsmonitoring == 1) && (warning.bts1low == 1) ) {
      status.cycleon_timeout = 0;
      status.cycleoff_timeout = 1;
   }


   if(enable.scheduler == 1){
      if((time_current_16 > timestamp.scheduled_start) && (time_current_16 < timestamp.scheduled_stop)){
         status.scheduler_ok = 1;
      }
      else{
         status.scheduler_ok = 0;
      }

   }
   else{
      status.scheduler_ok = 1;
   }

   if((status.cycleon_timeout == 0) && (status.scheduler_ok == 1)){
      status.schedule_cycle = 1;
      return(1);
   }
   else{
      status.schedule_cycle = 0;
      return(0);
   }
}

uint8_t processAutoCrank(uint8_t task_type)         //cranking only
{
   if(task_type == TASK_NEW){
      downtimer.crank = prog_delay.crank;
      process_on.crank = 1;
      crankTimerClear();
      flag.log_enable = 0;
   }
   else if(task_type == TASK_KILL){
      downtimer.crank = 0;
   }
   else{}   //task is default
     
   if((downtimer.crank > 0) && (fault.common == 0)){
      DG_CONTACTOR_OFF();

      if(enable.llopcheck == 1){
         if(status.llop == 1){
            if(enable.fuelsol == 1){
               FUEL_SOL_ON();
            }
            else{
               FUEL_SOL_OFF();
            }
            CRANK_ON();
         }
         else{
            CRANK_OFF();   //terminated if if LLOP status changes to that of engine running
   //         flag.log_enable = 1;
         }
      }
      else{
         if(enable.fuelsol == 1){
            FUEL_SOL_ON();
         }
         else{
            FUEL_SOL_OFF();
         }
         CRANK_ON();
         flag.log_enable = 0;
      }
      downtimer.enginestartup = prog_delay.enginestartup;
      process_on.enginestartup = 1;
      status.llop_timeout = 0;
      status.engine_stopped = 0;
      tempfault.dgundervolt = 0; tempfault.underspeed = 0;
      return(PROCESSING);
   }
   else if(process_on.crank == 1){
      downtimer.enginestartup = prog_delay.enginestartup;
      process_on.enginestartup = 1;
      status.llop_timeout = 0;
      status.engine_stopped = 0;
      tempfault.dgundervolt = 0; tempfault.underspeed = 0;
      process_on.crank = 0;
      CRANK_OFF();
   }
   else{
      CRANK_OFF();
      process_on.crank = 0;
      return(PROCESSOK);
   }
     
}

uint8_t processAutoStop(uint8_t task_type)
{
   if(task_type == TASK_NEW){
      if(downcounter.stop_attempts == 1)
         downtimer.stop = prog_delay.stop + 10;
      else if(downcounter.stop_attempts == 2)
         downtimer.stop = prog_delay.stop + 5;
      else
         downtimer.stop = prog_delay.stop;

      process_on.stop = 1;
      tempfault.dgovervolt = 0;
      tempfault.dgundervolt = 0;
      tempfault.overspeed = 0;
      tempfault.underspeed = 0;
      process_on.enginestartup = 0;
      downtimer.enginestartup = 0;
      flag.log_enable = 0;
      status.dg_ok_to_load == 0;
//      status.dghealthy = 0;
      crankTimerClear();
   }
   else if(task_type == TASK_KILL){
      downtimer.stop = 0;
   } else {}   //else if task is default

   if(fault.stop != 1){
      if(downtimer.stop > 0){
         process_on.stop = 1;
         status.llop_timeout = 0;
         DG_CONTACTOR_OFF();
         if(enable.fuelsol == 1){
            FUEL_SOL_OFF();
         }else {
            FUEL_SOL_ON();
         }
         flag.log_enable = 0;
         return(PROCESSING);
      }
      else if(process_on.stop == 1){
         DG_CONTACTOR_OFF();
         FUEL_SOL_OFF();
//         flag.log_enable = 1;   will be enabled in timer
         process_on.stop = 0;
         if(downcounter.stop_attempts > 0){
            downcounter.stop_attempts--;
         }else {}

         if(status.dgon == 1)
         {
            if(downcounter.stop_attempts > 0){
               status.recooled = 0;   processRecool(TASK_NEW);
            }
            else{
               fault.stop =1;
            }
            status.engine_stopped = 0;
            return(PROCESSFAIL);
         }
         else {
            fault.stop = 0;
            status.engine_stopped = 1;
            status.engine_running = 0;
            downcounter.stop_attempts = 0;
            process_on.stop = 0;
            return(PROCESSOK);
         }

      }
      else if (downcounter.stop_attempts > 0){
         if(status.recooled == 1){
            if(status.dgon == 1){
               if(downcounter.stop_attempts == 1)
                  downtimer.stop = prog_delay.stop + 10;
               else if(downcounter.stop_attempts == 2)
                  downtimer.stop = prog_delay.stop + 5;
               else
                  downtimer.stop = prog_delay.stop;
           
               process_on.stop = 1;
               tempfault.dgovervolt = 0;
               tempfault.dgundervolt = 0;
               tempfault.overspeed = 0;
               tempfault.underspeed = 0;
               process_on.enginestartup = 0;
               downtimer.enginestartup = 0;
               flag.log_enable = 0;
               status.dg_ok_to_load == 0;
               crankTimerClear();
            }
         }
         else if(status.dgon == 0){
            fault.stop = 0;
            status.engine_stopped = 1;
            status.engine_running = 0;
            downcounter.stop_attempts = 0;
            process_on.stop = 0;
            return(PROCESSOK);
         }
      }
      else{

      }
   }
   else{
//      FUEL_SOL_OFF();
      return(PROCESSFAIL);
   }
     

}

void processFaultDelay(uint8_t task_type)
{
   if(task_type == TASK_NEW){
      downtimer.fault_delay = FAULT_TOL_DELAY;
      process_on.fault_delay = 1;
   }
   else if(task_type == TASK_KILL){
      downtimer.fault_delay = 0;
   }


   if(downtimer.fault_delay > 0){
   //   DG_CONTACTOR_OFF();   //why turn off contactor
   }
   else if(process_on.fault_delay == 1){

      process_on.fault_delay = 0;
      if(tempfault.flt1 == 1)
         fault.flt1 = 1;
      if(tempfault.ht == 1)
         fault.ht = 1;
      if(tempfault.llop == 1)
         fault.llop = 1;
      if(tempfault.dgovervolt == 1)
         fault.dgovervolt = 1;
      if(tempfault.overspeed == 1)
         fault.overspeed = 1;

   }else {}

}

void processWarningtDelay(uint8_t task_type)
{
   if(task_type == TASK_NEW){
      downtimer.warning_delay = FAULT_TOL_DELAY;
      process_on.warning_delay = 1;
   }
   else if(task_type == TASK_KILL){
      downtimer.warning_delay = 0;
   }


   if(downtimer.warning_delay > 0){
   //   DG_CONTACTOR_OFF();   //why turn off contactor
   }
   else if(process_on.warning_delay == 1){
      process_on.warning_delay = 0;
      if(tempwarning.lowbat == 1)
         warning.lowbat = 1;
      if(tempwarning.highbat == 1)
         warning.highbat = 1;
      if(tempwarning.bts1low == 1)
         warning.bts1low = 1;
   }else {}

}

void processLowFuelDelay(uint8_t task_type){

   if((task_type == TASK_NEW) || ((tempfault.lowfuel == 1) && (fault.lowfuel == 0) && (process_on.lfl_delay == 0))){
      process_on.lfl_delay = 1;
      downtimer.lfl_delay = prog_delay.lfl_delay;
   }else{}

   if(downtimer.lfl_delay > 0){
      if(tempfault.lowfuel == 0){
         downtimer.lfl_delay = 0;
         process_on.lfl_delay = 0;
      }
   }
   else if(process_on.lfl_delay == 1){
      process_on.lfl_delay = 0;
      if(tempfault.lowfuel == 1){
         process_on.lfl_delay = 0;
         fault.lowfuel = 1;
      }
   }
   return;
}


void processTrip(uint8_t task_type){
   if((task_type == TASK_NEW ) && (process_on.enginestartup == 0)){
      process_on.trip = 1;
      downtimer.trip = prog_delay.trip;
   }

   if(downtimer.trip > 0){
      if((tempfault.dgundervolt == 0 ) && (tempfault.overload == 0) && (tempfault.underspeed == 0))
      {
         downtimer.trip = 0;
         process_on.trip = 0;
      }

   }
   else if(process_on.trip == 1){

      if(tempfault.dgundervolt == 1)
         fault.dgundervolt = 1;
      if(tempfault.overload == 1)
         fault.overload = 1;
      if(tempfault.underspeed == 1)
         fault.underspeed = 1;

      process_on.trip = 0;

   }
   return;
}

void processBuildup(uint8_t task_type)
{

   if( ((process_on.enginestartup == 0)&&(status.dgon == 1) && (status.dg_ok_to_load == 0)
         &&(downcounter.stop_attempts == 0) && (process_on.stop != 1) && (fault.common == 0))
         || (task_type == TASK_NEW) )
   {
      downtimer.enginestartup = prog_delay.enginestartup;
      process_on.enginestartup = 1;
//      downcounter.stop_attempts = 0;
   }

   if((downtimer.enginestartup > 0)&&(process_on.crank == 0)){
      process_on.enginestartup = 1;
      tempfault.underspeed = 0; tempfault.dgundervolt = 0;
      return;
   }

   if(process_on.stop == 1){
      process_on.enginestartup = 0;
      downtimer.enginestartup = 0;
      status.dg_ok_to_load = 0;
      downcounter.start_attempts = 0;
   }
   else if ((process_on.crank == 0) && (process_on.enginestartup == 1)){   //it wouldnt reach here if downtimer.enginestartup > 0
      process_on.enginestartup = 0;
      tempfault.underspeed = 0; tempfault.dgundervolt = 0;
      if(downcounter.start_attempts > 0){
         downcounter.start_attempts--;
      }else{}

      if((status.mode == MANUAL_MODE) && (status.semiauto == 0)){   //in this it was earlier if status.mode == AUTO_MODE, dont know why
         downcounter.start_attempts = 0;
      }else{}

      if(status.dghealthy == 0){
         if(downcounter.start_attempts == 0){
            if((status.mode == AUTO_MODE) || (status.semiauto == 1)){
               fault.start = 1;
               fault.common = 1;
            }else{
               status.engine_running = 0;
               status.dgon = 0;
               process_on.trip = 0;
               
            }   // no fails to start in manual mode

            status.dg_ok_to_load = 0;

            if((process_on.stop == 0)&&(status.mode == AUTO_MODE)){
               if(downcounter.stop_attempts == 0){
                  downcounter.stop_attempts = 3;
               }else {}
               processAutoStop(TASK_NEW);
            } else {}
         }else{
               status.engine_running = 0;
               status.dgon = 0;
               process_on.trip = 0;
         } 
         return;
      }
      else{
         fault.start = 0;
         if(tempfault.dgovervolt != 1){
            status.dg_ok_to_load = 1;
            downcounter.start_attempts = 0;
         }
         return;
      }
   }
   else{
      return;
   }
}

void processLLOPDelay(uint8_t task_type){

   if((status.engine_running == 0) && (status.dgon == 0)){
      status.llop_timeout = 0;
   }

   if(process_on.enginestartup == 1){
      process_on.llop_delay = 1;
      downtimer.llop_delay = LLOP_BYPASS_DELAY + downtimer.enginestartup;
   }

   if(downtimer.llop_delay > 0){
      status.llop_timeout = 0;
   }
   else if (process_on.llop_delay == 1){
      process_on.llop_delay = 0;
      status.llop_timeout = 1;
   }
}

uint8_t processRecool(uint8_t task_type)
{
   if((((status.dgon == 1)||(status.engine_running == 1) || (status.engine_stopped == 0))
         &&(STATUS_DG_CONTACT() == 0) && (status.recooled == 0) && (process_on.enginestartup == 0)
         &&(process_on.recool == 0))||(task_type == TASK_NEW))
   {
      downtimer.recool = prog_delay.recool;
      process_on.recool = 1;
   }
   else if(task_type == TASK_KILL){
      downtimer.recool = 0;
   }
   else if(STATUS_DG_CONTACT() == 1){
      status.recooled = 0;   process_on.recool = 0;
      downtimer.recool = prog_delay.recool;
   } else {}      //default process call

   if(downtimer.recool > 0){
      return(PROCESSING);
   }
   else if (process_on.recool == 1){
      process_on.recool = 0;
      status.recooled = 1;
      return(PROCESSOK);
   }

}

void processHooterAuto(void){
   if((fault.common == 1) && (process_on.hooter == 0)){
      if(status.hooter_done == 0){
         process_on.hooter = 1;
         downtimer.hooter = prog_delay.hooter;
      }else{}
   }
   else{

   }

   if(downtimer.hooter > 0){
      BUZZER_ON();
   }
   else {
      BUZZER_OFF();
      process_on.hooter = 0;
      if(fault.common == 1)
         status.hooter_done = 1;
   }

}

