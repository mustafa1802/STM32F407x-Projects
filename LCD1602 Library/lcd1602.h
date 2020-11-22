#ifndef LCD1602_H
#define LCD1602_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define SET_IF(expr)  ((expr) ? GPIO_PIN_SET : GPIO_PIN_RESET)

#define BITMASK_BIT_7	0x80
#define BITMASK_BIT_6	0x40
#define BITMASK_BIT_5	0x20
#define BITMASK_BIT_4	0x10
#define BITMASK_BIT_3	0x08
#define BITMASK_BIT_2	0x04
#define BITMASK_BIT_1	0x02
#define BITMASK_BIT_0	0x01

#define LCD_CLEARDISPLAY	0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

/*Function set bitmasks*/
#define LCD_DISPLAY_CONTROL_BIT			0x04
#define LCD_CURSOR_CONTROL_BIT			0x03
#define LCD_CONTROL_BIT_DL				0x10
#define LCD_CONTROL_BIT_F				0x04
#define LCD_CONTROL_BIT_N				0x08

typedef enum {
	LCD1602_InstructionMode = 0,
	LCD1602_DataMode
} LCD1602_CommandMode;

typedef enum {
	LCD1602_4bitMode = 0,
	LCD1602_8bitMode
} LCD1602_DataFormat;

typedef enum {
	LCD1602_1LineMode = 0,
	LCD1602_2LineMode
} LCD1602_DisplayLineMode;

typedef enum {
	LCD1602_5x8Font = 0,
	LCD1602_5x11Font
} LCD1602_DisplayFontMode;

typedef struct lcd1602_PinDescription{
	GPIO_TypeDef* GPIOPort;
	uint16_t GPIOPin;
} lcd_pin;

typedef struct lcd1602_PinDefinition{
	lcd_pin RS;
	lcd_pin Enable_Pin;
	lcd_pin Data_Pins[8];
} lcd_pinout;

typedef struct lcd1602_ConfigurationData{
	LCD1602_DataFormat lcd_dataFormat;
	uint8_t FunctionSet;
	uint8_t DisplayControl;
} lcd_config;

typedef struct lcd1602_data{
	LCD1602_CommandMode InstructionType;
	uint8_t value;
} lcd_data;

typedef struct lcd1602_BaseTemplate{

	//LCD parameters
	lcd_pinout *lcdPins;
	lcd_config *lcdCfg;

	//List of functions exposed from the driver

	void (*lcd_begin)(struct lcd1602_BaseTemplate* lcd);
	void (*print)(struct lcd1602_BaseTemplate *lcd,const char *string);

	void (*lcd_clear_display)(struct lcd1602_BaseTemplate *lcd);
	void (*lcd_setDisplay)(struct lcd1602_BaseTemplate *lcd , bool status);

	void (*lcd_return_home)(struct lcd1602_BaseTemplate *lcd);
	void (*lcd_setCursor)(struct lcd1602_BaseTemplate *lcd, uint8_t x, uint8_t y);

	void (*lcd_cursor_control)(struct lcd1602_BaseTemplate *lcd, bool status);
	void (*lcd_setInterfaceDataLength)(struct lcd1602_BaseTemplate *lcd, LCD1602_DataFormat InterfaceLength);
	void (*lcd_setNumberOfDisplayLines)(struct lcd1602_BaseTemplate *lcd, LCD1602_DisplayLineMode noOfLines);
	void (*lcd_setDisplayFont)(struct lcd1602_BaseTemplate *lcd, LCD1602_DisplayFontMode displayFont);

} LCD1602;


void LCD1602_Init(LCD1602* lcd);

#endif //LCD_1602_H
