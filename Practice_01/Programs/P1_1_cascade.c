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
 * Practice 1 - One-shot LED cascade on button press.
 * MCU: PIC16F887 @ 4 MHz (XT)
 *
 * Author: Daniel Valenzuela Corella
 * Project: Practice_1
 * File: P1_1_cascade.c
 * Created on August 8, 2023
 *
 * I/O:
 *   RD0  -> Push button input
 *   RB0...RB3 -> LEDs (through 220�470 O to GND)
 *
 * Behavior:
 *   Whenever the button event is detected, run a one-shot cascade:
 *   RB3 -> RB2 -> RB1 -> RB0, each LED ON for 250 ms, then OFF.
 *
 * Notes:
 *   - Ensure pins are configured as DIGITAL I/O (disable analog on PORTA/PORTB):
 *       ANSEL = 0x00; ANSELH = 0x00;
 *   - If the button is wired ACTIVE-LOW (pull-up to VDD, press to GND),
 *     the input test should be `if (PORTD.RD0 == 0)` after a small debounce.
 */

void main() {
    bool flag = false;          // Latches a single "run the cascade" request.

    // Digital configuration recommended.
    ANSEL  = 0x00;              // All AN0...AN7 as digital.
    ANSELH = 0x00;              // All AN8...AN13 as digital.

    TRISD0_bit = 1;             // RD0 as INPUT (push button).
    PORTD = 0x00;               // Clear PORTD latches.

    TRISB = 0x00;               // PORTB as OUTPUT (LEDs).
    PORTB = 0x00;               // All LEDs OFF.

    while (1) {
        // As written below, it triggers when RD0 reads HIGH (active-HIGH).
        if (PORTD.RD0) {                 // Button event detected (active-HIGH version).
            flag = true;                 // Arm one-shot cascade.
            Delay_ms(10);                // optional: small debounce (Very recommended).
            while (PORTD.RD0) { }        // wait for release.
        }

        // One-shot LED cascade (runs to completion once 'flag' is set)
        while (flag) {
            PORTB.RB3 = 1;      // LED on RB3 ON.
            Delay_ms(250);
            PORTB.RB3 = 0;      // LED on RB3 OFF.

            PORTB.RB2 = 1;      // LED on RB2 ON.
            Delay_ms(250);
            PORTB.RB2 = 0;      // LED on RB2 OFF.

            PORTB.RB1 = 1;      // LED on RB1 ON.
            Delay_ms(250);
            PORTB.RB1 = 0;      // LED on RB1 OFF.

            PORTB.RB0 = 1;      // LED on RB0 ON.
            Delay_ms(250);
            PORTB.RB0 = 0;      // LED on RB0 OFF.

            flag = false;       // Clear latch -> one-shot complete
        }
    }
}