
uint8_t TimedKBInput(void)
{
	uint16_t timeout = 0;
  	
	while((keyBoardRead() == KEY_NONE) && (timeout < 3000)){
		amfWdogRestart();
		amfDelayMs(10);
		timeout++;
	} 	
	if(timeout > 2990){
		return(KEY_EXIT);
	}
	else{
		return(keyBoardRead());
	}
}


void ValuePrint16(uint16_t temp)
{ 
  	
	LCD_GotoXY(2,1);
	printf(lcd_putc, "%03lu", temp);
	LCD_GotoXY(2,3);
}

void PrintSaved(void)
{
	LCD_GotoXY(2,12);
	printf(lcd_putc, "Saved");
}

void PrintHrMinValues(uint8_t pos , uint16_t hrs, uint16_t min)
{

	LCD_GotoXY(2,1);
	printf(lcd_putc, "%02lu:%02lu", hrs, min);

	LCD_GotoXY(2, pos);
	LCD_CursorOn();
	LCD_BlinkingOn();
}


uint8_t ReadTimeInput(uint16_t* inputhr, uint16_t* inputmin)
{
	uint8_t kb, count, err_ok;
	uint16_t  temp_t = 0;
	uint8_t temp_max = 0;
	if(*inputhr > 23){
		*inputhr = 23;
	}
	if(*inputmin > 59){
		*inputmin = 59;
	} 	
  	
//	PrintHrMinValues(count, *inputhr, *inputmin);
	for(count = 2; count < 7; count += 3){
	  	
		PrintHrMinValues(count, *inputhr, *inputmin);
	  	
		if(count == 2){
			temp_max = 23;
			temp_t = *inputhr;
		}
		if(count == 5){
			temp_max = 59;
			temp_t = *inputmin;
		}				  	
		while(TRUE){

			kb = TimedKBInput();
		  	
			if(kb == KEY_UP){
				if(temp_t < temp_max){
					temp_t +=1;
				}else if(temp_t >= temp_max){
					temp_t = 0;
				}
				if(count == 2){
					PrintHrMinValues(count, temp_t, *inputmin);
				}else if(count == 5){
					PrintHrMinValues(count, *inputhr, temp_t);
				}			  	
				//WDog_Clear();
				amfDelayLongMs(250);
			}
			else if(kb == KEY_DOWN){
				if(temp_t >0){
					temp_t -=1;
				}else if(temp_t <= 0){
					temp_t = temp_max;
				}
				if(count == 2){
					PrintHrMinValues(count, temp_t, *inputmin);
				}else if(count == 5){
					PrintHrMinValues(count, *inputhr, temp_t);
				} 	
				//WDog_Clear();
				amfDelayLongMs(250);
			}
			else if((kb  == KEY_SET)||(kb == KEY_EXIT)){
				break;
			}	  	
		}
		if(kb == KEY_SET){
			if(count == 2){
				*inputhr = temp_t;
			}else if (count == 5){
				*inputmin = temp_t;
			}
			err_ok =1;
		}
		else{
			err_ok = 0;
		}
		while(keyBoardRead() != KEY_NONE){
			amfWdogRestart();
		}
		amfDelayLongMs(1000);
	 }
	LCD_BlinkingOff();
	LCD_CursorOff();
	amfDelayLongMs(1000);
	return(err_ok);
}


void PrintYesOrNo(uint8_t ynflag)
{

	LCD_GotoXY(2,1);
	if(ynflag == 1){
		printf(lcd_putc, "Yes");
	}else if(ynflag == 0){
		printf(lcd_putc, "No ");
	}
	LCD_GotoXY(2,1);
	LCD_BlinkingOn();
	LCD_CursorOn();  	
}

uint8_t YesOrNo(uint8_t ynflag_initial)
{
	uint8_t kb, ynflag;
  	
	ynflag = ynflag_initial;
  	
	PrintYesOrNo(ynflag);
  	
	while(TRUE){
		kb = TimedKBInput();
		if((kb == KEY_UP) || (kb == KEY_DOWN)){
			if(ynflag == 0){
				ynflag = 1;
			}else {
				ynflag = 0;
			}
			PrintYesOrNo(ynflag);
			while(keyBoardRead() != KEY_NONE){
				amfWdogRestart();
			}
			amfDelayLongMs(250);
		}
		else if(kb == KEY_SET){
			break;
		}
		else if(kb == KEY_EXIT){
			ynflag = ynflag_initial;
			break;
		} else {}  	
	}
  	
	while(keyBoardRead() != KEY_NONE){
		amfWdogRestart();
	}
  	
	LCD_BlinkingOff();
	LCD_CursorOff();
  	
	amfDelayLongMs(1000);
  	
	return(ynflag);
}

void PrintEnDis(uint16_t ynflag)
{

	LCD_GotoXY(2,1);
	if(ynflag == 1){
		printf(lcd_putc, "Enabled ");
	}else if(ynflag == 0){
		printf(lcd_putc, "Disabled");
	}
	LCD_GotoXY(2,1);
	LCD_BlinkingOn();
	LCD_CursorOn();  	
}

uint16_t EnabledDisabled_Flash(uint32_t AddressBase, uint16_t AddDistance)
{
	uint8_t kb;
	uint16_t enflag_initial, enflag;
  	
//	amfFlashRead16(AddDistance, &enflag_initial);

	amfFlashRead16(AddressBase, AddDistance, &enflag_initial);
  	
	enflag = enflag_initial;
  	
	PrintEnDis(enflag);
  	
	while(TRUE){
		kb = TimedKBInput();
		if((kb == KEY_UP) || (kb == KEY_DOWN)){
			if(enflag == 0){
				enflag =1;
			}else {
				enflag =0;
			}
			PrintEnDis(enflag);
			while(keyBoardRead() != KEY_NONE){
				amfWdogRestart();
			}
			amfDelayLongMs(250);
		}
		else if(kb == KEY_SET){
			amfFlash_Set32(AddressBase , AddDistance, (uint32_t)enflag);
			amfDelayLongMs(100);
			PrintSaved();
			break;
		}
		else if(kb == KEY_EXIT){
			enflag = enflag_initial;
			break;
		} else {}  	
	}
  	
	while(keyBoardRead() != KEY_NONE){
		amfWdogRestart();
	}
  	
	LCD_BlinkingOff();
	LCD_CursorOff();
  	
	amfDelayLongMs(1000);
  	
	return(enflag);
}

void convertNumberToArray5(uint32_t value32, uint8_t * array)
{

	uint8_t count;
	uint32_t divisor = 10000;
	for(count = 5; count > 0; count--){
		array[count-1] = value32/(divisor);
		value32 = value32%divisor;
		divisor = divisor/10;
	}
}



uint32_t convertArrayToNumber5(uint8_t *array)
{
	uint32_t value32;
	value32 = 10000*(uint32_t)array[4] + 1000*(uint32_t)array[3] + 100*(uint32_t)array[2] 
						+ 10*(uint32_t)array[1] + (uint32_t)array[0];
	return(value32);
}



uint32_t GetValue32(uint32_t value32_initial)
{
	uint8_t value[5] = { 0, 0, 0, 0, 0};
	char string[5] , string1;
	uint8_t count, kb;
	uint32_t value32;
  	
	if(value32_initial > 99999){
		value32_initial = 99999;
	}
	value32 = value32_initial;
  	
	convertNumberToArray5(value32, value);
	sprintf(string, "%05lu", value32);
	LCD_WriteLineStr(2, string);
	LCD_GotoXY(2,1);
	LCD_BlinkingOn();
	LCD_CursorOn();
  	
	value32 = convertArrayToNumber5(value);
  	
	for(count = 5; count > 0; count--){
		LCD_GotoXY(2, 6-count);
		amfDelayLongMs(250); amfDelayLongMs(250);
		while(TRUE){
			kb = TimedKBInput();
			LCD_GotoXY(2, 6-count);
			if(kb == KEY_UP){
				if(value[count-1] < 9){
					value[count-1] += 1;
				}
				string1 = value[count-1] + 48;
				LCD_Write(string1);
				LCD_GotoXY(2, 6-count);
				while(keyBoardRead() != KEY_NONE){
					amfWdogRestart();
				}
				amfDelayLongMs(250);
			}
			else if (kb == KEY_DOWN){
				if(value[count-1] > 0){
					value[count-1] -= 1;
				}
				string1 = value[count-1] + 48;
				LCD_Write(string1);
				LCD_GotoXY(2, 6-count);
				while(keyBoardRead() != KEY_NONE){
					amfWdogRestart();
				}
				amfDelayLongMs(250);
			}
			else if ((kb == KEY_SET)||(kb == KEY_EXIT)){
				while(keyBoardRead() != KEY_NONE){
					amfWdogRestart();
				}
				break;
			}
		} 	
	}
	if(kb == KEY_SET){
		value32 = convertArrayToNumber5(value);
	}else {
		value32 = value32_initial;
	}

	while(keyBoardRead() != KEY_NONE){
		amfWdogRestart();
	}
	amfDelayLongMs(1000);

	return(value32);
}

uint16_t GetValue16(uint16_t CurrentValue, uint16_t min, uint16_t max){
	uint8_t kb;


	if(CurrentValue>max){
		CurrentValue = max;
	} else {}
	if(CurrentValue<min){
		CurrentValue = min;
	} else {}

	ValuePrint16(CurrentValue);

	LCD_CursorOn();
	LCD_BlinkingOn();

		while(TRUE){
			kb = TimedKBInput();

			if(kb == KEY_UP){
				if(CurrentValue<max){
					CurrentValue +=1;
				}
				ValuePrint16(CurrentValue);
				//WDog_Clear();
				amfDelayLongMs(250);
			}
			else if(kb == KEY_DOWN){
				if(CurrentValue>min){
					CurrentValue -=1;
				}
				ValuePrint16(CurrentValue);
				//WDog_Clear();
				amfDelayLongMs(250);
			}
			else if(kb == KEY_SET){
				amfDelayMs(1);
				PrintSaved();
				break;
			}
			else if ((kb== KEY_EXIT)|| (kb == KEY_NONE) || (kb == KEY_RESET)){
				break;
			}
		}
		LCD_BlinkingOff();
		LCD_CursorOff();
		while(keyBoardRead() != KEY_NONE);
		amfDelayLongMs(1000);
		return(CurrentValue);
}



uint16_t SetValue16(uint32_t EEAddBase, uint16_t AddDistance, uint16_t min, uint16_t max)
{
	uint16_t CurrentValue;
	uint8_t kb;
  	

	amfFlashRead16(EEAddBase, AddDistance, &CurrentValue);
  	
	if(CurrentValue>max){
		CurrentValue = max;
	} else {}
	if(CurrentValue<min){
		CurrentValue = min;
	} else {}
  	
	ValuePrint16(CurrentValue);
	  	
	LCD_CursorOn();
	LCD_BlinkingOn();

		while(TRUE){
			kb = TimedKBInput();
		  	
			if(kb == KEY_UP){
				if(CurrentValue<max){
					CurrentValue +=1;
				}
				ValuePrint16(CurrentValue);
				//WDog_Clear();
				amfDelayLongMs(250);
			}
			else if(kb == KEY_DOWN){
				if(CurrentValue>min){
					CurrentValue -=1;
				}
				ValuePrint16(CurrentValue);
				//WDog_Clear();
				amfDelayLongMs(250);
			}
			else if(kb == KEY_SET){
				amfFlash_Set32(EEAddBase , AddDistance, (uint32_t)CurrentValue);
				amfDelayMs(1);
				PrintSaved();
				break;
			}
			else if ((kb== KEY_EXIT)|| (kb == KEY_NONE) || (kb == KEY_RESET)||(kb==KEY_SET)){
				break;
			}	  	
		}
		LCD_BlinkingOff();
		LCD_CursorOff();
		while(keyBoardRead() != KEY_NONE){
			amfWdogRestart();
		}
		amfDelayLongMs(1000);
		return(CurrentValue);  	
}




uint32_t GetPassWord32(void)
{
	uint8_t value[5] = { 0, 0, 0, 0, 0};
	char string[5] , string1;
	uint8_t count, kb;
	uint32_t value32;
  	
	uint32_t value32_initial = 0;

	value32 = value32_initial;
  	
	convertNumberToArray5(value32, value);
	sprintf(string, "%05lu", value32);
	LCD_WriteLineStr(2, string);
	LCD_GotoXY(2,1);
	LCD_BlinkingOn();
	LCD_CursorOn();
  	
	value32 = convertArrayToNumber5(value);
  	
	for(count = 5; count > 0; count--){
		LCD_GotoXY(2, 6-count);
		amfDelayLongMs(250); amfDelayLongMs(250);
		while(TRUE){
			kb = TimedKBInput();
			LCD_GotoXY(2, 6-count);
			if(kb == KEY_UP){
				if(value[count-1] < 9){
					value[count-1] += 1;
				}
				string1 = value[count-1] + 48;
				LCD_Write(string1);
				LCD_GotoXY(2, 6-count);
				while(keyBoardRead() != KEY_NONE){
					amfWdogRestart();
				}
				amfDelayLongMs(250);
			}
			else if (kb == KEY_DOWN){
				if(value[count-1] > 0){
					value[count-1] -= 1;
				}
				string1 = value[count-1] + 48;
				LCD_Write(string1);
				LCD_GotoXY(2, 6-count);
				while(keyBoardRead() != KEY_NONE){
					amfWdogRestart();
				}
				amfDelayLongMs(250);
			}
			else if ((kb == KEY_SET)||(kb == KEY_EXIT)){
				string1 = '*';
				LCD_GotoXY(2, 6-count);
				LCD_Write(string1);
				while(keyBoardRead() != KEY_NONE){
					amfWdogRestart();
				}
				break;
			}
		} 	
	}
	if(kb == KEY_SET){
		value32 = convertArrayToNumber5(value);
	}else {
		value32 = value32_initial;
	}

	while(keyBoardRead() != KEY_NONE){
		amfWdogRestart();
	}
	amfDelayLongMs(1000);

	return(value32);
}
