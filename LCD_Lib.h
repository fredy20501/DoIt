/*******************************************************************************
 * LCD_Lib.c                                                                   *
 * MPLAB XC8 compiler LCD driver for LCDs with HD44780 compliant controllers.  *
 * https://simple-circuit.com/                                                 *
 *                                                                             *
 ******************************************************************************/
// Modified by Frederic Verret

#ifndef LCD_LIB_H
#define	LCD_LIB_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "LCD_serial.h"

#define LCD_FIRST_ROW          0x80
#define LCD_SECOND_ROW         0xC0
#define LCD_CLEAR              0x01
#define LCD_RETURN_HOME        0x02
#define LCD_ENTRY_MODE_SET     0x04
#define LCD_CURSOR_OFF         0x0C
#define LCD_UNDERLINE_ON       0x0E
#define LCD_BLINK_CURSOR_ON    0x0F
#define LCD_MOVE_CURSOR_LEFT   0x10
#define LCD_MOVE_CURSOR_RIGHT  0x14
#define LCD_TURN_ON            0x0C
#define LCD_TURN_OFF           0x08
#define LCD_SHIFT_LEFT         0x18
#define LCD_SHIFT_RIGHT        0x1E

#ifndef LCD_TYPE
   #define LCD_TYPE 2           // 0=5x7, 1=5x10, 2=2 lines
#endif

char RS;

void LCD_Write_Nibble(uint8_t n);
void LCD_Cmd(uint8_t Command);
void LCD_Goto(uint8_t col, uint8_t row);
void LCD_PutC(char LCD_Char);
void LCD_Print(char* LCD_Str);
void LCD_Begin();

void LCD_Write_Nibble(uint8_t n)
{
    LCD_RS = RS;
    SRSendData(n);

    // send enable pulse
    LCD_EN = 0;
    __delay_us(1);
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
}

// Send a command to the LCD
void LCD_Cmd(uint8_t Command)
{
    RS = 0;
    LCD_Write_Nibble(Command >> 4);
    LCD_Write_Nibble(Command);
    if((Command == LCD_CLEAR) || (Command == LCD_RETURN_HOME))
        __delay_ms(2);
}

// Set the write position on the LCD
// Note: upper left is 1,1; second row first position is 1,2
void LCD_Goto(uint8_t col, uint8_t row)
{
    switch(row)
    {
        case 2:
            LCD_Cmd(LCD_SECOND_ROW + col - 1);
            break;
        break;
        default: // case 1:
            LCD_Cmd(LCD_FIRST_ROW + col - 1);
    }

}

// Prints a character (LCD_Char) on the LCD
void LCD_PutC(char LCD_Char)
{
    RS = 1;
    LCD_Write_Nibble(LCD_Char >> 4);
    LCD_Write_Nibble(LCD_Char );
}

// Prints a string (LCD_Str) on the LCD
void LCD_Print(char* LCD_Str)
{
    uint8_t i = 0;
    RS = 1;
    while(LCD_Str[i] != '\0')
    {
        LCD_Write_Nibble(LCD_Str[i] >> 4);
        LCD_Write_Nibble(LCD_Str[i++] );
    }
}

// Initialize LCD module
// Must be called once before any function
void LCD_Begin()
{
    SRInit();
    
    RS = 0;
    LCD_RS = 0;
    LCD_EN = 0;
    SRSendData(0x0);

    __delay_ms(40);
    LCD_Cmd(3);
    __delay_ms(5);
    LCD_Cmd(3);
    __delay_ms(5);
    LCD_Cmd(3);
    __delay_ms(5);
    LCD_Cmd(LCD_RETURN_HOME);
    __delay_ms(5);
    LCD_Cmd(0x20 | (LCD_TYPE << 2));
    __delay_ms(50);
    LCD_Cmd(LCD_TURN_ON);
    __delay_ms(50);
    LCD_Cmd(LCD_CLEAR);
    __delay_ms(50);
    LCD_Cmd(LCD_ENTRY_MODE_SET | LCD_RETURN_HOME);
    __delay_ms(50);
}

#endif	/* LCD_LIB_H */

