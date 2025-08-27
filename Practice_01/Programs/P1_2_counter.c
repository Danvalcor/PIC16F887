// -- Device configuration bits (PIC16F887) --------------------------------------
#pragma config FOSC = XT        // External crystal/resonator, 4 MHz (XT mode)
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = ON       // Power-up Timer enabled
#pragma config MCLRE = ON       // MCLR pin enabled (use 10 kO pull-up to VDD)
#pragma config CP = OFF         // Code protection disabled
#pragma config CPD = OFF        // Data memory code protection disabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config IESO = ON        // Internal/External Switchover enabled
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor enabled
#pragma config LVP = OFF        // Low-Voltage Programming disabled (use high-voltage programming)

#include <stdbool.h>

/*
 * Practice 1 - 4-bit counter on button press
 * MCU: PIC16F887 @ 4 MHz (XT)
 *
 * Author: Daniel Valenzuela Corella
 * Project: Practice_1
 * File: P1_2_counter.c
 * Created on August 8, 2023
 *
 * I/O:
 *   RD0  -> Push button (assumed ACTIVE-LOW: pull-up to VDD, press to GND)
 *   RB0..RB3 -> LEDs with 220�470 O to GND (display the counter value)
 *
 */

// --- Helper: return true once per valid press (debounced, active-LOW) -------
static bool button_pressed(void) {
    if (PORTD.RD0 == 0) {       // pressed when line is LOW
        Delay_ms(20);           // debounce settle
        if (PORTD.RD0 == 0) {
            while (PORTD.RD0 == 0) { /* wait for release */ }
            Delay_ms(10);       // release debounce
            return true;
        }
    }
    return false;
}

void main(void) {
    unsigned char count = 0;    // 4-bit counter 0..15

    // Ensure digital I/O (PORTA/PORTB default to analog on some pins)
    ANSEL  = 0x00;              // AN0..AN7 as digital
    ANSELH = 0x00;              // AN8..AN13 as digital

    TRISD0_bit = 1;             // RD0 as input (button)
    PORTD = 0x00;

    TRISB = 0x00;               // PORTB as output (LEDs)
    PORTB = 0x00;               // all LEDs OFF at start

    while (1) {
        // Show the counter on RB0..RB3 (preserve upper nibble RB4..RB7)
        PORTB = (PORTB & 0xF0) | (count & 0x0F);

        // Increment on each valid press, wrap at 15 -> 0
        if (button_pressed()) {
            count = (count + 1) & 0x0F;
        }
    }
}
