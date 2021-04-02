/*
 * File: LCD_serial.h
 * Author: Frederic Verret
 */

#ifndef LCD_SERIAL_H
#define	LCD_SERIAL_H

#include <xc.h>

// Shift register connections
#define SR_SER LATCbits.LATC8
#define SR_RCLK LATCbits.LATC9
#define SR_SRCLK LATBbits.LATB13

/* === Pins Used ===
 * SR_SER:   SDA_2 | RC8
 * SR_RCLK:  SCL_2 | RC9
 * SR_SRCLK: TX_2  | RB13
 */

void SRInit() {
    // Set pins to output
    TRISCbits.TRISC8 = 0;
    TRISCbits.TRISC9 = 0;
    TRISBbits.TRISB13 = 0;
}

// Clock 4 bits of data into the register
void SRSendData(uint8_t data) {
    int i;
    for (i=0; i<4; i++) {
        SR_SER = (data >> i) & 0x01;
        
        SR_SRCLK = 0;
        __delay_us(1);
        SR_SRCLK = 1;
        __delay_us(1);
        SR_SRCLK = 0;
        
        SR_RCLK = 0;
        __delay_us(1);
        SR_RCLK = 1;
        __delay_us(1);
        SR_RCLK = 0;
    }
}

#endif	/* LCD_SERIAL_H */

