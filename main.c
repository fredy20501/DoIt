/*
 * File:   main.c
 * Author: Frederic Verret
 */

#include "xc.h"
#include <stdlib.h>

// Default FRC is 8MHz, we divide by 2 to 4MHz, so Fcy is 2MHz
#define FCY 2000000UL   // Fcy (Instruction cycle frequency) in Hz (required for __delayXXX() to work)
#include <libpic30.h>   // __delayXXX() function macros defined here

// Import other files
#include "LCD.h"
#include "timer.h"
#include "speaker.h"
#include "an_inputs.h"

// Set communication channel to PGC2 and PGD2 (for debugging)
#pragma config ICS = 2

// Global static variables
#define MAX_INSTRUCTIONS 128    // Size of array storing instruction sequence
#define INSTRUCTION_DELAY 500   // Delay between displaying instructions on LCD (ms))
#define ROUND_DELAY 500        // Delay between rounds (ms)
#define GAME_OVER_DELAY 3000    // Delay when lose a game (ms)
#define BUTTON PORTCbits.RC7

// Function prototypes
void initialize();
void waitForButton();
char generateNewInstruction();
void showSequence(char* instructions, int size);
int listenForSequence(char* instructions, int size);

int main(void) {
    initialize();
    
    // Wait for user to press button to start game
    waitForButton();
    
    // Initialize RNG using current timer value 
    // (timer value will depend on when user first presses the button which is random)
    srand(TMR1);
    stopTimer();
    
    // Enable timer interrupt now that RNG is initialized
    enableTimerInterrupt();
    
    // Array storing instruction sequence
    char instructions[MAX_INSTRUCTIONS] = {0};
    int size = 0;
    LCDSetScore(size);
    
    // Main program loop
    while(1) {
        
        // Generate new instruction
        instructions[size++] = generateNewInstruction();
        
        // Display sequence to user
        showSequence(instructions, size);
        
        // Wait for user to input the sequence
        int result = listenForSequence(instructions, size);
        
        // Force game to end if reached max instruction number (prevent array overflow)
        if (size>=MAX_INSTRUCTIONS) result = 0;
        
        if (result == 0) { // Game over
            // Show game over message
            LCDSetMessage("Game over!");
            
            // Play fail sound
            playSound(0);
            
            // Reset instructions
            int i;
            for (i=0; i<size; i++) instructions[i] = 0;
            size = 0;
            
            // Wait before overriding game over message
            __delay_ms(GAME_OVER_DELAY);
            
            // Wait for user to press button to start new game
            waitForButton();
            
            // Update score (i.e. reset to 0)
            LCDSetScore(size);
        }
        else {
            // Update score
            LCDSetScore(size);
            
            // Play success sound
            playSound(1);
            
            // Round completed successfully, wait before next round
            __delay_ms(ROUND_DELAY);
        }
    }
}

void initialize() {
    // Divide FRC by 2 using postscaler bits
    CLKDIVbits.FRCDIV = 1;
    
    // Button pin: RST_1 | RC7  (Digital input w/ pull-up resistor)
    TRISCbits.TRISC7 = 1;
    ANSELCbits.ANSELC7 = 0;
    CNPUCbits.CNPUC7 = 1;
    
    setupLCD();
    setupSpeaker();
    setupADC();
    setupTimer();
  
    // Start timer (will be used for random seed generation)
    startTimer();
}

// Wait until button is pressed
void waitForButton() {
    // Display idle message on LCD
    LCDSetMessage("Button = Start");
    
    while (BUTTON == 1); // Wait for button to be pressed
    while (BUTTON == 0); // Wait for button to be released

    LCDClearMessage();
}

// Generate a random instruction (0=joystick, 1=button, 2=potentiometer)
char generateNewInstruction() {
    return rand()%3;
}

// Loop through instructions and set LCD message to show the sequence to replicate
void showSequence(char* instructions, int size) {
    int i;
    for (i=0; i<size; i++) {
        __delay_ms(INSTRUCTION_DELAY);
        switch(instructions[i]) {
            case 0:
                LCDSetMessage("Flick it!");
                break;
            case 1:
                LCDSetMessage("Press it!");
                break;
            case 2:
                LCDSetMessage("Twist it!");
                break;
        }
        __delay_ms(INSTRUCTION_DELAY);
        LCDClearMessage();
    }
}


// Wait for inputs in the given sequence
// Returns 0 if failed, 1 if successful
int listenForSequence(char* instructions, int size) {
    // Loop through instructions and wait for each input
    int i;
    for (i=0; i<size; i++) {
        stopTimer();
        startTimer();
        
        // Store first input that was detected
        // (0=joystick, 1=button, 2=potentiometer)
        char inputDetected = -1;
        int potPreviousValue = readPotentiometer();
        int potValue = 0;
        while(!isTimerCompleted()) {
            // == CHECK BUTTON ==
            if (BUTTON == 0) {
                inputDetected = 1;
                playSound(2);
                // Wait until button is released
                while (BUTTON == 0);
                break;
            }
            
            // == CHECK JOYSTICK ==
            if (checkJoyInput()) {
                inputDetected = 0;
                playSound(2);
                // Wait until joystick returns to neutral
                while (checkJoyInput());
                break;
            }
            
            // == CHECK POTENTIOMETER ==
            potValue = readPotentiometer();
            if (checkPotInput(potValue, potPreviousValue)) {
                inputDetected = 2;
                playSound(2);
                // Wait until potentiometer stops turning
                while (checkPotInput(potValue, potPreviousValue)) {
                    potPreviousValue = potValue;
                    __delay_ms(INPUT_POLL_DELAY);
                    potValue = readPotentiometer();
                }
                break;
            }
            potPreviousValue = potValue;
            
            // Check inputs only every so often
            // (this affects the speed the potentiometer must be turned)
            __delay_ms(INPUT_POLL_DELAY);
        }
        
        // Check if timer ran out or button press was wrong
        if (isTimerCompleted() || inputDetected != instructions[i]) {
            return 0; // Game over
        }
        
        // Small delay to prevent detecting 1 input as multiple inputs (i.e. manual debouncing)
        __delay_ms(50);
    }
    return 1;
}
