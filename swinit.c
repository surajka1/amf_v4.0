void amfFaultReset(void);
void resetVariables(void);
void amfRomWriteDefault(void);
void decalibrateWrite(void);
void amfRomInit(void);
void amfRomWriteDefault(void);
void errorCheckRomData(void);


void peripheralInit(void)
{
   resetVariables();
   BUZZER_OFF();
   DG_CONTACTOR_OFF();
   EB_CONTACTOR_OFF();
   CRANK_OFF();
   FUEL_SOL_OFF();
   MANUAL_LED_OFF();
   FAULT_IND_OFF();
   BOARD_IND_ON();
     
   #ifdef amfcd4051
      #ifdef pic_4620
         setup_adc_ports(AN0_TO_AN1);
      #endif
      #ifdef pic_k22
         setup_adc_ports(sAN0 | sAN1);
      #endif
   #else
      #ifdef pic_4620
         setup_adc_ports(AN0);
      #endif
      #ifdef pic_k22
         setup_adc_ports(sAN0);
      #endif
   #endif
   
   setup_adc(ADC_CLOCK_INTERNAL|ADC_TAD_MUL_0);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_64);      //1.0 s overflow   
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);      //131 ms overflow
   //setup_timer_1(0b10110001);
   setup_timer_2(T2_DIV_BY_16,255,16);      //1.0 ms overflow, 16.3 ms interrupt
   setup_timer_3(T3_INTERNAL | T3_DIV_BY_8);      //131ms overflow
   setup_ccp2(CCP_CAPTURE_FE);
   setup_wdt(WDT_ON | WDT_4S);
     
   #ifdef pic_4620
      setup_low_volt_detect(LVD_TRIGGER_BELOW| LVD_42);
   #endif
   #ifdef pic_k22
      setup_low_volt_detect(LVD_TRIGGER_BELOW| LVD_41);
   #endif
     
   enable_interrupts(INT_CCP2);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_TIMER2);
   enable_interrupts(INT_TIMER3);
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   #ifdef pic_4620
      enable_interrupts(INT_LOWVOLT);
   #endif
   #ifdef pic_k22
      enable_interrupts(INT_HLVD);
   #endif
   
   port_b_pullups(TRUE);
   
   #ifdef amfcd4051
   //set_tris_a(0x03);
   #else
   set_tris_a(0x01);
   #endif
}

void amf_sw_init(void)
{
   uint16_t temp16;

   amfFaultReset();
  
   init_log_status();
     
   Eprom_Read16(0, &temp16);
   if(temp16 != 0xABCD){
      Eprom_Set16(0, 0xABCD);
      warning.common = 1;
      fault.errcode03 = 1;
   }
   Eprom_Read16(4, &temp16);
   if(temp16 > 9999){
      Eprom_Set16(4, 0);   //engine runtime.hour
      warning.common = 1;
      fault.errcode03 = 1;
   }
   Eprom_Read16(6, &temp16);
   if(temp16 > 59){
      Eprom_Set16(6, 0);   //engine runtime.minute
      warning.common = 1;
      fault.errcode03 = 1;
   }
   Eprom_Read16(8, &temp16);
   if(temp16 >1 ){ //ie, if neither auto nor manual mode
      Eprom_Set16(8, MANUAL_MODE);
      warning.common = 1;
      fault.errcode03 = 1;
   }     
     
   amfFlashRead16(FLASH_EE_START, 0,  &temp16);
   if(temp16 != 0xAAAA){
      amfRomWriteDefault();
      decalibrateWrite();
      warning.common = 1;
      fault.errcode02 = 1;
   }     
   amfWdogRestart();  
   amfRomInit();
   amfWdogRestart();
   errorCheckRomData();
   amfWdogRestart();
   amfRomInit();
   amfWdogRestart();
   amfExtRTCInit();
   

   sprintf(AMFString.lcd_data[0], "  Simcons GC12");
   #ifdef amfv34cd4051
   sprintf(AMFString.lcd_data[1], "Controller 3.40");
   #elif defined (amfv40cd4051)
   sprintf(AMFString.lcd_data[1], "Controller 4.00");  
   #else
   sprintf(AMFString.lcd_data[1], "Controller 3.30");
   #endif
   LCD_Init();
   LCD_WriteLineStr(1, AMFString.lcd_data[0]);
   LCD_WriteLineStr(2, AMFString.lcd_data[1]);        
   flag.displaystat = 0;
   flag.counter4 = 0;
        
   if(restart_cause() == WDT_TIMEOUT){
      fault.errcode04 = 1;
   }
     
   updateReadings();
   amfWdogRestart();
   updateReadings();   //two times to compensate powerup time of analog inputs.
   if(status.ebfail == EBOK){
      status.ebhealthy = 1;
   }
}


void amfRomInit(void){
   uint16_t temp16;

//   disableAmfInterrupts();
   Eprom_Read16(4, &uptimer.totalrunhr );
   if(uptimer.totalrunhr > 9999){
      uptimer.totalrunhr = 0;
   }
   Eprom_Read16(6, &uptimer.totalrunmin);
      if(uptimer.totalrunmin > 59){
      uptimer.totalrunmin = 0;
   }
   uptimer.totalrun = 3600*(uint32_t)uptimer.totalrunhr + 60*(uint32_t)uptimer.totalrunmin;

   Eprom_Read16(8, &temp16);
   (temp16 == 0) ? (status.mode = 0):(status.mode = 1);
   status.mode_rom = status.mode;

//   enable.cycletimer = *((uint16_t *)(FLASH_EE_START + 4U));
   amfFlashRead16(FLASH_EE_START, 4,&enable.cycletimer);
//   para.cycleonhr = *((uint16_t *)(FLASH_EE_START + 8U));
   amfFlashRead16(FLASH_EE_START, 8, &para.cycleonhr);
//   para.cycleonmin = *((uint16_t *)(FLASH_EE_START + 12U));
   amfFlashRead16(FLASH_EE_START, 12, &para.cycleonmin);
//   para.cycleoffhr = *((uint16_t *)(FLASH_EE_START + 16U));
   amfFlashRead16(FLASH_EE_START, 16, &para.cycleoffhr);
//   para.cycleoffmin = *((uint16_t *)(FLASH_EE_START + 20U));
   amfFlashRead16(FLASH_EE_START, 20, &para.cycleoffmin);
   prog_delay.cycleoff = (uint32_t)para.cycleoffhr * 3600 + (uint32_t)para.cycleoffmin * 60 ;
   prog_delay.cycleon = (uint32_t)para.cycleonhr * 3600 + (uint32_t)para.cycleonmin * 60 ;

//   enable.ebsinglephase = *((uint16_t *)(FLASH_EE_START + 24U));
   amfFlashRead16(FLASH_EE_START, 24, &enable.ebsinglephase);
//   para.ebundervolt_h = *((uint16_t *)(FLASH_EE_START + 28U));
   amfFlashRead16(FLASH_EE_START, 28, &para.ebundervolt_h);
//   para.ebundervolt_l = *((uint16_t *)(FLASH_EE_START + 32U));
   amfFlashRead16(FLASH_EE_START, 32, &para.ebundervolt_l);
//   para.ebovervolt = *((uint16_t *)(FLASH_EE_START + 36U));
   amfFlashRead16(FLASH_EE_START, 36, &para.ebovervolt);
//   enable.ebimbalance = *((uint16_t *)(FLASH_EE_START + 40U));
   amfFlashRead16(FLASH_EE_START, 40, &enable.ebimbalance);
//   para.imbalance = *((uint16_t *)(FLASH_EE_START + 44U));
   amfFlashRead16(FLASH_EE_START, 44, &para.imbalance);

//   enable.dgsinglephase = *((uint16_t *)(FLASH_EE_START + 48U));
   amfFlashRead16(FLASH_EE_START, 48, &enable.dgsinglephase);
//   para.dgundervolt = *((uint16_t *)(FLASH_EE_START + 52U));
   amfFlashRead16(FLASH_EE_START, 52, &para.dgundervolt);
//   para.dgovervolt = *((uint16_t *)(FLASH_EE_START + 56U));
   amfFlashRead16(FLASH_EE_START, 56, &para.dgovervolt);
//   temp16 = *((uint16_t *)(FLASH_EE_START + 60U));
   amfFlashRead16(FLASH_EE_START, 60, &temp16);
   para.lowfreq = (float)temp16;
//   temp16 = *((uint16_t *)(FLASH_EE_START + 64U));
   amfFlashRead16(FLASH_EE_START, 64, &temp16);
   para.highfreq = (float)temp16;

//   enable.llopcheck = *((uint16_t *)(FLASH_EE_START + 68U));
   amfFlashRead16(FLASH_EE_START, 68, &enable.llopcheck);
//   para.mfdelay_low = *((uint16_t *)(FLASH_EE_START + 72U));
   amfFlashRead16(FLASH_EE_START, 72, &para.mfdelay_low);
//   para.mfdelay_hr = *((uint16_t *)(FLASH_EE_START + 76U));
   amfFlashRead16(FLASH_EE_START, 76, &para.mfdelay_hr);
//   para.mfdelay_min = *((uint16_t *)(FLASH_EE_START + 80U));
   amfFlashRead16(FLASH_EE_START, 80, &para.mfdelay_min);
   prog_delay.mainsfail = ((uint32_t)para.mfdelay_hr)*3600 + ((uint32_t)para.mfdelay_min)*60 + (uint32_t)para.mfdelay_low;

//   prog_delay.crank = *((uint16_t *)(FLASH_EE_START + 84U));
   amfFlashRead16(FLASH_EE_START, 84, &prog_delay.crank);
//   para.max_crank_no = *((uint16_t *)(FLASH_EE_START + 88U));
   amfFlashRead16(FLASH_EE_START, 88, &para.max_crank_no);
//   prog_delay.enginestartup = *((uint16_t *)(FLASH_EE_START + 92U));
   amfFlashRead16(FLASH_EE_START, 92, &prog_delay.enginestartup);
//   prog_delay.ebrestore = *((uint16_t *)(FLASH_EE_START + 96U));
   amfFlashRead16(FLASH_EE_START, 96, &prog_delay.ebrestore);
//   prog_delay.trip = *((uint16_t *)(FLASH_EE_START + 100U));
   amfFlashRead16(FLASH_EE_START, 100, &prog_delay.trip);
//   prog_delay.recool = *((uint16_t *)(FLASH_EE_START + 104U));
   amfFlashRead16(FLASH_EE_START, 104, &prog_delay.recool);

//   enable.fuelsol = *((uint16_t *)(FLASH_EE_START + 108U));
   amfFlashRead16(FLASH_EE_START, 108, &enable.fuelsol);
//   prog_delay.stop = *((uint16_t *)(FLASH_EE_START + 112U));
   amfFlashRead16(FLASH_EE_START, 112, &prog_delay.stop);
//   prog_delay.hooter = *((uint16_t *)(FLASH_EE_START + 116U));
   amfFlashRead16(FLASH_EE_START, 116, &prog_delay.hooter);

//   temp16 = *((uint16_t *)(FLASH_EE_START + 120U));
   amfFlashRead16(FLASH_EE_START, 120, &temp16);
   para.dgbatlow = (float)temp16/10.0;
//   temp16 = *((uint16_t *)(FLASH_EE_START + 124U));
   amfFlashRead16(FLASH_EE_START, 124, &temp16);
   para.dgbathigh  = (float)temp16/10.0;

//   para.rpmperhz = *((uint16_t *)(FLASH_EE_START + 128U));
   amfFlashRead16(FLASH_EE_START, 128, &para.rpmperhz);

//   enable.scheduler = *((uint16_t *)(FLASH_EE_START + 132U));
   amfFlashRead16(FLASH_EE_START, 132, &enable.scheduler);
//   para.schedule_starthr = *((uint16_t *)(FLASH_EE_START + 136U));
   amfFlashRead16(FLASH_EE_START, 136, &para.schedule_starthr);
//   para.schedule_startmin = *((uint16_t *)(FLASH_EE_START + 140U));
   amfFlashRead16(FLASH_EE_START, 140, &para.schedule_startmin);
//   para.schedule_stophr =  *((uint16_t *)(FLASH_EE_START + 144U));
   amfFlashRead16(FLASH_EE_START, 144, &para.schedule_stophr);
//   para.schedule_stopmin = *((uint16_t *)(FLASH_EE_START + 148U));
   amfFlashRead16(FLASH_EE_START, 148, &para.schedule_stopmin);
   timestamp.scheduled_start = (uint16_t)para.schedule_starthr * 60 + (uint16_t)para.schedule_startmin ;
   timestamp.scheduled_stop = (uint16_t)para.schedule_stophr * 60 + (uint16_t)para.schedule_stopmin;

//   enable.btsmonitoring = *((uint16_t *)(FLASH_EE_START + 156));
   amfFlashRead16(FLASH_EE_START, 156, &enable.btsmonitoring);
//   temp16 = *((uint16_t *)(FLASH_EE_START + 160U));
   amfFlashRead16(FLASH_EE_START, 160, &temp16);
   para.bts1low  = (float)temp16/10.0;
//   enable.btsbypassrest = *((uint16_t *)(FLASH_EE_START + 164U));
   amfFlashRead16(FLASH_EE_START, 164, &enable.btsbypassrest);

//   prog_delay.lfl_delay = *((uint16_t *)(FLASH_EE_START + 168U));
   amfFlashRead16(FLASH_EE_START, 168, &prog_delay.lfl_delay);
//   prog_delay.cont_delay = *((uint16_t *)(FLASH_EE_START + 172U));
   amfFlashRead16(FLASH_EE_START, 172, &prog_delay.cont_delay);

//   enable.password = *((uint16_t *)(FLASH_EE_START + 176U));
   amfFlashRead16(FLASH_EE_START, 176, &enable.password);
//   para.password = *((uint32_t *)(FLASH_EE_START + 180U));
   amfFlashRead32(FLASH_EE_START, 180, &para.password);

   amfFlashRead32(FLASH_EE_START, 184, &para.btspassword);

   amfFlashRead16(FLASH_EE_START, 232, &calibvolt.dgbat);
   amfFlashRead16(FLASH_EE_START, 228, &calibreading.dgbat);
   ratio.dgbat = ((float)calibvolt.dgbat/(float)calibreading.dgbat)/100.0;
//   calibreading.bandgapdgbat = *((uint16_t *)(FLASH_EE_START + 236U));
   amfFlashRead16(FLASH_EE_START, 236, &calibreading.bandgapdgbat);

//   calibvolt.phase = *((uint16_t *)(FLASH_EE_START + 240U));
   amfFlashRead16(FLASH_EE_START, 240, &calibvolt.phase);
//   calibreading.ebr = *((uint16_t *)(FLASH_EE_START + 244U));
   amfFlashRead16(FLASH_EE_START, 244, &calibreading.ebr);
//   calibreading.eby = *((uint16_t *)(FLASH_EE_START + 248U));
   amfFlashRead16(FLASH_EE_START, 248, &calibreading.eby);
//   calibreading.ebb = *((uint16_t *)(FLASH_EE_START + 252U));
   amfFlashRead16(FLASH_EE_START, 252, &calibreading.ebb);
//   calibreading.dgr = *((uint16_t *)(FLASH_EE_START + 256U));
   amfFlashRead16(FLASH_EE_START, 256, &calibreading.dgr);
//   calibreading.dgy = *((uint16_t *)(FLASH_EE_START + 260U));
   amfFlashRead16(FLASH_EE_START, 260, &calibreading.dgy);
//   calibreading.dgb = *((uint16_t *)(FLASH_EE_START + 264U));
   amfFlashRead16(FLASH_EE_START, 264, &calibreading.dgb);
   ratio.ebr = ((float)calibvolt.phase/(float)calibreading.ebr);
   ratio.eby = ((float)calibvolt.phase/(float)calibreading.eby);
   ratio.ebb = ((float)calibvolt.phase/(float)calibreading.ebb);
   ratio.dgr = ((float)calibvolt.phase/(float)calibreading.dgr);
   ratio.dgb = ((float)calibvolt.phase/(float)calibreading.dgb);
   ratio.dgy = ((float)calibvolt.phase/(float)calibreading.dgy);
//   calibreading.bandgapphase =  *((uint16_t *)(FLASH_EE_START + 268U));
   amfFlashRead16(FLASH_EE_START, 268, &calibreading.bandgapphase);

   //bts calibration
   amfFlashRead16(FLASH_EE_START, 276, &calibvolt.bts1);
   amfFlashRead16(FLASH_EE_START, 272, &calibreading.bts1);
   ratio.btsbat1 = ((float)calibvolt.bts1/(float)calibreading.bts1)/100.0;
   amfFlashRead16(FLASH_EE_START, 280, &calibreading.bandgapbtsbat1);

   //imbalance tolerance
   amfFlashRead16(FLASH_EE_START, 284, &para.imb_tolerance);

   //bts password enable
   amfFlashRead16(FLASH_EE_START, 288, &enable.btspassword);


}


void amfFaultReset(void){
   fault.common = 0;
   fault.dgovervolt = 0; fault.dgundervolt = 0; fault.emstop = 0; fault.external = 0;
   fault.ht = 0; fault.llop = 0;fault.lowfuel = 0; fault.overload = 0; fault.overspeed = 0;
   fault.start = 0; fault.stop = 0; fault.underspeed = 0; fault.flt1 = 0; fault.alternator = 0;

   tempfault.dgovervolt = 0; tempfault.dgundervolt = 0; tempfault.flt1 = 0; tempfault.ht = 0;
   tempfault.llop = 0; tempfault.lowfuel = 0; tempfault.overload = 0; tempfault.overspeed = 0;
   tempfault.underspeed = 0;
   status.engine_running = 0;
   //status.engine_stopped = 0; //this line is commented because after reset is done,
                        //  automatic stop is triggered.
   status.hooter_done = 0; status.ebimbalance = 0;
   fault.errcode01 = 0; fault.errcode02 = 0; fault.errcode03 = 0; fault.errcode04 = 0;
   warning.bts1low = 0; warning.common = 0; warning.lowfuel = 0; 
   fault.rom_error_segment = 0;
     
   downcounter.stop_attempts = 0; downcounter.start_attempts = 0;

   FAULT_IND_OFF();
}

void resetVariables(void){
   ratio.btsbat1 = .018;
   ratio.dgbat = .052;
   ratio.ebr = 1;
   ratio.ebb = 1;
   ratio.eby = 1;
   ratio.dgr = 1;
   ratio.dgb = 1;
   ratio.dgy = 1;

   flag.max_display_items = 4; flag.scroll_lock = 0; flag.scroll_lock_temp = 0; flag.hold_disp_lock = 0;


   process_on.mainsfail = 0; process_on.ebrestore = 0; process_on.enginestartup = 0;
   process_on.recool = 0; process_on.stop = 0; process_on.hooter = 0; process_on.crank = 0;
   process_on.ebonload = 0; process_on.dgonload = 0; process_on.totalfail = 0;
   process_on.fault_delay = 0; process_on.cont_delay =0; process_on.hooter = 0;
   process_on.lfl_delay = 0; process_on.llop_delay = 0; process_on.trip = 0;
   process_on.warning_delay = 0;

   status.dgon = 0; status.buildingup =0; status.m_cont_delay_ok= 1;status.dg_cont_delay_ok = 1;
   status.cycleoff_timeout = 1;
   status.cycleon = 0; status.cycleon_timeout = 0;  status.dg_ok_to_load = 0;
   status.dg_resting = 0; status.dghealthy = 0; status.ebfail = 1; status.ebfail_timedout = 0;
   status.ebhealthy = 0; status.engine_running = 0; status.engine_stopped = 1;
   status.recooled = 0;  status.scheduler_ok = 0; status.llop_timeout = 0;
   status.hooter_done = 0; status.schedule_cycle = 0; status.ebimbalance = 0;
   status.bypassstart = 0;
   status.remote_mode = 0; status.remote_start = 0; status.semiauto = 0;   status.mode = MANUAL_MODE;
   status.remote_started = 0;

   downtimer.cont_delay = 0; downtimer.crank = 0; downtimer.ebrestore = 0; downtimer.enginestartup = 0;
   downtimer.hooter = 0; downtimer.kb_delay = 0; downtimer.mainsfail = 0; downtimer.recool = 0;
   downtimer.stop = 0; downtimer.trip = 0; downtimer.llop_delay = 0; downtimer.lfl_delay = 0;
   downtimer.fault_delay = 0; downtimer.warning_delay = 0; 
     
   downcounter.stop_attempts = 0; downcounter.start_attempts = 0;

   fault.common = 0;
   fault.dgovervolt = 0; fault.dgundervolt = 0; fault.emstop = 0; fault.external = 0;
   fault.ht = 0; fault.llop = 0;fault.lowfuel = 0; fault.overload = 0; fault.overspeed = 0;
   fault.start = 0; fault.stop = 0; fault.underspeed = 0, fault.flt1 = 0;
   fault.errcode01 = 0; fault.errcode02 = 0; fault.errcode03 = 0; fault.errcode04 = 0;
   fault.rom_error_segment = 0; 

   tempfault.dgovervolt = 0; tempfault.dgundervolt = 0; tempfault.flt1 = 0; tempfault.ht = 0;
   tempfault.llop = 0; tempfault.lowfuel = 0; tempfault.overload = 0; tempfault.overspeed = 0;
   tempfault.underspeed = 0;

   tempwarning.bts1low = 0; tempwarning.highbat = 0; tempwarning.lowbat = 0;
     
   warning.bts1low = 0; warning.common = 0; warning.highbat = 0; warning.lowbat = 0;
   warning.lowfuel = 0; 

   uptimer.dgdowntime = 0; uptimer.dguptime = 0;
   uptimer.totalrun = 0; uptimer.totalrunhr = 0; uptimer.totalrunmin = 0;
     
   para.bts1low = 48; para.btspassword = 12345; para.cycleoffhr = 1; para.cycleoffmin = 0;
   para.cycleonhr = 8; para.cycleonmin = 0; para.dgbathigh = 14; para.dgbatlow =11; 
   para.dgundervolt = 160; para.dgovervolt = 265; para.ebovervolt = 265; para.ebundervolt_l = 160; para.ebundervolt_h = 170;
   para.highfreq = 55; para.imb_tolerance = 10; para.imbalance = 10; para.lowfreq = 45;
   para.max_crank_no = 3; para.mfdelay_hr = 0; para.mfdelay_min = 0; para.mfdelay_low = 10;
   para.password = 12345; para.rpmperhz = 30; para.schedule_starthr = 6; para.schedule_startmin = 0;
   para.schedule_stophr = 22; para.schedule_stopmin = 0;
     
   enable.btsbypassrest = 0; enable.btsmonitoring = 0; enable.btspassword = 0; enable.cycletimer = 0;
   enable.dgsinglephase = 0; enable.ebimbalance = 0; enable.ebsinglephase = 0; enable.fuelsol = 0;
   enable.llopcheck = 0; enable.manmode = 0; enable.password = 0; enable.scheduler = 0;
     
   prog_delay.cont_delay = 1; prog_delay.crank = 3; prog_delay.cycleoff = 1000; prog_delay.cycleon = 10000;
   prog_delay.ebrestore = 30; prog_delay.enginestartup = 15; prog_delay.hooter = 10; prog_delay.lfl_delay = 60;
   prog_delay.mainsfail = 10; prog_delay.recool = 10; prog_delay.stop = 5; prog_delay.trip = 10;
     
   flag.lowvcc = 0;
}

void decalibrateWrite(void)
{

//   sector[57] = 1492;      //DG Battery calib volt reading
   amfFlash_Set32(FLASH_EE_START, 228, 231);

//   sector[58] = 1225;      // DG Battery calib volt
   amfFlash_Set32(FLASH_EE_START, 232, 1224);
     
//   sector[59] = 1562;      //dg battery calibration bandgap
   amfFlash_Set32(FLASH_EE_START, 236, 1);
     
//   sector[60] = 222;      //Phase calib volt
   amfFlash_Set32(FLASH_EE_START, 240, 0xE9);
     
//   sector[61] = 1405;      // ebr calib volt reading
   amfFlash_Set32(FLASH_EE_START, 244, 0xE9);
     
//   sector[62] = 1405;      // eby calib volt reading
   amfFlash_Set32(FLASH_EE_START, 248, 0xE9);
     
//   sector[63] = 1405;      // ebb calib volt reading
   amfFlash_Set32(FLASH_EE_START, 252, 0xE9);
     
//   sector[64] = 1405;      // dgr calib volt reading
   amfFlash_Set32(FLASH_EE_START, 256, 0xE2);
     
//   sector[65] = 1405;      // dgy calib volt reading
   amfFlash_Set32(FLASH_EE_START, 260, 0xE9);
     
//   sector[66] = 1405;      // dgb calib volt reading
   amfFlash_Set32(FLASH_EE_START, 264, 0xE9);
     
//   sector[67] = 1557;      //phase calibration bandgap
   amfFlash_Set32(FLASH_EE_START, 268, 1);
     
//   sector[69] = 6280;      //BTS Battery calib volt
   amfFlash_Set32(FLASH_EE_START, 276, 4720);
        
//   sector[68] = 1888;      // bts Battery calib volt reading
   amfFlash_Set32(FLASH_EE_START, 272, 429);
     
//   sector[70] = 1562;      //bts battery calibration bandgap
   amfFlash_Set32(FLASH_EE_START, 280, 1);
     
//   sector[71] = 2;         //imbalance tolerance in volts.
   amfFlash_Set32(FLASH_EE_START, 284, 2);
     
}

void errorCheckRomData(void){

   uint32_t sector[75] = {0};
   uint16_t temp_count;
   uint8_t correction_required = 0;

//   disableAmfInterrupts();

   for(temp_count =0; temp_count<75; temp_count++){
      amfFlashRead32(FLASH_EE_START , temp_count*4, &sector[temp_count]);
   }
      ////to know whether rom values are written initially
   if(sector[0] != 0xAAAA){
      sector[0] = 0xAAAA;
      correction_required = 1;
      fault.rom_error_segment = 1;
   }
      ////cycle timer enable
   if((sector[1] != 1) && (sector[1] != 0)){
      sector[1] = 0;
      correction_required = 1;
      fault.rom_error_segment = 2;
   }
   if((sector[2] > 23) || (sector[3] > 59)){
   ////cycle on timer hour
      sector[2] = 8;
   ////cycle on timer minutes
      sector[3] = 0;
      correction_required = 1;
      fault.rom_error_segment = 3;
   }
   if((sector[4] > 23) || (sector[5] > 59)){
   //   amfFlashWrite16(18, 1);      //cycle off timer hour
      sector[4] = 1;
   //   amfFlashWrite16(20, 0);      //cycle off timer minutes
      sector[5] = 0;
      correction_required = 1;
      fault.rom_error_segment = 4;
   }
   //   amfFlashWrite16(22, 0);      //mains single phase enable
   if((sector[6] != 1) && (sector[6] != 0)){
      sector[6] = 0;
      correction_required = 1;
      fault.rom_error_segment = 5;
   }
   //   amfFlashWrite16(24, 130);   //eb undervoltage low
   if((sector[8] < 70) || (sector[8] > 238)){
      sector[8] = 160;
      sector[7] = 170;
      correction_required = 1;
      fault.rom_error_segment = 6;
   }
   else if(sector[8] >= sector[7]){
      sector[7] = 170;
      sector[8] = 160;
      correction_required = 1;
      fault.rom_error_segment = 6;
   }
   //   amfFlashWrite16(26, 120);   //eb undervoltage high
   if((sector[7] <= sector[8])){
      sector[7] = 170;
      sector[8] = 160;
      correction_required = 1;
      fault.rom_error_segment = 7;
   }
   else if((sector[7] < 71) || (sector[7] > 239)){
      sector[7] = 170;
      sector[8] = 160;
      correction_required = 1;
      fault.rom_error_segment = 8;
   }
   //   amfFlashWrite16(28, 270);   //eb over voltage
   if((sector[9] < 240) || (sector[9] > 999)){
      sector[9] = 260;
      correction_required = 1;
      fault.rom_error_segment = 9;
   }
   //   amfFlashWrite16(30, 0);      //imbalance enable
   if((sector[10] != 1) && (sector[10] != 0)){
      sector[10] = 0;
      correction_required = 1;
      fault.rom_error_segment = 10;
   }
   //   amfFlashWrite16(32, 10);   //imbalance percent
   if((sector[11] > 50)){
      sector[11] = 10;
      correction_required = 1;
      fault.rom_error_segment = 11;
   }
   //   amfFlashWrite16(34, 0);      //dg single phase
   if((sector[12] != 1) && (sector[12] != 0)){
      sector[12] = 0;
      correction_required = 1;
      fault.rom_error_segment = 12;
   }
   //   amfFlashWrite16(36, 180);   //dg under voltage
   if((sector[13] < 90) || (sector[13] > 239)){
      sector[13] = 180;
      correction_required = 1;
      fault.rom_error_segment = 13;
   }
   //   amfFlashWrite16(38, 270);   //dg over voltage
   if((sector[14] < 240) || (sector[14] > 999)){
      sector[14] = 260;
      correction_required = 1;
      fault.rom_error_segment = 14;
   }
   //   amfFlashWrite16(40, 45);   //low frequency
   if((sector[15]  > 49)){
      sector[15] = 45;
      correction_required = 1;
      fault.rom_error_segment = 15;
   }
   //   amfFlashWrite16(42, 55);   //high frequency
   if((sector[15] > sector[16])){
      sector[16] = sector[15] + 10;
      correction_required = 1;
      fault.rom_error_segment = 16;
   }
   else if((sector[16] < 50) || (sector[16] > 999)){
      sector[16] = 55;
      correction_required = 1;
      fault.rom_error_segment = 17;
   }
   //   amfFlashWrite16(44, 0);      //llop check to crank
   if((sector[17] != 1) && (sector[17] != 0)){
      sector[17] = 0;
      correction_required = 1;
      fault.rom_error_segment = 18;
   }
   //   amfFlashWrite16(46, 10);   //mains fail delay
   if((sector[18] < 1) || (sector[18] > 999)){
      sector[18] = 10;
      correction_required = 1;
      fault.rom_error_segment = 19;
   }
   //   amfFlashWrite16(48, 0);      //mains fail delay extended hour
   if((sector[19] > 23)){
      sector[19] = 0;
      correction_required = 1;
      fault.rom_error_segment = 20;
   }
   //   amfFlashWrite16(50, 0);      //mains fail delay extended minute
   if((sector[20] > 59)){
      sector[20] = 0;
      correction_required = 1;
      fault.rom_error_segment = 21;
   }
   //   amfFlashWrite16(52, 3);      //cranking time
   if((sector[21] > 10)){
      sector[21] = 3;
      correction_required = 1;
      fault.rom_error_segment = 22;
   }
   //   amfFlashWrite16(54, 3);      //crank attempts
   if((sector[22] > 10)){
      sector[22] = 3;
      correction_required = 1;
      fault.rom_error_segment = 23;
   }
   //   amfFlashWrite16(56, 10);   //dg buildup time
   if((sector[23] > 999)){
      sector[23] = 10;
      correction_required = 1;
      fault.rom_error_segment = 24;
   }
   //   amfFlashWrite16(58, 10);   //mains restore delay
   if((sector[24] > 999)){
      sector[24] = 10;
      correction_required = 1;
      fault.rom_error_segment = 25;
   }
   //   amfFlashWrite16(60, 10);   //fault trip delay
   if((sector[25] > 60)){
      sector[25] = 10;
      correction_required = 1;
      fault.rom_error_segment = 26;
   }
   //   amfFlashWrite16(62, 10);   //recool delay
   if((sector[26] > 999)){
      sector[26] = 15;
      correction_required = 1;
      fault.rom_error_segment = 27;
   }
   //   amfFlashWrite16(64, 0);      //fuel solenoid enable
   if((sector[27] != 1) && (sector[27] != 0)){
      sector[27] = 0;
      correction_required = 1;
      fault.rom_error_segment = 28;
   }
   //   amfFlashWrite16(66, 5);      //stopper time
   if((sector[28] > 999) || (sector[28] < 1)){
      sector[28] = 15;
      correction_required = 1;
      fault.rom_error_segment = 29;
   }
   //   amfFlashWrite16(68, 10);   //hooter reset time
   if((sector[29] > 999)){
      sector[29] = 10;
      correction_required = 1;
      fault.rom_error_segment = 30;
   }
   //   amfFlashWrite16(70, 110);   //dg battery low multiplied by 10
   if((sector[30] < 70) || (sector[30] > 340) || (sector[30] > (sector[31] - 10))){
      sector[30] = 110;
      sector[31] = 150;
      correction_required = 1;
      fault.rom_error_segment = 31;
   }
   //   amfFlashWrite16(72, 150);   //dg battery high, x10
   if((sector[31] < 80) || (sector[31] > 350)){
      sector[31] = 150;
      correction_required = 1;
      fault.rom_error_segment = 32;
   }
   if(sector[31] < sector[30]){
      sector[30] = 110;
      sector[31] = 150;
      correction_required = 1;
      fault.rom_error_segment = 33;
   }
   //   amfFlashWrite16(74, 30);   //rpm/hz ratio
   if((sector[32] > 999) || (sector[32] < 1)){
      sector[32] = 30;
      correction_required = 1;
      fault.rom_error_segment = 34;
   }
   //   amfFlashWrite16(76, 0);      //scheduler enable
   if((sector[33] != 1) && (sector[33] != 0)){
      sector[33] = 0;
      correction_required = 1;
      fault.rom_error_segment = 35;
   }
   //   amfFlashWrite16(78, 6);      //scheduler start hour
   if((sector[34] > 23)){
      sector[34] = 6;
      correction_required = 1;
      fault.rom_error_segment = 36;
   }
   //   amfFlashWrite16(80, 0);      //scheduler start minutes
   if((sector[35] > 59)){
      sector[35] = 0;
      correction_required = 1;
      fault.rom_error_segment = 38;
   }
   //   amfFlashWrite16(82, 22);   //scheduler stop hour
   if((sector[36] > 23)){
      sector[36] = 22;
      correction_required = 1;
      fault.rom_error_segment = 39;
   }
   //   amfFlashWrite16(84, 0);      //scheduler stop minutes
   if((sector[37] > 59)){
      sector[37] = 0;
      correction_required = 1;
      fault.rom_error_segment = 40;
   }
   //   amfFlashWrite16(86, 0);      //flash dummy, write 0xFFFF
      sector[38] = 0xFFFF;
   //   amfFlashWrite16(88, 0);      //bts monitoring
   if((sector[39] != 1) && (sector[39] != 0)){
      sector[39] = 0;
      correction_required = 1;
      fault.rom_error_segment = 41;
   }
   //   amfFlashWrite16(90, 480);   //bts battery low voltage
   if((sector[40] < 70) || (sector[40] > 999)){
      sector[40] = 480;
      correction_required = 1;
      fault.rom_error_segment = 42;
   }
   //   amfFlashWrite16(92, 0);      //bypass rest on low battery
   if((sector[41] != 1) && (sector[41] != 0)){
      sector[41] = 0;
      correction_required = 1;
      fault.rom_error_segment = 43;
   }
   //   amfFlashWrite16(96, 5);      //low fuel delay
   if((sector[42] > 999)){
      sector[42] = 60;
      correction_required = 1;
      fault.rom_error_segment = 44;
   }
   //   amfFlashWrite16(98, 1);      //contactor delay
   if((sector[43] > 999)){   //removed condition less than 1, to allow fast switching
      sector[43] = 1;
      correction_required = 1;
      fault.rom_error_segment = 45;
   }
   //   amfFlashWrite16(100, 0);   //password enable
   if((sector[44] != 1) && (sector[44] != 0)){
      sector[44] = 0;
      correction_required = 1;
      fault.rom_error_segment = 46;
   }

   if(sector[45]  > 99999){   //reset password only if corrupted
   //   amfFlashWrite16(102, 12345); //password lsb of 32 bits
      sector[45] = 12345;
      correction_required = 1;
      fault.rom_error_segment = 47;
   }

   if(sector[46]  > 99999){   //reset bts password only if corrupted
   //   amfFlashWrite16(102, 12345); //password lsb of 32 bits
      sector[46] = 12345;
      correction_required = 1;
      fault.rom_error_segment = 48;
   }

   if(sector[71]  > 15){   //imbalance tolerance
      sector[71] = 2;
      correction_required = 1;
      fault.rom_error_segment = 49;
   }

   if((calibvolt.bts1 < 1000) || (calibvolt.bts1 > 35000)
         || (calibreading.bts1 < 100) || (calibreading.bts1 > 1000)
         || (calibreading.bandgapbtsbat1 < 1) || (calibreading.bandgapbtsbat1 > 1)
         || (ratio.btsbat1 < 0.005) || (ratio.btsbat1 > .3))
   {
      sector[69] = 4720;      //BTS Battery calib volt
      sector[68] = 429;      // bts Battery calib volt reading
      sector[70] = 1;      //bts battery calibration bandgap

      fault.rom_error_segment = 50;
      correction_required = 1;

   }
   if((calibreading.bandgapdgbat < 1) || (calibreading.bandgapdgbat > 1 )
         || (calibreading.bandgapphase < 1) || (calibreading.bandgapphase > 1)
         || (ratio.dgb < .01) || (ratio.dgr < .01) || (ratio.dgy < .01) || (ratio.dgbat < .001)
         || (ratio.ebr < .01) || (ratio.eby < .01) || (ratio.ebb < .01))
   {
      correction_required = 1;

      sector[57] = 231;      //DG Battery calib volt   reading

      sector[58] = 1224;      // DG Battery calib volt

      sector[59] = 1;      //dg battery calibration bandgap

      sector[60] = 0xE9;      //Phase calib volt

      sector[61] = 0xE9;      // ebr calib volt reading

      sector[62] = 0xE9;      // eby calib volt reading

      sector[63] = 0xE9;      // ebb calib volt reading

      sector[64] = 0xE2;      // dgr calib volt reading

      sector[65] = 0xE9;      // dgy calib volt reading

      sector[66] = 0xE9;      // dgb calib volt reading

      sector[67] = 1;      //phase calibration bandgap
      //amfDelayMs(50);
      fault.rom_error_segment = 51;
   }

   //bts password enable
   if((sector[72] != 1) && (sector[72] != 0)){
      sector[72] = 0;
      correction_required = 1;
      fault.rom_error_segment = 52;
   }

   if(correction_required == 1){
      warning.common = 1;
      fault.errcode01 = 1;
   }

   if(correction_required == 1){

      for(temp_count =0; temp_count<75; temp_count++){
        amfFlash_Set32(FLASH_EE_START, temp_count*4, sector[temp_count]);
      }

   }

}


void amfRomWriteDefault(void){

   uint32_t sector[46];
   uint16_t temp_count;

   for(temp_count =0; temp_count<46; temp_count++){
      amfFlashRead32(FLASH_EE_START , temp_count*4, &sector[temp_count]);
   }

   sector[0] = 0xAAAA;   //to know whether rom values are written initially

   sector[1] = 0;      //cycle timer enable

   sector[2] = 8;      //cycle on timer hour

   sector[3] = 0;      //cycle on timer minutes

   sector[4] = 1;      //cycle off timer hour

   sector[5] = 0;      //cycle off timer minutes

   sector[6] = 0;      //mains single phase enable

   sector[7] = 170;   //eb undervoltage high

   sector[8] = 160;   //eb undervoltage low

   sector[9] = 260;   //eb over voltage

   sector[10] = 0;      //imbalance enable

   sector[11] = 10;   //imbalance percent

   sector[12] = 1;      //dg single phase

   sector[13] = 180;   //dg under voltage

   sector[14] = 260;   //dg over voltage

   sector[15] = 45;   //low frequency

   sector[16] = 55;   //high frequency

   sector[17] = 0;      //llop check to crank

   sector[18] = 10;   //mains fail delay

   sector[19] = 0;      //mains fail delay extended hour

   sector[20] = 0;      //mains fail delay extended minute

   sector[21] = 3;      //cranking time

   sector[22] = 3;      //crank attempts

   sector[23] = 10;   //dg buildup time

   sector[24] = 30;   //mains restore delay

   sector[25] = 10;   //fault trip delay

   sector[26] = 15;   //recool delay

   sector[27] = 0;      //fuel solenoid enable

   sector[28] = 15;      //stopper time

   sector[29] = 10;   //hooter reset time

   sector[30] = 110;   //dg battery low multiplied by 10

   sector[31] = 150;   //dg battery high, x10

   sector[32] = 30;   //rpm/hz ratio

   sector[33] = 0;      //scheduler enable

   sector[34] = 6;      //scheduler start hour

   sector[35] = 0;      //scheduler start minutes

   sector[36] = 22;   //scheduler stop hour

   sector[37] = 0;      //scheduler stop minutes

   sector[38] = 0;      //flashdummy, write 0xFFFF

   sector[39] = 0;      //bts monitoring

   sector[40] = 480;   //bts battery low voltage

   sector[41] = 0;      //bypass rest on low battery

   sector[42] = 60;      //low fuel delay

   sector[43] = 1;      //contactor delay

   sector[44] = 0;      //password enable

   for(temp_count =0; temp_count<46; temp_count++){
     amfFlash_Set32(FLASH_EE_START, temp_count*4, sector[temp_count]);
   }

}


