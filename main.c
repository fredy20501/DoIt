/*
 * File:   main.c
 * Author: frede
 *
 * Created on March 18, 2021, 7:32 PM
 */

// Default FRC is 8MHz, we divide by 2 to 4MHz, so Fcy is 2MHz
#define FCY 2000000UL   // Fcy (Instruction cycle frequency) in Hz (required for __delayXXX() to work)
#include <libpic30.h>   // __delayXXX() function macros defined here

#include "xc.h"
#include "LCD.h"

// Set communication channel to PGC2 and PGD2 (for debugging)
#pragma config ICS = 2

int main(void) {
    // Initialize LCD
    LCDInit();

    // LCD testing
    
    LCDSetMessage("Button = Start");
    __delay_ms(3000);
    LCDClearMessage();

    int score = 0;
    while (1) {
        LCDSetScore(score++);

        LCDSetMessage("Flick it!");
        __delay_ms(500);
        LCDClearMessage();
        __delay_ms(500);
        LCDSetMessage("Press it!");
        __delay_ms(500);
        LCDClearMessage();
        __delay_ms(500);
        LCDSetMessage("Twist it!");
        __delay_ms(500);
        LCDClearMessage();
        __delay_ms(500);

        if (score%5 == 0) {
            LCDSetMessage("Game over!");
            __delay_ms(500);
            LCDClearMessage();
            __delay_ms(500);
        }
    }
}