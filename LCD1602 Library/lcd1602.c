/*
 * lcd1602.c
 *
 *  Created on: Nov 18, 2020
 *  	Author: Mustafa Shamsi
 */

#include "lcd1602.h"

/*Keeps track of 4 bit/8 bit data*/
static uint8_t is4BitData = 0;

/*  @brief Performs Enable signal pulse waveform for LCD1602
 *  @param Pointer to lcd base template
 */
void LCD1602_enable(LCD1602* lcd1602) {

	lcd_pin E = lcd1602->lcdPins->Enable_Pin;
	HAL_GPIO_WritePin(E.GPIOPort, E.GPIOPin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(E.GPIOPort, E.GPIOPin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(E.GPIOPort, E.GPIOPin, GPIO_PIN_SET);
}

/* @brief Writes one byte data to the LCD module. The data byte can be a instruction/character data
 * @param Pointer to lcd base template
 * @param Pointer to lcd data
 */
static void LCD1602_writeData(LCD1602 *lcd, lcd_data *data)
{
	//Get the RS pin definition
	lcd_pin RS  = lcd->lcdPins->RS;

	//Get data pins
	lcd_pin DB[8] = {0};
	for(int i =0 ;i < 8; i++)
		DB[i] = lcd->lcdPins->Data_Pins[i];

	if(!data->InstructionType)
	{
		//send command
		HAL_GPIO_WritePin(RS.GPIOPort, RS.GPIOPin, GPIO_PIN_RESET); // Command mode
	}
	else{
		//Send data
		HAL_GPIO_WritePin(RS.GPIOPort, RS.GPIOPin, GPIO_PIN_SET); // Data mode
	}

	//Write data to the LCD Module
	if(lcd->lcdCfg->lcd_dataFormat == LCD1602_4bitMode)
	{
		//Send data using 4 bit mode
		uint8_t ch = data->value;

		if(!is4BitData)
		{
			//First send the upper nibble
			HAL_GPIO_WritePin(DB[7].GPIOPort, DB[7].GPIOPin, SET_IF( (ch >> 4) & BITMASK_BIT_3) ); // 4th bit of lower nibble
			HAL_GPIO_WritePin(DB[6].GPIOPort, DB[6].GPIOPin, SET_IF( (ch >> 4) & BITMASK_BIT_2) );
			HAL_GPIO_WritePin(DB[5].GPIOPort, DB[5].GPIOPin, SET_IF( (ch >> 4) & BITMASK_BIT_1) );
			HAL_GPIO_WritePin(DB[4].GPIOPort, DB[4].GPIOPin, SET_IF( (ch >> 4) & BITMASK_BIT_0) ); // 0th bit(LSb)
			LCD1602_enable(lcd);

		}

		//Now send the lower nibble
		HAL_GPIO_WritePin(DB[7].GPIOPort, DB[7].GPIOPin, SET_IF( ch & BITMASK_BIT_3) ); // 4th bit of lower nibble
		HAL_GPIO_WritePin(DB[6].GPIOPort, DB[6].GPIOPin, SET_IF( ch & BITMASK_BIT_2) );
		HAL_GPIO_WritePin(DB[5].GPIOPort, DB[5].GPIOPin, SET_IF( ch & BITMASK_BIT_1) );
		HAL_GPIO_WritePin(DB[4].GPIOPort, DB[4].GPIOPin, SET_IF( ch & BITMASK_BIT_0) ); // 0th bit(LSb)
		LCD1602_enable(lcd);
	}
	else{
		//Send data using 8 bit mode
		uint8_t ch = data->value;

		HAL_GPIO_WritePin(DB[7].GPIOPort, DB[7].GPIOPin, SET_IF( ch  & BITMASK_BIT_7) );
		HAL_GPIO_WritePin(DB[6].GPIOPort, DB[6].GPIOPin, SET_IF( ch  & BITMASK_BIT_6) );
		HAL_GPIO_WritePin(DB[5].GPIOPort, DB[5].GPIOPin, SET_IF( ch  & BITMASK_BIT_5) );
		HAL_GPIO_WritePin(DB[4].GPIOPort, DB[4].GPIOPin, SET_IF( ch  & BITMASK_BIT_4) );
		HAL_GPIO_WritePin(DB[3].GPIOPort, DB[3].GPIOPin, SET_IF( ch  & BITMASK_BIT_3) );
		HAL_GPIO_WritePin(DB[2].GPIOPort, DB[2].GPIOPin, SET_IF( ch  & BITMASK_BIT_2) );
		HAL_GPIO_WritePin(DB[1].GPIOPort, DB[1].GPIOPin, SET_IF( ch  & BITMASK_BIT_1) );
		HAL_GPIO_WritePin(DB[0].GPIOPort, DB[0].GPIOPin, SET_IF( ch  & BITMASK_BIT_0) );
		LCD1602_enable(lcd);
	}

}

/* @brief Executes the LCD Initializatio sequence. This is required for the correct operation of the LCD module
 * @param Pointer to lcd base template
 */
static void LCD1602_Begin(LCD1602* lcd)
{
	//uint8_t cmd = 0x3;
	lcd_data dataToWrite;
	dataToWrite.InstructionType = LCD1602_InstructionMode;
	dataToWrite.value = 0x3;
	is4BitData = 1;

	// command #1, Attempting to reset LCD module
	LCD1602_writeData(lcd, &dataToWrite);
	HAL_Delay(10);

	// command #2, Attempting to reset LCD module
	LCD1602_writeData(lcd, &dataToWrite);
	HAL_Delay(1);

	// command #3, Attempting to reset LCD module
	LCD1602_writeData(lcd, &dataToWrite);
	HAL_Delay(1);

	// command #4 , to change interface mode to 4 bits
	dataToWrite.value = 0x2;
	LCD1602_writeData(lcd, &dataToWrite);
	HAL_Delay(1);

	// command #5 , set N & F (display lines and character font)
	//cmd = 0x28; N=1 , F=0; 2 lines with 5*8 font
	dataToWrite.value = 0x28;
	is4BitData = 0; //8 bit data
	LCD1602_writeData(lcd, &dataToWrite);
	HAL_Delay(5);

	//command #6 , Display ON/OFF command
	dataToWrite.value = 0x08;
	is4BitData = 0; //8 bit data
	LCD1602_writeData(lcd,&dataToWrite);
	HAL_Delay(2);

	//command #7, clear display
	dataToWrite.value = 0x01;
	LCD1602_writeData(lcd,&dataToWrite);
	HAL_Delay(10);

	//command #8, entry mode set
	dataToWrite.value = 0x06;
	LCD1602_writeData(lcd, &dataToWrite);
	HAL_Delay(5);

	//command #9 , set display and cursor
	dataToWrite.value = 0x0F;
	LCD1602_writeData(lcd,&dataToWrite);
	HAL_Delay(10);

	//Set initial values of function set command and Display control command
	lcd->lcdCfg->DisplayControl = 0x0F;
	lcd->lcdCfg->FunctionSet = 0x28;
}

/* @brief Prints a string to the LCD display
 * @param Pointer to lcd base template
 * @param Pointer to the string to print on the display
 */
static void LCD1602_printString(LCD1602 *lcd,const char *string)
{
	//Print the string on the LCD
	uint8_t index = 0;
	is4BitData = 0;

	while(1)
	{
		uint8_t ch = *(string++);
		if(ch == '\0')
			break;

		lcd_data character = {
				.InstructionType = LCD1602_DataMode,
				.value = ch
		};

		LCD1602_writeData(lcd, &character);
	}

}

/* @brief Clears the LCD's display
 * @param Pointer to lcd base template
 */
static void LCD1602_ClearDisplay(LCD1602 *lcd)
{
	lcd_data cmd = {
				.InstructionType = LCD1602_InstructionMode,
				.value = LCD_CLEARDISPLAY
			};

	is4BitData = 0;
	LCD1602_writeData(lcd,& cmd);
	HAL_Delay(2);
}

/* @brief Controls LCD display. It can be used to turn the LCD display ON/OFF without affecting the state of the data
 * 		  currently displayed on the LCD
 *
 * @param Pointer to lcd base template
 * @param Status of the LCD display. 1= TURN ON, 0= TURN OFF
 */
static void LCD1602_DiplayControl(LCD1602 *lcd, bool status)
{
	lcd_data cmd = {
						.InstructionType = LCD1602_InstructionMode,
						.value = status ? (lcd->lcdCfg->DisplayControl | LCD_DISPLAY_CONTROL_BIT):
								(lcd->lcdCfg->DisplayControl & (~LCD_DISPLAY_CONTROL_BIT))
					};

	is4BitData = 0;
	LCD1602_writeData(lcd, &cmd);
	HAL_Delay(1);
}

/* @brief Sets the cursor on the LCD display to a specific position
 * @param Pointer to lcd base template
 * @param x: Corresponds to the row number on the LCD display
 * @param y: Corresponds to the column number on the LCD display
 */
static void LCD1602_SetCursor(LCD1602 *lcd, uint8_t x, uint8_t y)
{
	uint8_t tmp;
	tmp = y & 0x0F;

	if(x == 0)
	{
		//First line, DDRAM address start from 0x00
		tmp |= 0x80;
	}
	else{
		//Second line , DDRAM starts from 0x40
		tmp |= 0xc0;
	}

	lcd_data cmd = {
				.InstructionType = LCD1602_InstructionMode,
				.value = tmp
				};

	is4BitData = 0;
	LCD1602_writeData(lcd,& cmd);
	HAL_Delay(2);

}

/* @brief Moves the cursor to Home Position (row = 0, col =0)
 * @param Pointer to lcd base template
 */
static void LCD1602_ReturnHome(LCD1602 *lcd)
{
	lcd_data cmd = {
						.InstructionType = LCD1602_InstructionMode,
						.value = LCD_RETURNHOME
				};

		is4BitData = 0;
		LCD1602_writeData(lcd, &cmd);
		HAL_Delay(2);
}

/* @brief Controls the cursor display and blink functionality
 * @param Pointer to lcd base template
 * @param status: Controls the status of the cursor, 1= SHOW CURSOR, 0= HIDE CURSOR
 */
static void LCD1602_CursorControl(LCD1602 *lcd, bool status)
{
	lcd_data cmd = {
						.InstructionType = LCD1602_InstructionMode,
						.value = status ? (lcd->lcdCfg->DisplayControl | LCD_CURSOR_CONTROL_BIT):
								(lcd->lcdCfg->DisplayControl & (~LCD_CURSOR_CONTROL_BIT))
						};

	is4BitData = 0;
	LCD1602_writeData(lcd, &cmd);
	HAL_Delay(1);

}

/* @brief Sets the Interface data length of the LCD module. The LCD1602 can operate in 8 bit/4 bit data modes
 * @param Pointer to lcd base template
 * @param Interface Length; Possible Values = LCD1602_4bitMode, LCD1602_8bitMode
 */
static void LCD1602_SetInterfaceDataLength(LCD1602* lcd, LCD1602_DataFormat InterfaceLength)
{
	lcd_data cmd = {
						.InstructionType = LCD1602_InstructionMode,
						.value = InterfaceLength == LCD1602_8bitMode ? (lcd->lcdCfg->FunctionSet | LCD_CONTROL_BIT_DL):
								(lcd->lcdCfg->FunctionSet & (~LCD_CONTROL_BIT_DL))
							};

	is4BitData = 0;
	LCD1602_writeData(lcd, &cmd);
	HAL_Delay(1);
}

/* @brief Sets the number of display lines of the LCD. LCD1602 supports single line/ double line mode
 * @param Pointer to lcd base template
 * @param Number of lines to enable in the LCD display
 */
static void LCD1602_SetDisplayLineMode(LCD1602 *lcd, LCD1602_DisplayLineMode noOfLines)
{
	lcd_data cmd = {
						.InstructionType = LCD1602_InstructionMode,
						.value = noOfLines == LCD1602_1LineMode ? (lcd->lcdCfg->FunctionSet | LCD_CONTROL_BIT_N):
								(lcd->lcdCfg->FunctionSet & (~LCD_CONTROL_BIT_N))
								};

	is4BitData = 0;
	LCD1602_writeData(lcd, &cmd);
	HAL_Delay(1);
}

/* @brief Sets the LCD display font
 * @param Pointer to lcd base template
 * @param Display font type. Supported formats are 5X8 pixels and 5X11 pixels
 */
static void LCD1602_SetDisplayFont(LCD1602 *lcd, LCD1602_DisplayFontMode displayFont)
{
	lcd_data cmd = {
						.InstructionType = LCD1602_InstructionMode,
						.value = displayFont == LCD1602_5x11Font ? (lcd->lcdCfg->FunctionSet | LCD_CONTROL_BIT_F):
								(lcd->lcdCfg->FunctionSet & (~LCD_CONTROL_BIT_F))
									};

	is4BitData = 0;
	LCD1602_writeData(lcd, &cmd);
	HAL_Delay(1);
}

/* @brief LCD Initialization function. This function is responsible for the initialization of all the function pointers
 * 		  of the lcd_baseTemplate struct. Call this function once at the start of your code. A call to this function is mandatory
 * 		  to use the various library functions
 *
 * @param Pointer to lcd base template
 */
void LCD1602_Init(LCD1602* lcd)
{
	lcd->lcd_begin = LCD1602_Begin;
	lcd->lcd_clear_display = LCD1602_ClearDisplay;
	lcd->lcd_cursor_control = LCD1602_CursorControl;
	lcd->lcd_return_home = LCD1602_ReturnHome;
	lcd->lcd_setCursor = LCD1602_SetCursor;
	lcd->lcd_setDisplay = LCD1602_DiplayControl;
	lcd->lcd_setDisplayFont = LCD1602_SetDisplayFont;
	lcd->lcd_setInterfaceDataLength = LCD1602_SetInterfaceDataLength;
	lcd->lcd_setNumberOfDisplayLines = LCD1602_SetDisplayLineMode;
	lcd->print = LCD1602_printString;
}
