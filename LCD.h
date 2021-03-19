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

// Function prototypes
void LCDSetMessage(char* msg);
void LCDSetScore(int score);

// Show given message on top line of LCD
// Clear the display is msg is empty string
void LCDSetMessage(char* msg) {
    // Todo (Fred)
}

// Show given score on bottom line of LCD
// Clear the display if score is <0
void LCDSetScore(int score) {
    // Todo (Fred)
    
    // Score: XX
}

#endif	/* LCD_H */