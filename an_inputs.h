/* 
 * File:   an_inputs.h
 * Author: Uwera Ntaganzwa, Frederic Verret
 */
 
#ifndef XC_AN_INPUTS_H
#define	XC_AN_INPUTS_H

#include <xc.h>

#define POTENTIOMETER_SENSITIVITY 60    // Speed that potentiometer must be moved to trigger an input (in degrees/s)
#define JOYSTICK_SENSITIVITY 5          // Threshold that joystick must be moved to trigger an input (in degrees)
#define INPUT_POLL_DELAY 100            // Delay between input polling (ms)

void setupADC() {
    /* === Pins Used ===
    * Potentiometer:    AN_1    | RC0/AN12
    * Joystick X:       MOSI_1  | RB9/AN11
    * Joystick Y:       MISO_1  | RB8/AN10
    */
    TRISCbits.TRISC0 = 1;
    TRISBbits.TRISB9 = 1;
    TRISBbits.TRISB8 = 1;
    ANSELCbits.ANSELC0 = 1;
    ANSELBbits.ANSELB9 = 1;
    ANSELBbits.ANSELB8 = 1;
    
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
    ADCON3Lbits.CNVCHSEL = 11;      // Set input channel
    ADCON3Lbits.CNVRTCH = 1;        // Trigger conversion (this bit gets cleared by hw on next cycle)
    while (!ADSTATLbits.AN11RDY);   // Wait until conversion result is ready
    return ADCBUF11;                // Read result
}

int readJoystickY() {
    ADCON3Lbits.CNVCHSEL = 10;      // Set input channel
    ADCON3Lbits.CNVRTCH = 1;        // Trigger conversion (this bit gets cleared by hw on next cycle)
    while (!ADSTATLbits.AN10RDY);   // Wait until conversion result is ready
    return ADCBUF10;                // Read result
}

// Given current and previous value returns 1 if input was detected, 0 otherwise
int checkPotInput(value, previousValue) {
    return abs((value - previousValue))>(POTENTIOMETER_SENSITIVITY/0.66);
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

#endif	/* XC_AN_INPUTS_H */

