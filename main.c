/*
 * File: main.c
 * Author: Frederic Verret
 */

#include "xc.h"
#include <stdlib.h>

// Default FRC is 8MHz, we divide by 2 to 4MHz, so Fcy is 2MHz
#define FCY 2000000UL   // Fcy (Instruction cycle frequency) in Hz (required for __delayXXX() to work)
#include <libpic30.h>
#include <p33CK256MP506.h>   // __delayXXX() function macros defined here

#include "an_inputs.h"

// Set communication channel to PGC2 and PGD2 (for debugging)
#pragma config ICS = 2

#define INPUT_POLL_DELAY 100        // Delay between input polling (ms)

int main(void) {
    // Divide FRC by 2 using postscaler bits
    CLKDIVbits.FRCDIV = 1;
    
    setupADC();
    
    // Ouput pin for LED
    // (CS/RB2 -> potentiometer LED) 
    TRISBbits.TRISB2 = 0;
    LATBbits.LATB2 = 0;
    // (SDK/RB7 -> joystick LED) 
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB7 = 0;
    
    while (1) {
        int inputDetected = -1; // will store the type of the first input detected
        
        int potPreviousValue = readPotentiometer();
        int potValue = 0;
        while(1) {
            // == CHECK POTENTIOMETER ==
            potValue = readPotentiometer();
            if (checkPotInput(potValue, potPreviousValue)) {
                inputDetected = 0;
                LATBbits.LATB2 = 1; // turn on led
                // Wait until potentiometer stops turning
                while (checkPotInput(potValue, potPreviousValue)) {
                    potPreviousValue = potValue;
                    __delay_ms(INPUT_POLL_DELAY);
                    potValue = readPotentiometer();
                }
                LATBbits.LATB2 = 0; // turn off led
                break;
            }
            potPreviousValue = potValue;
            
            // == CHECK JOYSTICK ==
            if (checkJoyInput()) {
                inputDetected = 1;
                LATBbits.LATB7 = 1; // turn on led
                // Wait until joystick returns to neutral
                while (checkJoyInput());
                LATBbits.LATB7 = 0; // turn off led
                break;
            }

            // Check inputs only every so often
            __delay_ms(INPUT_POLL_DELAY);
        }

        __delay_ms(1000);
    }
}