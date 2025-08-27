# Practice 01 — Digital I/O 

**Topic:** Push‑button input + LED outputs. Two programs are included:

* **P1\_1 – LED Cascade (one‑shot)**
* **P1\_2 – 4‑bit Counter on PORTB (debounced)**

This README explains what each program does, required components, wiring, configuration bits, how to build/run, and how to validate results in simulation.

---

## Folder layout (this practice)

```
Practice_01/
├─ Media/                     # Images & runtime GIFs
│  ├─ Practice01_Schematic_Drawing.png
│  ├─ P1_1_runtime.gif
│  └─ P1_2_runtime.gif
├─ Programs/                  # Source + HEX
│  ├─ P1_1_cascade.c
│  ├─ P1_2_counter.c
│  ├─ P1_1_cascade.hex
│  └─ P1_2_counter.hex
└─ Simulations/
   └─ Practice01_Proteus.pdsprj
```
---

## Hardware & components (Bill of Materials)

* **MCU:** PIC16F887 (DIP‑40)
* **Clock:** 4 MHz crystal **+ 22 pF** capacitors ×2 (XT/HS mode)
* **Supply:** 5 V; **MCLR** with **10 kΩ** pull‑up to VDD
* **Button:** 1 × momentary push‑button on **RD0**
* **Resistors:** 10 kΩ (pull‑up/down for button as wired), **220 Ω ×4** (LED series)
* **LEDs:** 4 × LEDs on **RB0..RB3**
* (Recommended) **100 nF** decoupling capacitor close to MCU VDD–VSS

> Note on button polarity: the code assumes **active‑LOW** (press → RD0 = 0) unless stated otherwise. If your wiring is active‑HIGH, invert the conditions as documented below.
**Schematic:**

![Practice 01 Schematic](./Practice_01/Media/Practice01_Schematic_Drawing.png)
---

## Pin mapping

| Function | Pin          | Notes                                                                      |
| -------- | ------------ | -------------------------------------------------------------------------- |
| Button   | RD0          | Active‑LOW with 10 kΩ pull‑up to VDD (or active‑HIGH with 10 kΩ pull‑down) |
| LEDs     | RB0..RB3     | Each LED to GND through 220 Ω                                              |
| OSC      | OSC1/OSC2    | 4 MHz crystal + 22 pF to GND                                               |
| MCLR     | RE3/MCLR/VPP | 10 kΩ to VDD                                                               |

---

## Configuration bits (common)

```c
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
```

Also force **digital I/O**:

```c
ANSEL  = 0x00;   // AN0..AN7 digital
ANSELH = 0x00;   // AN8..AN13 digital
```

---

## Program A — P1\_1 LED Cascade (one‑shot)

 - **Goal:** When the button is pressed, run a **single** cascade:
`RB3 → RB2 → RB1 → RB0`, each LED ON for **250 ms**, then OFF.

 - **Algorithm:** Poll RD0. On a valid press, set a `flag` and (optionally) wait for release (simple debounce). While `flag` is set, execute the sequence and then clear the `flag` (one‑shot).

**Active‑LOW input (recommended).**

```c
if (PORTD.RD0 == 0) {   // pressed (LOW)
    Delay_ms(10);       // debounce
    while (PORTD.RD0 == 0) { /* wait release */ }
    flag = true;
}
```

 - **Expected result:** One full cascade per press. No repeated runs while the button remains held.

**Demo:** 
---
![P1\_1 runtime](./Practice_01/Media/P1_1_runtime.gif)

---

## Program B — P1\_2 4‑bit Counter (debounced)

 - **Goal:** Count **0..15** on each **press** and show the value on `RB0..RB3`. After 15, wrap to 0.

 - **Algorithm:** Use a small debounce routine that detects **press → release**. On each valid press, increment an 8‑bit variable and mask its low nibble to display on PORTB.

**Core snippet.**

```c
static bit button_pressed(void) {
    if (PORTD.RD0 == 0) {           // active‑LOW
        Delay_ms(20);
        if (PORTD.RD0 == 0) {
            while (PORTD.RD0 == 0) {}
            Delay_ms(10);
            return 1;
        }
    }
    return 0;
}

unsigned char count = 0;
PORTB = (PORTB & 0xF0) | (count & 0x0F);
if (button_pressed()) {
    count = (count + 1) & 0x0F;
}
```

 - **Expected result:** Each press increments **exactly once**; no spurious counts on bounce; rollover 15→0 works.

**Demo:**
---
![P1\_2 runtime](./Practice_01/Media/P1_2_runtime.gif)


---

## How to build & run

### mikroC PRO for PIC

1. Open the project or create a new one for **PIC16F887** @ **4 MHz XT**.
2. Add `P1_1_cascade.c` or `P1_2_counter.c` from `/Programs/`.
3. Compile → **.hex** generated in the output folder.
4. Program your board (ICSP/PICkit) **or** load the HEX in the simulator.

### MPLAB X + XC8 (optional ports)

Use the same logic with `__delay_ms()` and `#define _XTAL_FREQ 4000000UL`. Keep config bits consistent.

---

## Simulation (Proteus)

1. Open `Simulations/Practice01_Proteus.pdsprj`.
2. Ensure **Clock Frequency = 4.0000 MHz** and/or a **4 MHz crystal** with **22 pF** caps.
3. Load the matching **.hex** file from `/Programs/` into the MCU.
4. Run ▶ and verify behavior. Use the virtual oscilloscope if you want to confirm 250 ms steps.

---

## Validation checklist

* ✅ `ANSEL/ANSELH = 0x00` (digital I/O)
* ✅ MCLR has 10 kΩ pull‑up; button wired as assumed by the code
* ✅ LEDs on RB0..RB3 light as expected
* ✅ Cascade shows **\~250 ms** per LED (±10 %)
* ✅ Counter increments once per press and wraps 15→0

---

## Troubleshooting

* **No LED activity:** check VDD/VSS, TRISB = 0x00, and that you wrote to `PORTB` not `TRISB`.
* **Wrong timing:** verify 4 MHz clock in Proteus and crystal value.
* **Button “jumps” counts:** increase debounce (e.g., 20–30 ms) and ensure you wait for release.
* **Pins act analog:** set `ANSEL/ANSELH` to 0.

---

## What I learned

* Difference between **polling** and **edge‑based** (press→release) detection.
* Why **debounce** is required and simple ways to implement it.
* How to map a 4‑bit value directly to a port and keep the upper nibble unchanged.
* How oscillator selection affects delays and simulation timing.

---

***Author**: Daniel Valenzuela Corella · **Course**: Microprocessors Lab · Practice 01 — Digital I/O*
