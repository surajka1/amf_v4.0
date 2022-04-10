#define LCD_Init()	lcd_init()



#define amfDelayMs(x)	delay_ms(x)

#define amfWdogRestart()	restart_wdt()

#define LCD_CursorOn()	lcd_send_byte(0, LCD_DISP_ON_CURSOR)

#define LCD_BlinkingOn()	lcd_send_byte(0, LCD_DISP_ON_BLINK)

#define LCD_BlinkingOff()	lcd_send_byte(0, LCD_DISP_ON)

#define LCD_CursorOff()	lcd_send_byte(0, LCD_DISP_ON)
