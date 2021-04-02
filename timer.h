/*
 * File:   timer.c
 * Author: frede
 *
 * Created on March 19, 2021, 9:19 AM
 */

// This is a guard condition so that contents of this file are not included more than once.  
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h> // include processor files - each processor file is guarded.

// Function prototypes
int isTimerCompleted();
void setupTimer();
void enableTimerInterrupt();
void stopTimer();
void startTimer();


// Global variable indicating whether timer has completed
int timerCompleted = 0;

int isTimerCompleted() {
    return timerCompleted;
}

void setupTimer() {
    // Note: By default timer uses internal peripheral clock source (Fp), where Fp=Fcy
    stopTimer();
    T1CONbits.TCKPS = 3; // Set prescaler to 1:256
}

void enableTimerInterrupt() {
    IFS0bits.T1IF = 0;  // Clear Interrupt flag bit
    IEC0bits.T1IE = 1;  // Interrupt enable for Timer 1
}

void stopTimer() {
    T1CONbits.TON = 0;  // Stop timer
    TMR1 = 0x0;         // Clear timer register
    PR1 = 0x9896;       // Load period register (39,062 -> 5 seconds given Fp=2MHz and 1:256 prescaler)
}

void startTimer() {
    T1CONbits.TON = 1;  // Start timer
    timerCompleted = 0;
}

// Timer 1 ISR
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void)
{
    stopTimer();
    timerCompleted = 1; // Set global variable to indicate timer has completed
    IFS0bits.T1IF = 0;  // Clear Timer 1 interrupt flag bit
}


#endif	/* TIMER_H */