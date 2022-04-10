#INT_TIMER0				//1 second
void  TIMER0_isr(void) 
{
	//flag.counterbig++;
	set_timer0(get_timer0()+2800);
	clear_interrupt(INT_TIMER0);
  	
	if(status.dgon)
		uptimer.totalrun++;

	if(status.dgon){
		uptimer.dguptime++;
		uptimer.dgdowntime = 0;
	}
	else{
		uptimer.dgdowntime++;
		uptimer.dguptime = 0;
	}
  	

  	
	if(downtimer.crank > 0){
		downtimer.crank --;
	}
	else if(process_on.crank == 1){
		CRANK_OFF();
	}
	else{
//		CRANK_OFF();
	}

	if(downtimer.stop > 0){
		downtimer.stop --;
	}
	else if(process_on.stop == 1){
	  FUEL_SOL_OFF();
	}

	if(downtimer.cont_delay > 0)
		downtimer.cont_delay--;

	if(status.semiauto == 1){
		MANUAL_LED_TOGGLE();
	}  	
  	
	if(downtimer.mainsfail > 0)
		downtimer.mainsfail--;

	if(downtimer.hooter > 0){
		downtimer.hooter--;
	}
	else if(process_on.hooter == 1){
	  BUZZER_OFF();
	}

	if(downtimer.enginestartup > 0)
		downtimer.enginestartup--;

	if(downtimer.recool >0)
		downtimer.recool --;

	if(downtimer.ebrestore > 0)
		downtimer.ebrestore --;

	if(downtimer.trip >0 )
		downtimer.trip--;

	if(downtimer.lfl_delay > 0)
		downtimer.lfl_delay--;

	if(downtimer.fault_delay > 0)
		downtimer.fault_delay--;

	if(downtimer.llop_delay > 0)
		downtimer.llop_delay--;

	if(downtimer.kb_delay > 0)
		downtimer.kb_delay--;

	if(downtimer.warning_delay >0)
		downtimer.warning_delay--;
  	
}

#INT_TIMER1
void  TIMER1_isr(void) 
{
	clear_interrupt(INT_TIMER1);
  	
	if(flag.discard_freq == 0){
		counter.freq = 65000;
	}
	else{
		flag.discard_freq = 0;
	}
}

#INT_CCP2
void CCP2_Isr(void){

	clear_interrupt(INT_TIMER1);

	if(flag.discard_freq == 0){
		set_timer1(0);
		counter.freq =  CCP_2;//get_timer1();
	}
	else{
		flag.discard_freq = 0;
	} 

}

#INT_TIMER2
void  TIMER2_isr(void) 
{
	clear_interrupt(INT_TIMER2);
	//flag.counterbig2++;

}

#INT_TIMER3		//for clearing keyboard interrupt, and reenabling.
void  TIMER3_isr(void) 
{

	//flag.counterbig3++;
  	
  clear_interrupt(INT_TIMER3);
  if(flag.counter4 > 7){
	  flag.counter4 = 0;
	  flag.discard_kb = 0;
	  flag.displaystat =1;
  	
  }
  else{
	flag.counter4++;
  }
  
  if((flag.scroll_lock == 1) || (flag.scroll_lock_temp == 1)){
		if(flag.displaypos > flag.max_display_items){
			flag.displaypos = flag.max_display_items;
		}
  }
  
  if(flag.counter32 > 63){
  
		///8 sec interrupt
		if((flag.scroll_lock == 0) && (flag.scroll_lock_temp == 0)){
			if(flag.displaypos >= flag.max_display_items)
			{
				flag.displaypos = 0;
			}else{
				flag.displaypos++;
			}
		}
	  	
		flag.log_enable = 1;
		//end of 8 sec interrupt
	  	
		flag.counter32 = 0;
  }
  else{
		flag.counter32++;
  }
}

#INT_RB
void  RB_isr(void) 
{
   
	uint8_t temp8;
  	
	clear_interrupt(INT_RB);

	if(flag.discard_kb != 1){		//discard immediate interrupt chaining

		temp8 = keyBoardRead_inside_int();

		if(temp8 == KEY_DOWN)
		{
			BOARD_IND_TOGGLE();
		  	
			if(flag.displaypos >= flag.max_display_items)
			{
					flag.displaypos = 0;
			}else{
				flag.displaypos++;
			}

			while(keyBoardRead_inside_int() != KEY_NONE){
				amfWdogRestart();
			}

			if(flag.scroll_lock_temp == 1)
			{
				flag.scroll_lock_temp = 0;
				flag.release_disp_lock[flag.hold_disp_lock] = 1;
			}
		  	
			flag.discard_kb = 1;
			flag.displaystat = 1;
			flag.counter4 = 4;
			flag.counter32 = 0;

		}
		else if(temp8 == KEY_UP)
		{
			BOARD_IND_TOGGLE();
			if(flag.displaypos < 1)
			{
				flag.displaypos = flag.max_display_items;
			}else{
				flag.displaypos--;
			}

			while(keyBoardRead_inside_int() != KEY_NONE){
				amfWdogRestart();
			}

			if(flag.scroll_lock_temp == 1)
			{
				flag.scroll_lock_temp = 0;
				flag.release_disp_lock[flag.hold_disp_lock] = 1;
			}
		  	
			flag.discard_kb = 1;
			flag.displaystat = 1;
			flag.counter4 = 4;
			flag.counter32 = 0;
		}
	}
	else{

	}
}

//#INT_RDA
void  RDA_isr(void) 
{
	clear_interrupt(INT_RDA);

}

//#INT_TBE
void  TBE_isr(void) 
{
	clear_interrupt(INT_TBE);

}

#ifdef pic_4620
	#INT_LOWVOLT
#endif
#ifdef pic_k22
	#INT_HLVD
#endif
void  LOWVOLT_isr(void) 
{
	#byte HLVDCON = 0xFD2;
	#ifdef pic_4620
		clear_interrupt(INT_LOWVOLT);
	#endif
	#ifdef pic_k22
		clear_interrupt(INT_HLVD);
	#endif
  	
	if( (HLVDCON & (0x80) ) == 0x80)	//ie, if voltage rises above set point
	{
		#ifdef pic_4620
			setup_low_volt_detect(LVD_TRIGGER_BELOW| LVD_42);
		#endif
		#ifdef pic_k22
			setup_low_volt_detect(LVD_TRIGGER_BELOW| LVD_41);
		#endif
	  	
		flag.lowvcc = 0;
	}
	else {
  	
		#ifdef pic_4620
			setup_low_volt_detect(LVD_TRIGGER_ABOVE | LVD_45);
		#endif
		#ifdef pic_k22
			setup_low_volt_detect(LVD_TRIGGER_ABOVE | LVD_43);
		#endif
	  	
		flag.lowvcc = 1;
	}
}

