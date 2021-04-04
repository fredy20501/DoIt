/* 
 * File: speaker.h
 * Author: Carter MacLennan
 */

#ifndef XC_SPEAKER_H
#define	XC_SPEAKER_H

#include <xc.h>

void setupSpeaker (void) {
     /* AUXILIARY PLL
     *   AFPLLO = AFPLLI * [M / (N1 * N2 * N3)]
     *   AFPLLO = 8MHz * [125/(1 * 2 * 1)]
     *   AFPLLO = 500MHz 
     */
    ACLKCON1bits.FRCSEL = 1;        // clock source = 8MHz internal FRC 
    APLLFBD1bits.APLLFBDIV = 125;   // M = 125
    ACLKCON1bits.APLLPRE = 1;       // N1 = 1
    APLLDIV1bits.APOST1DIV = 2;     // N2 = 2
    APLLDIV1bits.APOST2DIV = 1;     // N3 = 1
    ACLKCON1bits.APLLEN = 1;        // AFPLLO is connected to the APLL post-divider output  
    
    // DAC CONFIGURATION 
    DACCTRL1Lbits.CLKSEL = 2;   // FDAC = AFPLLO "Auxillary PLL out"
    DACCTRL1Lbits.DACON = 1;    // Enables DAC modules
    DAC1CONLbits.DACEN = 1;     // Enables DACx Module
    DAC1CONLbits.DACOEN = 1;    // Connects DACx to the DACOUT1 pin  
    
    //TRIANGLE WAVE MODE
    SLP1DATbits.SLPDAT = 0x1;       // Slope rate, counts per step 
    SLP1CONHbits.TWME = 1;          // Enable Triangle Mode for DACx
    SLP1CONHbits.SLOPEN = 1;        // Triangle mode requires SLOPEN to be set to '1'
    
    // AMPLIFIER
    TRISDbits.TRISD15 = 0;   // output from dspic to amplifier 
}

// Turn on the speaker for the given duration then off for the delay (in ms)
void enableSpeaker(int duration, int delay) {
    LATDbits.LATD15 = 1;    // Enable Amplifier 
    __delay_ms(duration);
    LATDbits.LATD15 = 0;    // Disable Amplifier 
    __delay_ms(delay);
}

// Play sound (0=fail, 1=success, 2=beep)
void playSound(int type) {
    DAC1DATHbits.DACDAT = 0xF00;        // 3840 * (AVdd = 3.3V)/4095 = 3.09
    if (type == 2) {
        // high-pitched tone
        DAC1DATLbits.DACLOW = 0x4FF;    // 1279 * (AVdd = 3.3V)/4095 = 1.03
        
        // 1 quick beeps
        enableSpeaker(100, 50);
    }
    if (type == 1) {
        // high-pitched tone
        DAC1DATLbits.DACLOW = 0x4FF;    // 1279 * (AVdd = 3.3V)/4095 = 1.03
        
        // 2 quick beeps
        enableSpeaker(100, 50);
        enableSpeaker(100, 50);
    }
    else {
        // low-pitched tone
        DAC1DATLbits.DACLOW = 0x000;    // 0 * (AVdd = 3.3V)/4095 = 0
        
        // 3 short beeps and 1 long beep
        enableSpeaker(250, 100);
        enableSpeaker(250, 100);
        enableSpeaker(250, 100);
        enableSpeaker(750, 100);
    }
}

#endif	/* XC_SPEAKER_H */

