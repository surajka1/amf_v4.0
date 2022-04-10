void autoRun(void);
void manualRun(void);

void processIO(void)
{
   uint8_t temp_value_8;

   updateReadings();
   amfDisplay();
   ebIsConsistant(TASK_DEFAULT);
   processMainsFail(TASK_DEFAULT);
   processContactorDelay(TASK_DEFAULT);

   processBuildup(TASK_DEFAULT);
   processAutoCrank(TASK_DEFAULT);
   processAutoStop(TASK_DEFAULT);
   processRecool(TASK_DEFAULT);
   processLowFuelDelay(TASK_DEFAULT);
   processWarningtDelay(TASK_DEFAULT);
   processLLOPDelay(TASK_DEFAULT);
   processTrip(TASK_DEFAULT);
   processFaultDelay(TASK_DEFAULT);
   processHooterAuto();
   scheduler();
   if(flag.log_enable == 1){
      processLog();
   }

     

   /************************************   Enter settings menu  ***********************************/
   if((keyBoardRead() == KEY_MENU)) //&& (process_on.crank == 0) && (process_on.stop == 0))
   {

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }
      amfDelayLongMs(500);
      settings();
      amfDelayLongMs(100);
      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }

   }
   else{}

   /************************************  Fault Reset control  **************************************/

   if((keyBoardRead() == KEY_RESET))
   {

      downtimer.hooter = 0;
      status.hooter_done = 1;
      BUZZER_OFF();
//      if((process_on.crank == 0) && (process_on.stop == 0)){
         downtimer.kb_delay = RESET_DELAY;
         while((keyBoardRead() == KEY_RESET) && (downtimer.kb_delay > 0 )){
            amfWdogRestart();
         }
         if(downtimer.kb_delay == 0){
            amfFaultReset();
         }
         while(keyBoardRead() == KEY_RESET){
            amfWdogRestart();
         }
//      }

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }

   }
   else{}

   /***************************************  Auto/Manual mode control  ********************************/
   if((keyBoardRead() == KEY_MANUAL) && (process_on.crank == 0) && (process_on.stop == 0)){

      downtimer.kb_delay = 3;
      while((keyBoardRead() == KEY_MANUAL) && (downtimer.kb_delay > 0 )){
         amfWdogRestart();
      }
      if(downtimer.kb_delay == 0){
         LCD_Init();

         if(status.mode == AUTO_MODE){
            status.mode = MANUAL_MODE;
            MANUAL_LED_ON();

            printf(lcd_putc,  "\f Manual mode");
            printf(lcd_putc,  "\n   Selected");
         }
         else{   //if controller is currently running in manual mode
            if(status.remote_mode == 1){      //if controller is in remote mode
               printf(lcd_putc,  "\fController is in");
               printf(lcd_putc,  "\n Remote mode");
            }
            else{
               status.mode = AUTO_MODE;
               MANUAL_LED_OFF();
               printf(lcd_putc, "\f  Auto mode");
               printf(lcd_putc, "\n   Selected");
            }
         }
         Eprom_Set16(8, (uint16_t)status.mode);
         status.mode_rom = status.mode;
         status.semiauto = 0;
      }

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }

   }
   else {}

   /************************************   Manual Engine stop control  ********************************/

   if((keyBoardRead() == KEY_STOP) && (process_on.crank == 0) && (process_on.stop == 0)){

      temp_value_8 = 1;
      flag.displaystat = 0;
      status.llop_timeout = 0;
      DG_CONTACTOR_OFF();
      while((keyBoardRead() == KEY_STOP)){

         amfWdogRestart();

         if(status.mode == AUTO_MODE){
            fault.emstop = 1;
            fault.common = 1;
            FAULT_IND_ON();
         }
         else if (status.semiauto == 1){
            if(status.remote_start == 1){
               fault.emstop = 1;
               fault.common = 1;
               FAULT_IND_ON();
            }
            else{
               status.semiauto = 0;
            }
         }
         if(enable.fuelsol == 1){
            FUEL_SOL_OFF();
         }
         else{
            FUEL_SOL_ON();
         }
         sprintf(AMFString.lcd_data[0], "StoppingEngine..");
         sprintf(AMFString.lcd_data[1], "%d", temp_value_8);
         if(flag.displaystat == 1){
            flag.displaystat = 0;
            temp_value_8++;
            LCD_Init();
            LCD_WriteLineStr(1, AMFString.lcd_data[0]);
            LCD_WriteLineStr(2, AMFString.lcd_data[1]);
         }else{}

         status.semiauto = 0;

      }
      FUEL_SOL_OFF();
      status.engine_stopped = 1;
      status.engine_running = 0;

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }

   }
   else {}

   /************************************   Date and time view  ***********************************/
   if((keyBoardRead() == KEY_TIME) && (process_on.crank == 0) && (process_on.stop == 0))
   {
      while(keyBoardRead() == KEY_TIME){
         amfWdogRestart();
         temp_value_8 =  amftime.tm_sec;
         amfGetRTCTimeExternal(&amftime);
         if(temp_value_8 != amftime.tm_sec){
            //sprintf(AMFString.lcd_data[0], "Time D/M, H:M:S");
            sprintf(AMFString.lcd_data[0], "LowVolt = %d", flag.lowvcc);
            sprintf(AMFString.lcd_data[1], "%02d/%02d,%02d:%02d:%02d", amftime.tm_mday, amftime.tm_mon + 1 ,amftime.tm_hour, amftime.tm_min, amftime.tm_sec);
            LCD_Init();
            LCD_WriteLineStr(1, AMFString.lcd_data[0]);
            LCD_WriteLineStr(2, AMFString.lcd_data[1]);
         }
      }
      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }
   }
   else{}

   /******************************************************************************************************/
   /*************************************    Fault handling  *****************************************/
   /*         if low fuel fault is ignored in manual mode - need to be reprogrammed
    */
   if(fault.common == 1)   {

      DG_CONTACTOR_OFF();
   }
   else {}

   if((fault.common == 1) && (status.engine_stopped == 0)&&(fault.stop == 0) && (process_on.crank == 0))   {
      DG_CONTACTOR_OFF();
      if(fault.start || fault.dgovervolt || fault.dgundervolt || fault.lowfuel || fault.alternator
               || fault.underspeed || fault.overspeed || fault.overload || fault.ht || fault.llop )
      {
         if((status.recooled == 1) ){
            if(process_on.stop == 0){
               if(downcounter.stop_attempts == 0){
                  downcounter.stop_attempts = 3;
               }else {}
               processAutoStop(TASK_NEW);
            } else {}
         }else {}
      }
      else            // else if emergency stop
      {
         if(process_on.stop == 0){
            if(downcounter.stop_attempts == 0){
               downcounter.stop_attempts = 3;
               processAutoStop(TASK_NEW);
            }
            else {
               if(status.recooled == 1){
                  processAutoStop(TASK_NEW);
               }
            }
         }
      }
   }
   else {
            //add code here
   }

   /*****************************************************************************************************************/

   if(status.mode == AUTO_MODE){   //Auto mode
      autoRun();
   }
   else if(status.mode == MANUAL_MODE){
      manualRun();
   }
   else{   //if no mode -- write escape routes
   }
     
   return;
}

void autoRun(void)
{

   if(status.dgon == 1){
      status.engine_running = 1;
      status.engine_stopped = 0;
   }
   else{
      status.dghealthy = 0;
      status.dg_ok_to_load = 0;
   }


   if((STATUS_EB_CONTACT() == 1) && (STATUS_DG_CONTACT() == 1))   {
      if(status.ebhealthy == 1){
         DG_CONTACTOR_OFF();
      }
      else if(status.dghealthy == 1){
         EB_CONTACTOR_OFF();
      }
      else{
         DG_CONTACTOR_OFF();
      }
   }   else   {   }         //else nothing

   if((status.ebhealthy == 1) || ((STATUS_EB_CONTACT() == 1) && (status.ebfail == EBOK))){
      DG_CONTACTOR_OFF();
      if(status.m_cont_delay_ok ==1){
         EB_CONTACTOR_ON();
      }
   }
   else{
      EB_CONTACTOR_OFF();
   }

   /******************************* Engine Stop process in auto  mode ******************************/
   /////////////// stop engine if mains is healthy //////////////////////////////////////////////////

   if(((status.ebhealthy == 1)||(status.schedule_cycle != 1))
         && ((process_on.crank == 0) && (process_on.enginestartup == 0)))
   {
      if(((status.dgon == 1)||(status.engine_stopped == 0))&&(fault.stop == 0)){
         DG_CONTACTOR_OFF();
         if(status.recooled == 1){
            if(process_on.stop == 0){
               if(downcounter.stop_attempts == 0){
                  downcounter.stop_attempts = 3;
               }else {}
               processAutoStop(TASK_NEW);
            }
         }
      }
   }


   /*************************************** Engine auto start process ******************************/
     
   if((status.ebfail_timedout == 1)&&(fault.common == 0)&&(status.dgon == 0) && (STATUS_DG_CONTACT() == OFF) && (status.schedule_cycle == 1)){
      if((process_on.crank == 0) &&(process_on.enginestartup == 0) && (process_on.trip == 0) && (process_on.stop == 0)
            && (downcounter.stop_attempts == 0) && (status.engine_running == 0))
      {
         if(downcounter.start_attempts == 0){
            downcounter.start_attempts = para.max_crank_no;
         }else{}
            processAutoCrank(TASK_NEW);
      }
   }

   /************************************ Auto Mains OK mode  ***************************************/

   if(status.ebhealthy == 1)   {
      DG_CONTACTOR_OFF();
      if(status.m_cont_delay_ok == 1){
         EB_CONTACTOR_ON();
      }

   }

   /************************************ DG Auto Load on mode  ****************************************/

   if( (status.dg_ok_to_load == 1 ) && (status.ebhealthy == 0) && (fault.common == 0)
          && (status.schedule_cycle == 1) && ( downcounter.stop_attempts == 0) && (process_on.stop != 1))
   {
      downcounter.stop_attempts = 0; downcounter.start_attempts = 0;
      status.dg_resting = 0;
      status.recooled = 0; 

      EB_CONTACTOR_OFF();
      if(status.dg_cont_delay_ok == 1){
         DG_CONTACTOR_ON();
      }

      if(enable.fuelsol == ENABLED)   {
         FUEL_SOL_ON();
      }
      else {
         FUEL_SOL_OFF();
      }

   }    
   else {
      DG_CONTACTOR_OFF();
   }
     
        
   /************************************ Auto Scheduled Off  Mode  *************************************/
     
   if( (status.schedule_cycle != 1) && (status.dgon == 0) && (status.ebhealthy == 0)){
      status.auto_scheduled_off = 1;
   } else {
      status.auto_scheduled_off = 0;
   }

   /******************************************    The End   **********************************************/

   return;
}

void manualRun(void)
{
   uint8_t temp_counter;

   status.auto_scheduled_off = 0;

   if(status.dgon == 1){
      status.engine_running = 1;
      status.engine_stopped = 0;
   }
   else{
      status.dghealthy = 0;
      status.dg_ok_to_load = 0;
   }

   /********************************** Remote Mode Operation ************************************/

   if(status.remote_start == 1){

      //engine start mode
      if((process_on.crank == 0) &&(process_on.enginestartup == 0) && (process_on.trip == 0)
            && (process_on.stop == 0) && (downcounter.stop_attempts == 0)
            && (status.engine_running == 0) && (fault.common == 0)&&(status.dgon == 0))
      {
         if(downcounter.start_attempts == 0){
            downcounter.start_attempts = para.max_crank_no;
         }else{}
         processAutoCrank(TASK_NEW);
      }

      //contactor close mode
      if( (status.dg_ok_to_load == 1 )&& (fault.common == 0)
            && ( downcounter.stop_attempts == 0) && (process_on.stop != 1))
      {
         downcounter.stop_attempts = 0; downcounter.start_attempts = 0;
         status.dg_resting = 0;
         status.recooled = 0;

         EB_CONTACTOR_OFF();
         if(status.dg_cont_delay_ok == 1){
            DG_CONTACTOR_ON();
         }

         if(enable.fuelsol == ENABLED)   {
            FUEL_SOL_ON();
         }
         else {
            FUEL_SOL_OFF();
         }

      }
      else {
         DG_CONTACTOR_OFF();
      }

      if((status.ebhealthy == 1) && (status.m_cont_delay_ok == 1) && (fault.common == 1)){
         EB_CONTACTOR_ON();
      }
      else if(fault.common == 1){
         EB_CONTACTOR_OFF();
      }

   }
   else if(status.remote_mode == 1){

      //engine stop if remote start condition is removed
      if((process_on.crank == 0) && (process_on.enginestartup == 0) && (fault.stop == 0) && (status.remote_started == 1))
      {
         if(((status.dgon == 1)||(status.engine_stopped == 0))&&(fault.stop == 0)){
            DG_CONTACTOR_OFF();
            if(status.recooled == 1){
               if(process_on.stop == 0){
                  if(downcounter.stop_attempts == 0){
                     downcounter.stop_attempts = 3;
                  }else {}
                  processAutoStop(TASK_NEW);
               }
            }
         }
      }

      //back to load on dg.
      if(status.semiauto == 1){         //only if real remote operation, not manual mode by wiring
         if(STATUS_DG_CONTACT() == ON){
            DG_CONTACTOR_OFF();
         }
         //DG_CONTACTOR_OFF();
         if((status.ebhealthy == 1) && (status.m_cont_delay_ok == 1)){
            EB_CONTACTOR_ON();
         }
         else{
            EB_CONTACTOR_OFF();
         }
      }
   }

   /********************************** End of Remote Mode Operation ******************************/

   /**********************************   DG Contactor manual control  ****************************/

   if((keyBoardRead() == KEY_DGC) && (process_on.crank == 0) && (process_on.stop == 0)){

      amfWdogRestart();

      downtimer.kb_delay = 2;

      if(fault.common == 1){
         sprintf(AMFString.lcd_data[0], "Reset fault to");
         sprintf(AMFString.lcd_data[1], " close Contactor");
         LCD_Init();
         LCD_WriteLineStr(1, AMFString.lcd_data[0]);
         LCD_WriteLineStr(2, AMFString.lcd_data[1]);
         while(keyBoardRead() == KEY_DGC){
            amfWdogRestart();
         }
      }

      while((keyBoardRead() == KEY_DGC) && (downtimer.kb_delay > 0 )){
         amfWdogRestart();
      }
      if((downtimer.kb_delay == 0) && (fault.common == 0)){

         if(status.remote_start == 0){
            status.semiauto = 0;
         }

         if(STATUS_DG_CONTACT() == ON){
            DG_CONTACTOR_OFF();
         }
         else{
            if(STATUS_EB_CONTACT() == ON){
               EB_CONTACTOR_OFF();
               amfDelayLongMs(250);
               amfWdogRestart();
               amfDelayLongMs(250);
               amfWdogRestart();
            }
            EB_CONTACTOR_OFF();
            DG_CONTACTOR_ON();
         }
      }

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }

   }
   else {}


   /*********************************   Mains Contactor manual control  ****************************/

   if((keyBoardRead() == KEY_MC) && (process_on.crank == 0) && (process_on.stop == 0)){
      downtimer.kb_delay = 2;
      amfWdogRestart();
      while((keyBoardRead() == KEY_MC) && (downtimer.kb_delay > 0 )){
         amfWdogRestart();
      }
      if(downtimer.kb_delay == 0){

         if(status.remote_start == 0){
            status.semiauto = 0;
         }

         if(STATUS_EB_CONTACT() == ON){
            EB_CONTACTOR_OFF();
         }
         else{
            if(STATUS_DG_CONTACT() == ON){
               DG_CONTACTOR_OFF();
               amfDelayLongMs(250);
               amfWdogRestart();
               amfDelayLongMs(250);
               amfWdogRestart();
            }
            DG_CONTACTOR_OFF();
            EB_CONTACTOR_ON();
         }
      }

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }
   }
   else {}


   /*********************************   Manual Engine start control  ********************************/

   if((keyBoardRead() == KEY_START) && (process_on.crank == 0) && (process_on.stop == 0)){

      temp_counter = 1;
      flag.displaystat = 0;
      status.llop_timeout = 0;
      DG_CONTACTOR_OFF();

      if(fault.common == 1){
         sprintf(AMFString.lcd_data[0], "Reset fault");
         sprintf(AMFString.lcd_data[1], " before crank");
         LCD_Init();
         LCD_WriteLineStr(1, AMFString.lcd_data[0]);
         LCD_WriteLineStr(2, AMFString.lcd_data[1]);
         while(keyBoardRead() == KEY_START){
            amfWdogRestart();
         }
      }

      if((status.dgon == 1) && (reading.frequency > 25)){
         sprintf(AMFString.lcd_data[0], "Engine is ");
         sprintf(AMFString.lcd_data[1], " already running");
         LCD_Init();
         LCD_WriteLineStr(1, AMFString.lcd_data[0]);
         LCD_WriteLineStr(2, AMFString.lcd_data[1]);
         while(keyBoardRead() == KEY_START){
            amfWdogRestart();
         }
      }

      sprintf(AMFString.lcd_data[0], "CrankingEngine..");
      while((keyBoardRead() == KEY_START) && (fault.common == 0) && ((status.dgon == 0) && (reading.frequency < 25))){

         status.semiauto = 0;

         downtimer.crank = 2;   //to prevent crank turning off in interrupt
         CRANK_ON();
         if(enable.fuelsol == 1){
            FUEL_SOL_ON();
         }
         else{
            FUEL_SOL_OFF();
         }
         sprintf(AMFString.lcd_data[1], "%d", temp_counter);
         if(flag.displaystat == 1){
            flag.displaystat = 0;
            temp_counter++;
            LCD_Init();
            LCD_WriteLineStr(1, AMFString.lcd_data[0]);
            LCD_WriteLineStr(2, AMFString.lcd_data[1]);
         }else{}
         amfWdogRestart();
      }
      CRANK_OFF();
      downtimer.crank = 0;

      downtimer.enginestartup = prog_delay.enginestartup;
      process_on.enginestartup = 1;

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }

   }
   else {}


   /******************************************    The End   **********************************************/

   return;
}


