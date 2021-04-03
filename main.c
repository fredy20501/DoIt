/*
 * File:   main.c
 * Author: frede
 *
 * Created on March 18, 2021, 7:32 PM
 */

#include "xc.h"
#include <stdlib.h>

// Default FRC is 8MHz, we divide by 2 to 4MHz, so Fcy is 2MHz
#define FCY 2000000UL   // Fcy (Instruction cycle frequency) in Hz (required for __delayXXX() to work)
#include <libpic30.h>
#include <p33CK256MP506.h>   // __delayXXX() function macros defined here

// Set communication channel to PGC2 and PGD2 (for debugging)
#pragma config ICS = 2

#define POTENTIOMETER_SENSITIVITY 60    // Speed that potentiometer must be moved to trigger an input (in degrees/s)
#define JOYSTICK_SENSITIVITY 5         // Threshold that joystick must be moved to trigger an input (in degrees)
#define INPUT_POLL_DELAY 100            // Delay between input polling (ms)

void initialize();
void setupADC();
int readPotentiometer();
int readJoystickX();
int readJoystickY();
int checkPotInput(value, previousValue);
int checkJoyInput();

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

// Given current and previous value returns 1 if input was detected, 0 otherwise
int checkPotInput(value, previousValue) {
    return abs((value - previousValue)>(POTENTIOMETER_SENSITIVITY/0.66));
    // ** Potentiometer Calculations **
    // Min value: 0x0005 = 5
    // Max value: 0x0FFC = 4092
    // ADC output Range: 4087
    // Angle range of potentiometer: 270 degrees
    // Change of 1 in ADC output = 0.066 degrees
}

// Read the values for the joystick axes and returns 1 if the joystick is not neutral, 0 otherwise
int checkJoyInput() {
    int joyXValue = readJoystickX();
    int joyYValue = readJoystickY();
    int detectedJoyX = abs(joyXValue - 0x800)>(JOYSTICK_SENSITIVITY/0.0122);
    int detectedJoyY = abs(joyYValue - 0x7E0)>(JOYSTICK_SENSITIVITY/0.0223);
    return detectedJoyX || detectedJoyY;
    // ** Joystick X Calculations **
    // Min value:       0x000
    // Neutral value:   0x800 to 0x80A
    // Max value:       0xFFE
    // ADC output Range: 2046 (one side)
    // Angle range of X axis: 45 degrees (one side)
    // Change of 1 in ADC output = 0.0122 degrees
    // ** Joystick Y Calculations **
    // Min value:       0x008
    // Neutral value:   0x7D6 to 0x7E3
    // Max value:       0xFFB
    // ADC output Range: 2016 (one side)
    // Angle range of Y axis: 45 degrees (one side)
    // Change of 1 in ADC output = 0.0223 degrees
}

void setupADC() {
    // Pin Configuration
    // Potentiometer:   AN12/RC0 -> AN mikrobus A
    // Joystick X:      AN13/RC1 -> MOSI mikrobus B
    // Joystick Y:      AN14/RC2 -> MISO mikrobus B
    TRISCbits.TRISC0 = 1;
    TRISCbits.TRISC1 = 1;
    TRISCbits.TRISC2 = 1;
    ANSELCbits.ANSELC0 = 1;
    ANSELCbits.ANSELC1 = 1;
    ANSELCbits.ANSELC2 = 1;
    
    // ADC Configuration
    ADCON1Hbits.FORM = 0;           // Data output format = integer (default)
    ADCON1Hbits.SHRRES = 0b11;      // Resolution = 12 bits (default)
    ADCON2Hbits.SHRSAMC = 0;        // Sampling duration in clock periods: Tadcore = 2 (default)
    ADCON3Lbits.SHRSAMP = 0;        // Set ADC core sampling to be triggered by CNVRTCH bit
    ADCON3Hbits.CLKSEL = 0;         // Use Fp (peripheral clock) [2MHz]
    ADCON5Hbits.WARMTIME = 0b0101;  // Set power up delay to 32 source clock periods (need at least 10us which is 20 periods given Fc = 2MHz)
    
    // Enable ADC Module (only enable after ADC module has been configured)
    ADCON1Lbits.ADON = 1;
    
    // Setup Shared Core
    ADCON5Lbits.SHRPWR = 1;             // Shared ADC core power enable
    while (ADCON5Lbits.SHRRDY == 0);    // Wait until shared ADC core is ready
    ADCON3Hbits.SHREN = 1;              // Enable shared ADC core
}

int readPotentiometer() {
    ADCON3Lbits.CNVCHSEL = 12;      // Set input channel
    ADCON3Lbits.CNVRTCH = 1;        // Trigger conversion (this bit gets cleared by hw on next cycle)
    while (!ADSTATLbits.AN12RDY);   // Wait until conversion result is ready
    return ADCBUF12;                // Read result
}

int readJoystickX() {
    ADCON3Lbits.CNVCHSEL = 13;      // Set input channel
    ADCON3Lbits.CNVRTCH = 1;        // Trigger conversion (this bit gets cleared by hw on next cycle)
    while (!ADSTATLbits.AN13RDY);   // Wait until conversion result is ready
    return ADCBUF13;                // Read result
}

int readJoystickY() {
    ADCON3Lbits.CNVCHSEL = 14;      // Set input channel
    ADCON3Lbits.CNVRTCH = 1;        // Trigger conversion (this bit gets cleared by hw on next cycle)
    while (!ADSTATLbits.AN14RDY);   // Wait until conversion result is ready
    return ADCBUF14;                // Read result
}