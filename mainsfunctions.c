
uint8_t ebIsConsistant(uint8_t task_type)
{
	if((status.ebhealthy == 0) && (process_on.ebrestore == 0)){
		downtimer.ebrestore = prog_delay.ebrestore;
		process_on.ebrestore = 1;
	}
	else if(task_type == TASK_KILL){
		downtimer.ebrestore = 0;
	}

	if(downtimer.ebrestore > 0){
		if(status.ebfail == EBFAIL){
			process_on.ebrestore = 0;
			status.ebhealthy =0;
			return(PROCESSFAIL);
		}
		else {
			return(PROCESSING);
		}
	}
	else{
		process_on.ebrestore = 0;
		if(status.ebfail == EBOK){
			status.ebhealthy = 1;
			status.ebfail_timedout = 0;
			return(PROCESSOK);
		}
		else{
			status.ebhealthy = 0;
			return(PROCESSFAIL);
		}
	}

}

uint8_t processContactorDelay(uint8_t task_type)
{
	uint8_t temp8_eb, temp8_dg;
	temp8_dg = STATUS_DG_CONTACT();
	temp8_eb = STATUS_EB_CONTACT();

	if( ((( (temp8_dg == 0) && (status.m_cont_delay_ok == 0) )
			|| ((temp8_eb == 0) && (status.dg_cont_delay_ok == 0))) && (process_on.cont_delay == 0))
			|| (task_type == TASK_NEW))
	{
		process_on.cont_delay = 1;
		downtimer.cont_delay = prog_delay.cont_delay;
		if((process_on.crank == 0) && (process_on.stop == 0)){
			crankTimerClear();
		}
	}
	else if(task_type == TASK_KILL){
		downtimer.cont_delay = 0;
	}

	if(temp8_dg == 1){
		status.m_cont_delay_ok = 0;
	}
	if(temp8_eb == 1){
		status.dg_cont_delay_ok = 0;
	}

	if(downtimer.cont_delay > 0){
		return(PROCESSING);
	}
	else {

		if(process_on.cont_delay == 1){
			process_on.cont_delay = 0;

			if(temp8_eb == 0){
				status.dg_cont_delay_ok = 1;
			}
			if(temp8_dg == 0){
				status.m_cont_delay_ok = 1;
			}
		}

		return(PROCESSOK);
	}

}

uint8_t processMainsFail(uint8_t task_type)
{

	if((status.ebfail_timedout == 0)&&(status.ebhealthy == 0)&&(process_on.mainsfail == 0)){
		downtimer.mainsfail = prog_delay.mainsfail;
		process_on.mainsfail = 1;
	}
	else if(task_type == TASK_KILL){
		downtimer.mainsfail =0;
	}

	if(downtimer.mainsfail > 0){		// following should be reviewed

		if((status.ebfail == EBOK)){	//if mains returns
			if(status.ebhealthy == 1){	//if mains is consistent
				process_on.mainsfail = 0;
				return(PROCESSFAIL);
			}
			else{		//mains returns and not consistent
				return(PROCESSING);
			}
		}
		else {		//if eb is still fail
			if((enable.btsmonitoring == 1) && (warning.bts1low == 1)){
				process_on.mainsfail = 0;
				status.ebfail_timedout = 1;
				return(PROCESSOK);
			}
			else
				return(PROCESSING);
		}
	}
	else{							//down counter is done and mains fail is confirmed
		if(status.ebhealthy == 1){
			process_on.mainsfail = 0;
			return(PROCESSFAIL);
		}
		else{
			process_on.mainsfail = 0;
			status.ebfail_timedout = 1;
			return(PROCESSOK);
		}

	}
  	
}



