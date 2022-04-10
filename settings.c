#include "time.h"

#define MAXMENU 37

void setter(uint8_t x);

void subset_calib(void);

void subsetter_calib(uint8_t x);

void subsetter_bts(uint8_t x);

void subset_bts(void);


const char set[40][17]=
{
   "MainsSinglePhase",            //0
   "MainsUndrvoltHgh",            //1
   "MainsUndrvoltLow",            //2
   "MainsOverVoltage",            //3
   "Mains Imbalance ",            //4
   "DG Single Phase ",            //5
   "DG UnderVoltage ",            //6
   "DG OverVoltage  ",            //7
   "DG UnderSpeed   ",            //8
   "DG OverSpeed    ",            //9
   "Safe Cranking   ",            //10
   "MainsFailDelay  ",            //11
   "FailDelyExtended",            //12
   "Cranking Time   ",            //13
   "Crank Attempts  ",            //14
   "DG BuildUp Delay",            //15
   "MainsRestorDelay",            //16
   "Fault Trip Delay",            //17
   "DG Recool Delay ",            //18
   "SolnoidOnToStart",            //19
   "DG Stopper Time ",            //20
   "HooterResetTime ",            //21
   "Contactor Delay ",            //22
   "Low Fuel Delay  ",            //23
   "DG Bat Low Level",            //24
   "DG Bat HighLevel",            //25
   "Engine RPM/HZ   ",            //26
   "Run - Rest Cycle",            //27
   "Scheduler       ",            //28
   "Time-24Hr Clock ",            //29
   "Date Settings   ",            //30
   "FactoryDefaults?",            //31
   "Calibration     ",            //32
   "Password Options",            //33
   "Event Log       ",            //34
   "Clear Event Log ",            //35
   "BtsBatMonitoring",            //36
   "DisplayScrolLock"            //37
  
};

const char set_calib[5][17] =
{
   "Calibrate DGBat?", //0
   "CalibratePhases?", //1
   "Decalibrate?    ", //2
   "Imbal Tolerance ", //3
   "Edit RunHour?   "  //4
};

const char set_bts[6][17] = {
   "Enabl/Disabl BTS",   //0
   "Bts Bat LowLevel",   //1
   "Calibrate BtsBat",   //2
   "BypasRestOnLoBat",   //3
   "BTS Passwd Enabl",   //4
   "ChangeBtsPasswd?"   //5
};

static void PrintSec()
{
   LCD_GotoXY(2,5);
   printf(lcd_putc, "Sec");
}

static void PrintPercent()
{
   char temp_s[8] = "    %  " ;
   LCD_GotoXY(2,1);
   LCD_WriteString(temp_s);
}

static void PrintHrMin()
{
   LCD_GotoXY(2,7);
   printf(lcd_putc,  "Hr:Min");
}

static void PrintVolt()
{
   LCD_GotoXY(2,7);
   printf(lcd_putc, "Volts");
}

static void PrintHz()
{
   LCD_GotoXY(2,5);
   printf(lcd_putc, "Hz");
}


static void Printx100mV()
{
   LCD_GotoXY(2,5);
   printf(lcd_putc, "x100 mV");
}

void settings(void)
{

   uint8_t ivar=0, kb=KEY_NONE;
   uint8_t max_menu_no= MAXMENU;
   char temp_lcd_string[17];
   volatile uint32_t temp32;
   volatile uint16_t temp16;

   disable_interrupts(INT_RB);
   //disable_interrupts(INT_EXT);

   LCD_Init();
   printf(lcd_putc, "\fAMF Settings");
   amfDelayLongMs(1000);

   if(enable.password == 1){
      LCD_Init();
      printf(lcd_putc, "\fEnter Password");
      temp32 = GetPassWord32();
      if((temp32 != para.password) && (temp32 != PASSWORD_MASTER)){
         LCD_Init();
         printf(lcd_putc, "\fWrong Password");
         amfDelayLongMs(1000);
//         KBI1->SC |= KBI_SC_KBIE_MASK;      //enable kbi interrupt upon exit
         return;
      }
   }
     
start:  
     
   LCD_Init();
   sprintf(temp_lcd_string, "%s",set[ivar]);
   LCD_WriteLineStr(1, temp_lcd_string);

   LCD_GotoXY(1,1);
   LCD_BlinkingOn();
   LCD_CursorOn();
     
   if(ivar==max_menu_no){
      sprintf(temp_lcd_string, "%s", set[0]);
      LCD_WriteLineStr(2, temp_lcd_string);
   }
   else{
      sprintf(temp_lcd_string, "%s", set[ivar+1]);
      LCD_WriteLineStr(2, temp_lcd_string);
   }
   LCD_GotoXY(1,1);
           
   while(TRUE)
   {   
         kb = TimedKBInput();
         if(kb == KEY_UP)
         {
             if(ivar > 0){
                  ivar -= 1;
             }else{
                ivar = max_menu_no;
             }
         }
         else if(kb==KEY_DOWN)
         {
             if(ivar < max_menu_no){
                  ivar += 1;
             }else {
                ivar=0;
             }
         }
         else if(kb == KEY_SET)
         {
             LCD_Init();
             amfDelayLongMs(500);
             setter(ivar);
             kb=KEY_NONE;
             goto start;
         }
         else 
             break;
        
      LCD_Init();
      sprintf(temp_lcd_string, "%s",set[ivar]);
      LCD_WriteLineStr(1, temp_lcd_string);
      LCD_GotoXY(1,1);
      LCD_CursorOn();
      LCD_BlinkingOn();
           
      if(ivar==max_menu_no){
         sprintf(temp_lcd_string, "%s", set[0]);
         LCD_WriteLineStr(2, temp_lcd_string);
         LCD_GotoXY(1,1);
          //ivar=0;
      }
      else{
         sprintf(temp_lcd_string, "%s", set[ivar+1]);
         LCD_WriteLineStr(2, temp_lcd_string);
      }
      LCD_GotoXY(1,1);
      kb=KEY_NONE;
      amfDelayLongMs(500);
   }
     
   //dummy read to point the address away to an
   //address that is not used.
   Eprom_Read16(10, &temp16);
   amfFlashRead32(FLASH_EE_START, 152, &temp32);
     
   enable_interrupts(INT_RB);
   return;
}

void setter(uint8_t x){
     
   char temp_lcd_string[17];
   uint16_t temp_data, temp_data2;
   uint32_t temp32;
   LCD_Init();
   sprintf(temp_lcd_string, "%s", set[x]);
   LCD_WriteLineStr(1, temp_lcd_string);    
     
   switch(x) {
        
      case 0:{      //mains single phase; Flash info: 22
         enable.ebsinglephase = EnabledDisabled_Flash(FLASH_EE_START, 24);
         break;        
      }
        
      case 1:{      //mains undervoltage high; Flash info 24
         PrintVolt();
         para.ebundervolt_h = SetValue16(FLASH_EE_START, 28, para.ebundervolt_l + 1, 239);
         break;
      }
        
      case 2:{      //mains undervoltage low; Flash info 26
         PrintVolt();
         para.ebundervolt_l = SetValue16(FLASH_EE_START, 32, 70, para.ebundervolt_h - 1 );
         break;
      }
        
      case 3:{      //mains overvoltage      //Flash info 28
         PrintVolt();
         para.ebovervolt = SetValue16(FLASH_EE_START, 36, 240, 500);
         break;
      }
        
      case 4:{      //mains imbalance      //Flash info 30 - 32
         if((enable.ebimbalance = EnabledDisabled_Flash(FLASH_EE_START, 40)) == 1){   //if imbalance enabled
            PrintPercent();
            LCD_GotoXY(2,11);
            printf(lcd_putc,  "      ");
            para.imbalance = SetValue16(FLASH_EE_START, 44, 1, 50);
         }
         break;
      }
        
      case 5:{         //DG Single Phase ; Flash info : 34
         enable.dgsinglephase = EnabledDisabled_Flash(FLASH_EE_START, 48);
         break;
      }
        
      case 6:{      //DG Undervoltage ; Flash info : 36
         PrintVolt();
         para.dgundervolt = SetValue16(FLASH_EE_START, 52, 90, 239);
         break;
      }
        
      case 7:{      //DG Overvoltage ; Flash info : 38
         PrintVolt();
         para.dgovervolt = SetValue16(FLASH_EE_START, 56, 240, 500);
         break;
      }
        
      case 8:{      //DG Underspeed ; Flash info: 40
         PrintHz();
         para.lowfreq = (float)SetValue16(FLASH_EE_START, 60, 30, 49);
         break;
      }
        
      case 9:{         //DG Overspeed ; Flash info : 42
         PrintHz();
         para.highfreq = (float)SetValue16(FLASH_EE_START, 64, 50, 100);
         break;     
      }
        
      case 10:{         //LLOP Check to crank ; Flash Info : 44
         printf(lcd_putc, "\fLLOPCheckToCrank");
         enable.llopcheck = EnabledDisabled_Flash(FLASH_EE_START, 68);
         break;        
      }
        
      case 11:{         //Mains Fail Delay ; Flash info : 46
         PrintSec();
         para.mfdelay_low = SetValue16(FLASH_EE_START, 72, 1, 999);
         prog_delay.mainsfail = (uint32_t)para.mfdelay_hr*3600 + (uint32_t)para.mfdelay_min*60 + (uint32_t)para.mfdelay_low;
         break;
      }
        
      case 12:{         //Mains fail delay extended Flash info : 48 - 50
         if(ReadTimeInput(&para.mfdelay_hr, &para.mfdelay_min) == 1)
         {
            amfFlash_Set32(FLASH_EE_START, 76, (uint32_t)para.mfdelay_hr);
            amfFlash_Set32(FLASH_EE_START, 80, (uint32_t)para.mfdelay_min);
            PrintSaved();
         }else {
            amfFlashRead16(FLASH_EE_START, 76, &para.mfdelay_hr);
            amfFlashRead16(FLASH_EE_START, 80, &para.mfdelay_min);
         }
         prog_delay.mainsfail = (uint32_t)para.mfdelay_hr*3600 + (uint32_t)para.mfdelay_min*60 + (uint32_t)para.mfdelay_low;
         break;
      }
        
      case 13:{         //Cranking Time ; Flash info : 52
         PrintSec();
         prog_delay.crank = SetValue16(FLASH_EE_START, 84, 1, 10);
         break;
      }
        
      case 14:{         //Cranking attempts: Flash info 54
         para.max_crank_no = SetValue16(FLASH_EE_START, 88, 1, 10);
         break;
      }
        
      case 15:{         //DG Buildup Time : Flash info 56
         PrintSec();
         prog_delay.enginestartup = SetValue16(FLASH_EE_START, 92, 1, 999);
         break;
      }
        
      case 16:{         //Mains Restore Delay; Flash info : 58
         PrintSec();
         prog_delay.ebrestore = SetValue16(FLASH_EE_START, 96, 1, 999);
         break;
      }
        
      case 17:{      //Fault Trip delay : Flash info : 60
         PrintSec();
         prog_delay.trip = SetValue16(FLASH_EE_START, 100, 1, 60);
         break;
      }
        
      case 18:{         //DG Recool delay : Flash info : 62
         PrintSec();
         prog_delay.recool = SetValue16(FLASH_EE_START, 104, 1, 999);
         break;
      }
        
      case 19:{         //Solenoid On to Start: Flash info 64
         enable.fuelsol = EnabledDisabled_Flash(FLASH_EE_START, 108);
         break;
      }
        
      case 20:{      //DG Stopper Time: Flash info 66
         PrintSec();
         prog_delay.stop = SetValue16(FLASH_EE_START, 112, 1, 999);
         break;        
      }
        
      case 21:{         // Hooter Reset time; Flash info : 68
         PrintSec();
         prog_delay.hooter = SetValue16(FLASH_EE_START, 116, 1, 999);
         break;
      }
        
      case 22:{      //contactor delay; flash info : 98
         PrintSec();
         prog_delay.cont_delay = SetValue16(FLASH_EE_START, 172, 0, 999);
         break;
      }

      case 23:{      //low fuel delay; flash info : 96
         PrintSec();
         prog_delay.lfl_delay = SetValue16(FLASH_EE_START, 168, 1, 999);
         break;
      }

      case 24:{      //DG Battery Low level; Flash info 70
         Printx100mV();
         temp_data2 = (uint16_t)(para.dgbathigh * 10.0);
         temp_data = SetValue16(FLASH_EE_START, 120, 90, temp_data2 - 10);
         para.dgbatlow = (float)temp_data / 10.0 ;
         break;
      }
        
      case 25:{      //DG Battery High level; Flash info 72
         Printx100mV();
         temp_data2 = (uint16_t)(para.dgbatlow * 10.0);           
         temp_data = SetValue16(FLASH_EE_START, 124, temp_data2 + 10 , 350);
         para.dgbathigh = (float)temp_data / 10.0 ;
         break;
      }
        
      case 26:{         //Engine RPM/Hz ; Flash info : 74
         para.rpmperhz = SetValue16(FLASH_EE_START, 128, 1, 999);
         break;
      }
        
      case 27:{               //Cycle Timer : Flash info: 12 - 20
         amfFlashRead16(FLASH_EE_START, 4, &enable.cycletimer);
         //write code for default value error here
         if(EnabledDisabled_Flash(FLASH_EE_START, 4) == 1){   //if cycle timer is enabled

            LCD_Init();
            printf(lcd_putc, "\fAutoDGOnTimeMax ");
            PrintHrMin();
            if(ReadTimeInput(&para.cycleonhr, &para.cycleonmin) == 1){
               temp32 = prog_delay.cycleon;
               amfFlash_Set32(FLASH_EE_START, 8,(uint32_t)para.cycleonhr );
               amfFlash_Set32(FLASH_EE_START, 12,(uint32_t)para.cycleonmin );
               prog_delay.cycleon = (uint32_t)para.cycleonhr * 3600 + (uint32_t)para.cycleonmin * 60 ;
               PrintSaved();
            }else{   //if exited without saving
               amfFlashRead16(FLASH_EE_START, 8, &para.cycleonhr);
               amfFlashRead16(FLASH_EE_START, 12, &para.cycleonmin);
            }

            LCD_Init();
            printf(lcd_putc, "\fAutoDGRestTime  ");
            PrintHrMin();
            if(ReadTimeInput(&para.cycleoffhr, &para.cycleoffmin) == 1){
               amfFlash_Set32(FLASH_EE_START, 16, (uint32_t)para.cycleoffhr);
               amfFlash_Set32(FLASH_EE_START, 20, (uint32_t)para.cycleoffmin);
               prog_delay.cycleoff = (uint32_t)para.cycleoffhr * 3600 + (uint32_t)para.cycleoffmin * 60 ;
               PrintSaved();
            }else{  //if exited without saving
               amfFlashRead16(FLASH_EE_START, 16, &para.cycleoffhr);
               amfFlashRead16(FLASH_EE_START, 20, &para.cycleoffmin);
            }

         }else{}

         amfFlashRead16(FLASH_EE_START, 4, &enable.cycletimer);

         break;
      }

      case 28:{         //Scheduler; Flash info : 76 - 84
         amfFlashRead16(FLASH_EE_START, 132, &enable.scheduler);
         //write code for default value error here
         if(EnabledDisabled_Flash(FLASH_EE_START, 132) == 1){   //if scheduler is enabled
              
            LCD_Init();
            printf(lcd_putc, "\fSchedulStartTime");
            PrintHrMin();
            if(ReadTimeInput(&para.schedule_starthr, &para.schedule_startmin) == 1){
               amfFlash_Set32(FLASH_EE_START, 136, (uint32_t)para.schedule_starthr);
               amfFlash_Set32(FLASH_EE_START, 140, (uint32_t)para.schedule_startmin);
               timestamp.scheduled_start = para.schedule_starthr * 60 + para.schedule_startmin ;
               PrintSaved();
            }else{   //if exited without saving
               amfFlashRead16(FLASH_EE_START, 136, &para.schedule_starthr);
               amfFlashRead16(FLASH_EE_START, 140, &para.schedule_startmin);
            }
              
            LCD_Init();
            printf(lcd_putc, "\fSchedulStopTime");
            PrintHrMin();              
            if(ReadTimeInput(&para.schedule_stophr, &para.schedule_stopmin) == 1){
               amfFlash_Set32(FLASH_EE_START, 144, (uint32_t)para.schedule_stophr);
               amfFlash_Set32(FLASH_EE_START, 148, (uint32_t)para.schedule_stopmin);
               timestamp.scheduled_stop = ((uint16_t)para.schedule_stophr * 60) + (uint16_t)para.schedule_stopmin ;
               PrintSaved();
            }else{  //if exited without saving
               amfFlashRead16(FLASH_EE_START, 144, &para.schedule_stophr);
               amfFlashRead16(FLASH_EE_START, 148, &para.schedule_stopmin);
            }
                    
         }else{}
         amfFlashRead16(FLASH_EE_START, 132, &enable.scheduler);

         break;
      }
        
      case 29:{      //Time : 24 hour clock ; Flash info: no flash
         amfGetRTCTimeExternal(&amftime);
         temp_data = (uint16_t)amftime.tm_hour;
         temp_data2 = (uint16_t)amftime.tm_min;
         if(ReadTimeInput(&temp_data, &temp_data2) == 1){
            amftime.tm_hour = (int)temp_data ;
            amftime.tm_min = (int)temp_data2;
            amftime.tm_sec =0;
            mktime(&amftime);
            amfSetRTCTimeExternal(&amftime);
            PrintSaved();

         }else  {   //if exited without saving

         }
         break;
      }
        
      case 30:{         //Date Settings; Flash info: 86 //flash for dummy writing
         amfGetRTCTimeExternal(&amftime);

         LCD_Init();
         sprintf(temp_lcd_string, "Enter Day");
         LCD_WriteLineStr(1, temp_lcd_string);    
         amftime.tm_mday = (uint8_t)GetValue16((uint16_t)amftime.tm_mday, 1, 31);
           
         LCD_Init();
         sprintf(temp_lcd_string, "Enter Month");
         LCD_WriteLineStr(1, temp_lcd_string);    
         amftime.tm_mon = (uint8_t)GetValue16((uint16_t)(amftime.tm_mon + 1), 1, 12) - 1;
           
         LCD_Init();
         sprintf(temp_lcd_string, "Enter Year");
         LCD_WriteLineStr(1, temp_lcd_string);    
         amftime.tm_year = (uint16_t)GetValue16((uint16_t)(amftime.tm_year - 100), 18, 40) + 100;

         mktime(&amftime);
         amfSetRTCTimeExternal(&amftime);
         break;
      }
        
      case 31:{         //Factory Defaults: Flash info: No flash
         if(YesOrNo(0) == 1){
            amfRomWriteDefault();
            amfRomInit();
            LCD_Init();
            printf(lcd_putc,  "\fDefaults saved.");
            printf(lcd_putc,  "\nPlease restart!");
            amfDelayLongMs(2000);

         }
         else {}   //if selected no
         break;
      }        

      case 32:{      //Calibration: Flash info: Will update you

         subset_calib();
         break;

      }
        
      case 33:{         //password options: Flash info: Will update you
         LCD_Init();
         printf(lcd_putc, "\fEnable Password?");
         if(YesOrNo(enable.password) == 1){   //if yes option selected
            if(enable.password == 0){   //if password was disabled earlier
               LCD_Init();
               printf(lcd_putc, "\fEnter Password");
               temp32 = GetPassWord32();
               if((temp32 == para.password)|| (temp32 == PASSWORD_MASTER)){   //if correct password is entered
                  amfFlash_Set32(FLASH_EE_START, 176, 1);
                  enable.password = 1;
                  LCD_Init();
                  printf(lcd_putc, "\fPassword Enabled");
                  amfDelayLongMs(1000);
               }
               else{      //wrong password is entered
                  LCD_Init();
                  printf(lcd_putc, "\fWrong Password");
                  amfDelayLongMs(1000);
               }
            }
         }
         else{   //if no option is selected
            if(enable.password == 1){      //if password was enabled earlier, ie, entered menu with password
               enable.password = 0;
               LCD_Init();
               printf(lcd_putc, "\fPasswrd disabled");
               amfFlash_Set32(FLASH_EE_START, 176, 0);
               amfDelayLongMs(1000);
            }
         }

         // Change password

         LCD_Init();
         printf(lcd_putc,  "\fChange Password?");
         if(YesOrNo(0) == 1){
            if(enable.password == 0){ //if password was not entered previously, not considering the just disabled action
               LCD_Init();
               printf(lcd_putc, "\fEnterOldPasswrd");
               temp32 = GetPassWord32();
               if((temp32 != para.password) && (temp32 != PASSWORD_MASTER)){   //if incorrect password is entered
                  LCD_Init();
                  printf(lcd_putc, "\fWrong Password!");
                  amfDelayLongMs(1000);
                  return;      //return to menu if wrong password is entered
               }
            } else {}   //else don't ask for current password

            LCD_Init();
            printf(lcd_putc,  "\fEnterNewPassword");
            temp32 = GetPassWord32();
            printf(lcd_putc, "\fEnter Once Again");
            if(temp32 != GetPassWord32()){      //if the passwords do not match
               LCD_Init();
               printf(lcd_putc,  "\fPasswords Do Not");
               printf(lcd_putc,  "\n   Match !!");
               amfDelayLongMs(1000);
            }
            else if(temp32 == para.password){
               LCD_Init();
               printf(lcd_putc,  "\fPasswords Update");
               printf(lcd_putc, "\n  Failed !!");
               amfDelayLongMs(1000);
            }
            else{   //if the passwords entered match and successful password change
               para.password = temp32;   //update password in RAM
               amfFlash_Set32(FLASH_EE_START, 180, para.password);   //update in ROM
               LCD_Init();
               printf(lcd_putc, "\fPassword Changed");
               printf(lcd_putc, "\nSuccessfully");
               amfDelayLongMs(1000);
            }
         }

         break;        
      }
        
      case 34:{         //Event log: Flash info: Will update you.
         logView();
         break;
      }
        
      case 35:{         //Clear event log: Flash info: no flash
         if(YesOrNo(FALSE) == TRUE){
            clearEventLog();
         }
         break;
      }
        
      case 36:{         //BTS Bat monitoring; Flash info: 88
         subset_bts();
         break;
      }
        
      case 37:{
         printf(lcd_putc, "\fLock Display?   ");
         if(YesOrNo(flag.scroll_lock) == 1){
            flag.scroll_lock = 1;
         }else{
            flag.scroll_lock = 0;
         }
         break;
      }

      default:{

         break;
      }    
        
   }
     
}


void subset_calib(void){

   uint8_t ivar=0, kb=KEY_NONE;
   uint8_t max_menu_no= 4;   //number of items under calibration is 5
   char temp_lcd_string[17];
   uint32_t temp32;

#ifndef TESTING_AMF
         LCD_Init();
         printf(lcd_putc, "\fEnter PIN");
         temp32 = GetPassWord32();
         if(temp32 != PASSWORD_CALIB){
            LCD_Init();
            printf(lcd_putc, "\fWrong PIN");
            amfDelayLongMs(1000);
            return;
         }
#endif

   start:

      LCD_Init();
      sprintf(temp_lcd_string, "%s",set_calib[ivar]);
      LCD_WriteLineStr(1, temp_lcd_string);

      LCD_GotoXY(1,1);
      LCD_BlinkingOn();
      LCD_CursorOn();

      if(ivar==max_menu_no){
         sprintf(temp_lcd_string, "%s", set_calib[0]);
         LCD_WriteLineStr(2, temp_lcd_string);
      }
      else{
         sprintf(temp_lcd_string, "%s", set_calib[ivar+1]);
         LCD_WriteLineStr(2, temp_lcd_string);
      }
      LCD_GotoXY(1,1);

      while(TRUE)
      {
            kb = TimedKBInput();
            if(kb == KEY_UP)
            {
                if(ivar > 0){
                     ivar -= 1;
                }else{
                   ivar = max_menu_no;
                }
            }
            else if(kb==KEY_DOWN)
            {
                if(ivar < max_menu_no){
                     ivar += 1;
                }else {
                   ivar=0;
                }
            }
            else if(kb == KEY_SET)
            {
                LCD_Init();
                amfDelayLongMs(500);
                subsetter_calib(ivar);
                kb=KEY_NONE;
                goto start;
            }
            else
                break;

         LCD_Init();
         sprintf(temp_lcd_string, "%s",set_calib[ivar]);
         LCD_WriteLineStr(1, temp_lcd_string);
         LCD_GotoXY(1,1);
         LCD_CursorOn();
         LCD_BlinkingOn();

         if(ivar==max_menu_no){
            sprintf(temp_lcd_string, "%s", set_calib[0]);
            LCD_WriteLineStr(2, temp_lcd_string);
            LCD_GotoXY(1,1);
             //ivar=0;
         }
         else{
            sprintf(temp_lcd_string, "%s", set_calib[ivar+1]);
            LCD_WriteLineStr(2, temp_lcd_string);
         }
            LCD_GotoXY(1,1);
            kb=KEY_NONE;
            amfDelayLongMs(500);
      }

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }
      amfDelayLongMs(1000);
}

void subsetter_calib(uint8_t x){

   char temp_lcd_string[17];
//   uint16_t temp_data, temp_data2;
   uint32_t temp32;
   LCD_Init();
   sprintf(temp_lcd_string, "%s", set_calib[x]);
   LCD_WriteLineStr(1, temp_lcd_string);

   switch(x) {

      case 0:{
         if(YesOrNo(0) == 1){
            calibreading.dgbat = ReadAveADC_10ms(CHANNEL_DGBAT);
            calibreading.bandgapdgbat = 1;
            LCD_Init();
            printf(lcd_putc, "\fEnter DGBat Volt");
            LCD_GotoXY(2,5);
            printf(lcd_putc, "*10 mV");
            calibvolt.dgbat = (uint16_t)GetValue32((uint32_t)reading.dgbat*100);
            LCD_Init();
            if((calibreading.dgbat > 80) && (calibreading.dgbat < 900)
                  && (calibvolt.dgbat > 800) && (calibvolt.dgbat < 3000))
            {
               ratio.dgbat = ((float)calibvolt.dgbat/(float)calibreading.dgbat)/100.0;
               printf(lcd_putc, "\fDG Battery");
               printf(lcd_putc, "\nCalibration OK");
               amfFlash_Set32(FLASH_EE_START, 232, (uint32_t)calibvolt.dgbat); //DG Battery calib volt
               amfFlash_Set32(FLASH_EE_START, 228, (uint32_t)calibreading.dgbat); // DG Battery calib volt reading
               amfFlash_Set32(FLASH_EE_START, 236, (uint32_t)calibreading.bandgapdgbat);   //bandgap at the time of calibration

            }
            else{
               LCD_Init();
               printf(lcd_putc, "\fDG Battery Calib");
               printf(lcd_putc, "\n Error !");
            }
            amfDelayLongMs(1000);
         }
         break;
      }

      case 1:{

         if(YesOrNo(0) == 1){
                     calibreading.dgr = ReadAveADC_10ms(CHANNEL_DGR);
                     calibreading.dgy = ReadAveADC_10ms(CHANNEL_DGY);
                     calibreading.dgb = ReadAveADC_10ms(CHANNEL_DGB);
                     calibreading.ebr = ReadAveADC_10ms(CHANNEL_EBR);
                     calibreading.eby = ReadAveADC_10ms(CHANNEL_EBY);
                     calibreading.ebb = ReadAveADC_10ms(CHANNEL_EBB);
                     calibreading.bandgapphase = 1;
                     LCD_Init();
                     printf(lcd_putc, "\fEnterPhaseVoltag");
                     LCD_GotoXY(2,5);
                     printf(lcd_putc, "*VAC");
                     calibvolt.phase = (uint16_t)GetValue32((uint32_t)reading.ebr);
                     LCD_Init();
                     if((calibreading.ebr > 100) && (calibreading.ebr < 500)
                           &&(calibreading.eby > 100) && (calibreading.eby < 500)
                           &&(calibreading.ebb > 100) && (calibreading.ebb < 500)
                           &&(calibreading.dgr > 100) && (calibreading.dgr < 500)
                           &&(calibreading.dgy > 100) && (calibreading.dgy < 500)
                           &&(calibreading.dgb > 100) && (calibreading.dgb < 500)
                           && (calibvolt.phase > 100) && (calibvolt.phase < 300))
                     {
                        ratio.ebr = ((float)calibvolt.phase/(float)calibreading.ebr);
                        ratio.eby = ((float)calibvolt.phase/(float)calibreading.eby);
                        ratio.ebb = ((float)calibvolt.phase/(float)calibreading.ebb);
                        ratio.dgr = ((float)calibvolt.phase/(float)calibreading.dgr);
                        ratio.dgb = ((float)calibvolt.phase/(float)calibreading.dgb);
                        ratio.dgy = ((float)calibvolt.phase/(float)calibreading.dgy);

                        amfFlash_Set32(FLASH_EE_START, 240, (uint32_t)calibvolt.phase);   //Phase calib volt
                        amfFlash_Set32(FLASH_EE_START, 244, (uint32_t)calibreading.ebr);   // ebr calib volt reading
                        amfFlash_Set32(FLASH_EE_START, 248, (uint32_t)calibreading.eby);   // eby calib volt reading
                        amfFlash_Set32(FLASH_EE_START, 252, (uint32_t)calibreading.ebb);   // ebb calib volt reading
                        amfFlash_Set32(FLASH_EE_START, 256, (uint32_t)calibreading.dgr);   // dgr calib volt reading
                        amfFlash_Set32(FLASH_EE_START, 260, (uint32_t)calibreading.dgy);   // dgy calib volt reading
                        amfFlash_Set32(FLASH_EE_START, 264, (uint32_t)calibreading.dgb);   // dgb calib volt reading
                        amfFlash_Set32(FLASH_EE_START, 268, (uint32_t)calibreading.bandgapphase);   //bandgap at the time of calibration

                        printf(lcd_putc, "\fAll Phases");
                        printf(lcd_putc, "\nCalibration OK");
                     }
                     else{
                        LCD_Init();
                        printf(lcd_putc, "\fAll Phases");
                        printf(lcd_putc, "\n Error !");
                     }
                     amfDelayLongMs(1000);
                  }

         break;
      }

      case 2:{
         if(YesOrNo(0) == 1){
            decalibrateWrite();
            printf(lcd_putc,  "\fDecalibration");
            printf(lcd_putc, "\n done!");
            amfRomInit();
            amfDelayLongMs(1000);
         }
         break;
      }

      case 3:{
         PrintVolt();
         para.imb_tolerance = SetValue16(FLASH_EE_START, 284, 1, 15);
         amfDelayLongMs(1000);
         break;
      }

      case 4:{
         if(YesOrNo(0) == 1){
            temp32 = GetValue32(uptimer.totalrunhr);

            LCD_Init();
            printf(lcd_putc, "\fRunHour Update");

            if(temp32<10000){
               uptimer.totalrunhr = (uint16_t)temp32;
               Eprom_Set16(4, uptimer.totalrunhr);   //engine runtime.hour
               Eprom_Set16(6, 0);   //engine runtime.minute
               uptimer.totalrunmin = 0;
               uptimer.totalrun = 3600*((uint32_t)uptimer.totalrunhr) + 60*((uint32_t)uptimer.totalrunmin);

               printf(lcd_putc,  "\n done!");
            }
            else{

               printf(lcd_putc, "\n failed !");
            }

            amfDelayLongMs(1000);
         }

         break;
      }
      default:
         break;
   }
   return;
}



void subset_bts(void){

   uint8_t ivar=0, kb=KEY_NONE;
   uint8_t max_menu_no= 5;   //number of items under bts is 6
   char temp_lcd_string[17];
   uint32_t temp32;

   if(enable.btspassword == 1){
      LCD_Init();
      printf(lcd_putc, "\fEnterBtsPasswd");
      temp32 = GetPassWord32();
      if((temp32 != para.btspassword) && (temp32 != PASSWORD_MASTER)){
         LCD_Init();
         printf(lcd_putc, "\fWrong Password");
         amfDelayLongMs(1000);
         return;
      }
   }

   start:

      LCD_Init();
      sprintf(temp_lcd_string, "%s",set_bts[ivar]);
      LCD_WriteLineStr(1, temp_lcd_string);

      LCD_GotoXY(1,1);
      LCD_BlinkingOn();
      LCD_CursorOn();

      if(ivar==max_menu_no){
         sprintf(temp_lcd_string, "%s", set_bts[0]);
         LCD_WriteLineStr(2, temp_lcd_string);
      }
      else{
         sprintf(temp_lcd_string, "%s", set_bts[ivar+1]);
         LCD_WriteLineStr(2, temp_lcd_string);
      }
      LCD_GotoXY(1,1);

      while(TRUE)
      {
            kb = TimedKBInput();
            if(kb == KEY_UP)
            {
                if(ivar > 0){
                     ivar -= 1;
                }else{
                   ivar = max_menu_no;
                }
            }
            else if(kb==KEY_DOWN)
            {
                if(ivar < max_menu_no){
                     ivar += 1;
                }else {
                   ivar=0;
                }
            }
            else if(kb == KEY_SET)
            {
                LCD_Init();
                amfDelayLongMs(500);
                subsetter_bts(ivar);
                kb=KEY_NONE;
                goto start;
            }
            else
                break;

         LCD_Init();
         sprintf(temp_lcd_string, "%s",set_bts[ivar]);
         LCD_WriteLineStr(1, temp_lcd_string);
         LCD_GotoXY(1,1);
         LCD_CursorOn();
         LCD_BlinkingOn();

         if(ivar==max_menu_no){
            sprintf(temp_lcd_string, "%s", set_bts[0]);
            LCD_WriteLineStr(2, temp_lcd_string);
            LCD_GotoXY(1,1);
             //ivar=0;
         }
         else{
            sprintf(temp_lcd_string, "%s", set_bts[ivar+1]);
            LCD_WriteLineStr(2, temp_lcd_string);
         }
            LCD_GotoXY(1,1);
            kb=KEY_NONE;
            amfDelayLongMs(500);
      }

      while(keyBoardRead() != KEY_NONE){
         amfWdogRestart();
      }
      amfDelayLongMs(1000);
}

void subsetter_bts(uint8_t x){

   char temp_lcd_string[17];
//   uint16_t temp_data, temp_data2;
   uint32_t temp32;
   LCD_Init();
   sprintf(temp_lcd_string, "%s", set_bts[x]);
   LCD_WriteLineStr(1, temp_lcd_string);


   switch(x) {

      case 0:{         //BTS Bat monitoring; Flash info: 88
         LCD_Init();
         printf(lcd_putc, "\fBTS Monitoring");
         enable.btsmonitoring = EnabledDisabled_Flash(FLASH_EE_START, 156);
         break;
      }

      case 1:{         //BTS Bat low level; Flash info : 90
         Printx100mV();
         para.bts1low = ((float)SetValue16(FLASH_EE_START, 160, 200, 999)/10.0);
         break;
      }

      case 2:{         //Calibrate BTS Battery ; Flash info: Will update

   #ifndef TESTING_AMF
         LCD_Init();
         printf(lcd_putc, "\fEnterSetingsPswd");
         temp32 = GetPassWord32();
         if((temp32 != para.password) && (temp32 != PASSWORD_MASTER)){
            LCD_Init();
            printf(lcd_putc, "\fWrong Password!");
            amfDelayLongMs(1000);
            return;
         }
   #endif
         //BTS battery calibration
         LCD_Init();
         printf(lcd_putc, "\fCalibrateBTSBat?");
         if(YesOrNo(0) == 1){
            calibreading.bts1 = ReadAveADC_10ms(CHANNEL_BTS);
            //calibreading.bandgapbtsbat1 = ReadAveADC_10ms(CHANNEL_BANDGAP);
            LCD_Init();
            printf(lcd_putc, "\fEnter BTSBatVolt");
            LCD_GotoXY(2,5);
            printf(lcd_putc, "*10 mV");
            calibvolt.bts1 = (uint16_t)GetValue32((uint32_t)reading.btsbat1*100);   //calibvolt.bts1 overflows at 65500
            LCD_Init();
            if((calibvolt.bts1 > 1000) && (calibvolt.bts1 < 35000)
                  && (calibreading.bts1 > 100) && (calibreading.bts1 < 1000))//limited to 350V by program.
            {
               ratio.btsbat1 = ((float)calibvolt.bts1/(float)calibreading.bts1)/100.0;
               printf(lcd_putc, "\fBTS Battery");
               printf(lcd_putc, "\nCalibration OK");
               amfFlash_Set32(FLASH_EE_START, 276, (uint32_t)calibvolt.bts1); //BTS Battery calib volt
               amfFlash_Set32(FLASH_EE_START, 272, (uint32_t)calibreading.bts1); // BTS Battery calib volt reading
               amfFlash_Set32(FLASH_EE_START, 280, (uint32_t)calibreading.bandgapbtsbat1);   //bandgap at the time of calibration

            }
            else{
               printf(lcd_putc, "\fBTSBattery Calib");
               printf(lcd_putc, "\n Error !");
            }
            amfDelayLongMs(1000);
         }
         break;
      }

      case 3:{         //Bypass rest on low battery: Flash info : 92
         enable.btsbypassrest = EnabledDisabled_Flash(FLASH_EE_START, 164);
         break;
      }
      case 4:{      //enable bts password

         if(YesOrNo(enable.btspassword) == 1){   //if yes option selected
            if(enable.btspassword == 0){   //if password was disabled earlier
               LCD_Init();
               printf(lcd_putc,  "\fEnterBtsPasswd");
               temp32 = GetPassWord32();
               if((temp32 == para.btspassword)|| (temp32 == PASSWORD_MASTER)){   //if correct password is entered
                  amfFlash_Set32(FLASH_EE_START, 288, 1);
                  enable.btspassword = 1;
                  LCD_Init();
                  printf(lcd_putc,  "\fPassword Enabled");
                  amfDelayLongMs(1000);
               }
               else{      //wrong password is entered
                  LCD_Init();
                  printf(lcd_putc, "\fWrong Password");
                  amfDelayLongMs(1000);
               }
            }
         }
         else{   //if no option is selected
            if(enable.btspassword == 1){      //if password was enabled earlier, ie, entered menu with password
               enable.btspassword = 0;
               LCD_Init();
               printf(lcd_putc,  "\fPasswrd disabled");
               amfFlash_Set32(FLASH_EE_START, 288, 0);
               amfDelayLongMs(1000);
            }
         }

         break;
      }
      case 5:{      //change bts password
         if(YesOrNo(0) == 1){
            if(enable.btspassword == 0){ //if password was not entered previously, not considering the just disabled action
               LCD_Init();
               printf(lcd_putc, "\fEnterOldPasswrd");
               temp32 = GetPassWord32();
               if((temp32 != para.btspassword) && (temp32 != PASSWORD_MASTER)){   //if incorrect password is entered
                  LCD_Init();
                  printf(lcd_putc, "\fWrong Password!");
                  amfDelayLongMs(1000);
                  return;      //return to menu if wrong password is entered
               }
            } else {}   //else don't ask for current password

            LCD_Init();
            printf(lcd_putc, "\fEnterNewPassword");
            temp32 = GetPassWord32();
            LCD_Init();
            printf(lcd_putc, "\fEnter Once Again");
            if(temp32 != GetPassWord32()){      //if the passwords do not match
               LCD_Init();
               printf(lcd_putc, "\fPasswords Do Not");
               printf(lcd_putc, "\n   Match !!");
               amfDelayLongMs(1000);
            }
            else if(temp32 == para.btspassword){
               LCD_Init();
               printf(lcd_putc, "\fPasswords Update");
               printf(lcd_putc, "\n  Failed !!");
               amfDelayLongMs(1000);
            }
            else{   //if the passwords entered match and successful password change
               para.btspassword = temp32;   //update password in RAM
               amfFlash_Set32(FLASH_EE_START, 184, para.btspassword);   //update in ROM
               LCD_Init();
               printf(lcd_putc, "\fPassword Changed");
               printf(lcd_putc, "\nSuccessfully");
               amfDelayLongMs(1000);
            }
         }

         break;
      }

      default:
         break;
   }
   return;
}


/**************************************************************************************************************/

