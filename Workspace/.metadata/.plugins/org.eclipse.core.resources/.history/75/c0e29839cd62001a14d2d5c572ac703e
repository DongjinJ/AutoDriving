/*
 * LCD_HD44780_I2C.c
 *
 *	The MIT License.
 *  Created on: 23.06.2018
 *      Author: Mateusz Salamon
 *      www.msalamon.pl
 *
 *      Website: https://msalamon.pl/czy-konwerter-i2c-z-lcd-to-dobrana-para/
 *      GitHub:  https://github.com/lamik/LCD_I2C_STM32_HAL
 *
 */
#include "main.h"
#include "i2c.h"
#include "stdio.h"

#include "LCD_HD44780_I2C.h"

//
//	Expander pins definition
//
#define RS_BIT_MASK 0x01
#define RW_BIT_MASK 0x02
#define EN_BIT_MASK 0x04
#define BL_BIT_MASK 0x08
#define D4_BIT_MASK 0x10
#define D5_BIT_MASK 0x20
#define D6_BIT_MASK 0x40
#define D7_BIT_MASK 0x80

#define LCD_I2C_TIMEOUT 10

I2C_HandleTypeDef *hi2c_lcd;
uint8_t LcdAddress;
uint8_t ByteToExpander;
/*
 *
 * 	INSIDE FUNCTIONS
 *
 */

//
//	Send/Read data to/from expander function
//
void LCD_SendDataToExpander(uint8_t *Data)
{
	HAL_I2C_Master_Transmit(hi2c_lcd, LCD_I2C_ADDRESS, Data, 1, LCD_I2C_TIMEOUT);
}

//
//	Set data port
//
static inline void LCD_SetDataPort(uint8_t Data)
{
	ByteToExpander &= ~(0xF0); // Clear Data bits

	if(Data & (1<<0))
		ByteToExpander |= D4_BIT_MASK;

	if(Data & (1<<1))
		ByteToExpander |= D5_BIT_MASK;

	if(Data & (1<<2))
		ByteToExpander |= D6_BIT_MASK;

	if(Data & (1<<3))
		ByteToExpander |= D7_BIT_MASK;

	LCD_SendDataToExpander(&ByteToExpander);
}
//
//	Control signals
//
static inline void LCD_SetRS(void)
{
	ByteToExpander |= RS_BIT_MASK;
	LCD_SendDataToExpander(&ByteToExpander);
}

static inline void LCD_ClearRS(void)
{
	ByteToExpander &= ~(RS_BIT_MASK);
	LCD_SendDataToExpander(&ByteToExpander);
}

static inline void LCD_SetEN(void)
{
	ByteToExpander |= EN_BIT_MASK;
	LCD_SendDataToExpander(&ByteToExpander);
}

static inline void LCD_ClearEN(void)
{
	ByteToExpander &= ~(EN_BIT_MASK);
	LCD_SendDataToExpander(&ByteToExpander);
}

static inline void LCD_SetRW(void)
{
	ByteToExpander |= RW_BIT_MASK;
	LCD_SendDataToExpander(&ByteToExpander);
}

static inline void LCD_ClearRW(void)
{
	ByteToExpander &= ~(RW_BIT_MASK);
	LCD_SendDataToExpander(&ByteToExpander);
}

void LCD_BacklightOff(void)
{
	ByteToExpander &= ~(BL_BIT_MASK);
	LCD_SendDataToExpander(&ByteToExpander);
}

void LCD_BacklightOn(void)
{
	ByteToExpander |= BL_BIT_MASK;
	LCD_SendDataToExpander(&ByteToExpander);
}

//
//	Write byte to LCD
//
void LCD_WriteByte(uint8_t data)
{

	LCD_SetEN();
	LCD_SetDataPort(data >> 4);
	LCD_ClearEN();
//	Delay_us(120);
	LCD_SetEN();
	LCD_SetDataPort(data);
	LCD_ClearEN();

	Delay_us(120); // Wait for data processing
}

//
//	Write command to LCD
//
void LCD_WriteCmd(uint8_t cmd)
{
	LCD_ClearRS();
	LCD_WriteByte(cmd);
	Delay_us(1000); //<<--- wait for command processing
}

//
//	Write data to LCD
//
void LCD_WriteData(uint8_t data)
{
	LCD_SetRS();
	LCD_WriteByte(data);
}

/*
 *
 * 	USER FUNCTIONS
 *
 */

//
//	Write one character to LCD
//
void LCD_Char(char c)
{
	LCD_WriteData(((c >= 0x80) && (c <= 0x87)) ? (c & 0x07) : c);
}

//
//	Write string to LCD
//
void LCD_String(char* str)
{
	char c;
	while((c = *(str++)))
		LCD_Char(c);
}

//
// Print integer on LCD
//
void LCD_Int(int value)
{
	char buf[LCD_X+1];
	sprintf(buf, "%d", value);
	LCD_String(buf);
}

//
// Print hexadecimal on LCD
//
void LCD_Hex(int value, uint8_t upper_case)
{
	char buf[LCD_X+1];
	if(upper_case)
		sprintf(buf, "%X", value);
	else
		sprintf(buf, "%x", value);
	LCD_String(buf);
}

//
// Define custom char in LCD CGRAM in 'number' slot
//
void LCD_DefChar(uint8_t number, uint8_t *def_char)
{
	uint8_t i, c;
	LCD_WriteCmd(64+((number&0x7)*8));
	for(i = 0; i < 8; i++)
	{
		c = *(def_char++);
		LCD_WriteData(c);
	}
}

//
// Back to home position
//
void LCD_Home()
{
	LCD_WriteCmd(LCDC_CLS|LCDC_HOME);
}

//
// Control cursor visibility
//
void LCD_Cursor(uint8_t on_off)
{
	if(on_off == 0)
		LCD_WriteCmd(LCDC_ONOFF|LCDC_DISPLAYON);
	else
		LCD_WriteCmd(LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

//
// Control cursor blinking
//
void LCD_Blink(uint8_t on_off)
{
	if(on_off == 0)
		LCD_WriteCmd(LCDC_ONOFF|LCDC_DISPLAYON);
	else
		LCD_WriteCmd(LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

//
// Set cursor for x-column, y-row
//
void LCD_Locate(uint8_t x, uint8_t y)
{
	switch(y)
	{
		case 0:
			y = LCD_LINE1;
			break;
#if (LCD_Y>1)
		case 1:
			y = LCD_LINE2;
			break;
#endif
#if (LCD_Y>2)
		case 2:
			y = LCD_LINE3;
			break;
#endif
#if (LCD_Y>3)
		case 3:
			y = LCD_LINE4;
			break;
#endif
	}

	LCD_WriteCmd((0x80 + y + x));
}

//
//	Clear LCD
//
void LCD_Cls(void)
{
	LCD_WriteCmd(LCDC_CLS);
}

//
//	Initialization
//
void LCD_Init(I2C_HandleTypeDef *hi2c)
{
	hi2c_lcd = hi2c;

	LCD_BacklightOn();
	LCD_ClearRS();
	LCD_ClearEN();
	LCD_ClearRW();

	Delay_us(15);

	LCD_SetDataPort(LCDC_FUNC|LCDC_FUNC8B);
	Delay_us(4100);
	LCD_SetDataPort(LCDC_FUNC|LCDC_FUNC8B);
	Delay_us(100);
	LCD_SetDataPort(LCDC_FUNC|LCDC_FUNC4B); //4-byte mode
	Delay_us(100);
	LCD_WriteCmd(LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7); // 4-bit, 2 lanes, 5x7 chars

	LCD_WriteCmd(LCDC_ONOFF|LCDC_CURSOROFF); // Cursor off
	LCD_WriteCmd(LCDC_ONOFF|LCDC_DISPLAYON); // LCD on
	LCD_WriteCmd(LCDC_ENTRY|LCDC_ENTRYR); // Data entry right

	LCD_Cls(); // Clear display
}
