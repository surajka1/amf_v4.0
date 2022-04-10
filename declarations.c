///////////////////////////SIMCONS CONTROL SYSTEMS AMF//////////////////////////
////COPYRIGHT: SIMCONS CONTROL SYSTMES//////////////////////////////////////////
//No part of this code may be printed or copied ////////////////////////////////

///////////////////////////////declarations.c///////////////////////////////////
//The file includes declarations////////////////////////////////////////////////

#define NO_FAULT   0
#define SOFT_FAULT   1
#define HARD_FAULT   2

#define STOP_FAIL   1
#define CRANK_SUCCESS   1
#define AUTO_START_SUCCESS 1
#define AUTO_START_FAIL   0
#define EB_IS_BACK   2

#define AUTO_NONE   0
#define AUTO_EB 1
#define AUTO_DG   2
#define AUTO_FAULT   3
#define MAN_DGON   4
#define MAN_DG_ONLOAD   5
#define MAN_EB_ONLOAD 6

#define EBFAIL 1
#define EBOK 0

#define PROCESSOK 1
#define PROCESSING 2
#define PROCESSFAIL 3
#define DEFAULT 0

#define TASK_NEW 1
#define TASK_DEFAULT 2
#define TASK_KILL 3

#define AUTO_MODE 0
#define MANUAL_MODE 1

#define ENABLED 1
#define DISABLED 0

#ifndef ON
#define ON (1)
#endif

#ifndef OFF
#define OFF (0)
#endif
//!///////////////////////sw  VARIABLES/////////////////////////////////

#define DG_V_THRESHOLD 50
#define BTS_V_THRESHOLD 10

#define DG_THRESHOLD_VOLT	60
#define LLOP_BYPASS_DELAY	60
#define FAULT_TOL_DELAY		3
#define RESET_DELAY		3

#define PASSWORD_MASTER 12098
#define PASSWORD_CALIB	77368

///////////////////////// Other Variables ///////////////////////////////////////
#define IMBALANCE_TOLERANCE 3

extern struct tm amftime;

extern struct readings{
   uint16_t dgr, dgy, dgb, ebr, eby, ebb;
   float dgbat, btsbat1, btsbat2, btsbat3, frequency;
   uint16_t bandgap;
}   reading;   

extern struct calibReadings{      //separate both structures, as unused variables in both
   uint16_t dgbat, ebr, eby, ebb, dgr, dgy, dgb, bts1, phase, bandgapdgbat, bandgapphase, bandgapbtsbat1;
}calibvolt, calibreading;

extern struct flagstatus{
   uint8_t ebfail, dgon, buildingup, llop;
   uint8_t dghealthy, ebhealthy;
   volatile uint8_t engine_running, engine_stopped;
   uint8_t ebrfail, ebyfail, ebbfail, ebimbalance;
   uint8_t dg_resting, cycleon;
   uint8_t mode, mode_rom, remote_mode, remote_start, semiauto, remote_started;
   uint8_t ebfail_timedout;
   uint8_t recooled, dg_cont_delay_ok, m_cont_delay_ok;
   uint8_t dg_ok_to_load;
   uint8_t scheduler_ok, schedule_cycle;
   volatile uint8_t cycleon_timeout, cycleoff_timeout, llop_timeout;
   uint8_t bypassstart;
   uint8_t hooter_done;
   uint8_t flashWriteComplete;
   uint8_t auto_scheduled_off;
}   status;

extern struct amf_running_status{
   uint32_t auto_none, fault, total_fault, auto_dg, auto_eb, auto_scheduled_off;
   uint32_t man_no_contact, man_dg_contact, man_eb_contact, man_dgon, man_ebfail;
}  ;

extern struct warningflags{
   uint8_t lowbat, highbat, lowfuel, bts1low, common;
}   warning;

extern struct tempwarningflags{
   uint8_t lowbat, highbat, bts1low;
}tempwarning;

extern struct faultflags{
   uint8_t llop, ht, overload, emstop, flt1, lowfuel;
   uint8_t stop, start, overspeed, underspeed, dgundervolt, dgovervolt;
   uint8_t external , common, alternator;
   uint8_t errcode01, errcode02, errcode03, errcode04;
   uint8_t rom_error_segment;
}   fault;

extern struct tempfaults{
   uint8_t llop, ht, lowfuel, flt1;
   uint8_t overload, overspeed, underspeed, dgundervolt, dgovervolt;
}tempfault;
   
extern struct enableparameters{
   uint16_t ebsinglephase, ebimbalance, dgsinglephase, llopcheck, manmode, scheduler, fuelsol;
   uint16_t cycletimer,   password, btspassword, btsmonitoring, btsbypassrest;
}   enable;

extern struct prog_parameters{
   uint16_t cycleonhr, cycleonmin, cycleoffhr, cycleoffmin;
   uint16_t schedule_starthr, schedule_startmin, schedule_stophr, schedule_stopmin;
   uint16_t mfdelay_low, mfdelay_hr, mfdelay_min;
   uint16_t ebovervolt, ebundervolt_h, ebundervolt_l, dgovervolt, dgundervolt, imbalance, imb_tolerance;
   uint16_t max_crank_no, rpmperhz;
   float dgbatlow, dgbathigh;
   float bts1low;
   float lowfreq, highfreq;
   uint32_t password, btspassword;
}   para;

extern struct calibrationratios{
   float dgr, dgy, dgb, ebr, eby, ebb, dgbat, btsbat1, bandgapdgbat, bandgapphase,bandgapbts1;
}   ratio;

extern struct prog_delays{
   uint32_t mainsfail;
   uint16_t enginestartup, recool, crank, stop, hooter, ebrestore;
   uint32_t cycleon, cycleoff;
   uint16_t trip, cont_delay, lfl_delay;
}prog_delay;

extern struct down_timers{
   volatile uint32_t mainsfail;
   volatile uint16_t crank, stop, hooter, enginestartup, recool, ebrestore, trip;
   volatile uint16_t cont_delay, lfl_delay, fault_delay, llop_delay, warning_delay;
   volatile uint8_t kb_delay;
}   downtimer;

extern struct up_timers{
   volatile uint32_t dguptime, dgdowntime;
   volatile uint32_t totalrun;
   uint16_t totalrunhr, totalrunmin;
}   uptimer;

extern struct time_stamps{
   uint16_t scheduled_start, scheduled_stop;
}timestamp;

//extern struct password{
//   uint8_t passwdbyte1, passwdbyte2, passwdbyte3, passwdbyte4, passwdbyte5;
//}   calib, menu, bts;

extern struct keyboard{
   uint8_t keypress;
}value;

extern struct counters{
   volatile uint16_t freq;
}counter;

extern struct globalflags{
   volatile uint8_t displaystat, displaypos;
   uint8_t ee_write_ok, max_display_items, scroll_lock;
   volatile uint8_t scroll_lock_temp, release_disp_lock[10], hold_disp_lock;
   volatile uint8_t discard_freq, discard_kb;
   volatile uint8_t log_enable;
   volatile uint8_t lowvcc;
   volatile uint8_t counter8, counter4, counter32;
//   volatile uint16_t counterbig, counterbig1,  counterbig2, counterbig3;
}flag;

extern struct displaycontent{
   uint8_t display[22];
}content;

extern struct strings{
   char lcd_data[2][25];   //size 25 to counter lengthy sprintf writes.
}AMFString;

extern struct processon{
   volatile uint8_t mainsfail, crank, stop, hooter, enginestartup, recool, ebrestore, trip, cont_delay;
   uint8_t ebonload, dgonload, totalfail;
   uint8_t lfl_delay, fault_delay, llop_delay, warning_delay;

}process_on;

extern struct counterdown{
   uint8_t start_attempts, stop_attempts;
}downcounter;




struct readings reading;   

struct flagstatus status;

struct tm amftime;
//struct amf_running_status amf_status;

struct warningflags warning;

struct tempwarningflags tempwarning;

struct faultflags fault;

struct tempfaults tempfault;

   
struct enableparameters enable;

struct prog_parameters para;

struct calibrationratios ratio;

struct prog_delays prog_delay;

struct down_timers downtimer;

struct up_timers uptimer;

struct time_stamps timestamp;

struct keyboard value;

struct counters counter;

struct globalflags flag;

struct strings AMFString;

struct processon process_on;

struct counterdown downcounter;

struct calibReadings calibvolt, calibreading;

struct displaycontent content;




