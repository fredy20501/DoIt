/*
 * File:   LCD.c
 * Author: frede
 *
 * Created on March 18, 2021, 9:13 PM
 */

// This is a guard condition so that contents of this file are not included more than once.  
#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdio.h>

// LCD module connections
#define LCD_RS LATDbits.LATD3
#define LCD_EN LATDbits.LATD4

#include "LCD_Lib.h"

/* === Pins Used ===
 * LCD_RS: SCL_1 | RD3
 * LCD_EN: SDA_1 | RD4
 */

void setupLCD() {
    // Initialize pins (set to outputs)
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    // Initialize LCD
    LCD_Begin();
}

// Show given message on first line of LCD
// Clear the display is msg is empty string
void LCDSetMessage(char* msg) {
    LCD_Goto(1,1);
    LCD_Print(msg);
}

// Clear the first line of the LCD
void LCDClearMessage() {
    LCD_Goto(1,1);
    LCD_Print("                ");
}

// Show given score on second line of LCD
void LCDSetScore(int score) {
    LCD_Goto(1,2);
    
    char buffer[20];
    sprintf(buffer, "Score: %i", score);
    
    LCD_Print(buffer);
}

#endif	/* LCD_H */