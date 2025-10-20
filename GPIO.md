# GPIO (General Purpose Input/Output)

The first practical module! GPIO is the most fundamental concept in microcontrollers. It's how the chip interacts with the physical world—turning on LEDs, reading button presses, and sending signals to other components.

My goal here is to master controlling these pins directly by reading and writing to their registers.


---

## The 3 Key GPIO Registers

To control any pin, I need to work with three 8-bit registers. The `x` in the name stands for the port letter (`B`, `C`, or `D`).

### 1. `DDRx` – Data Direction Register
* **Purpose:** This is the most important register. It sets the *direction* of each pin in the port (either INPUT or OUTPUT).
* This is exactly like the `TRIS` register I used with PIC microcontrollers.
* **Rule:** Each bit in this register controls the corresponding pin.
    * `1` = The pin is an **OUTPUT**.
    * `0` = The pin is an **INPUT** (this is the default state on power-up).

### 2. `PORTx` – Port Data Register
* **Purpose:** This register has two different jobs, which depend on the pin's direction (set by `DDRx`).
* **Job 1 (If pin is an OUTPUT):** `PORTx` *writes* the value.
    * `1` = Drive the pin **HIGH** (e.g., 5V).
    * `0` = Drive the pin **LOW** (e.g., 0V).
* **Job 2 (If pin is an INPUT):** `PORTx` controls the *internal pull-up resistor*.
    * `1` = **Enable** the internal pull-up resistor. (This is great for buttons!)
    * `0` = **Disable** the pull-up (the pin is "floating").

### 3. `PINx` – Port Input Pins Register
* **Purpose:** This is a **read-only** register. Its only job is to *read* the current digital state of a pin.
* **Rule:** If I read this register, it tells me if the physical pins are currently HIGH (`1`) or LOW (`0`). This is what I'll use to check if a button is pressed.

---

## The Core Skill: Bit Manipulation

I can't just write `DDRB = 1;` because that would set `PB0` but also clear the other 7 pins. The key is to change *only* the single bit I care about. This is done with bitwise operators (`|`, `&`, `~`).

### Method 1: The Individual Bit Method (Recommended)

This is for setting or clearing one bit at a time.

#### To Set a Bit (Set to `1`)
* **Use-Case:** Make a pin an **OUTPUT** (`DDRx`) or set an output pin **HIGH** (`PORTx`).
* **Operator:** Bitwise **OR (`|`)**.
* **Example:** Set `PB0` (Pin 0 on Port B) as an output.
    ```c
    DDRB = DDRB | (1 << DDB0);
    // Or the common shorthand:
    DDRB |= (1 << DDB0);
    ```
* **My Explanation:**
    1.  Assume `DDRB` is `00000000` (all inputs).
    2.  `(1 << DDB0)` means "take the number 1 and left-shift it 0 times". This creates a "bitmask": `00000001`.
    3.  We perform a bitwise `OR`:
        ```
          00000000  (Current DDRB value)
        | 00000001  (Our bitmask)
        ----------------
        = 00000001  (The new DDRB value)
        ```
    4.  The `OR` operation sets the 0th bit to `1` and leaves all other bits untouched.

* **Example (Setting multiple bits):**
    ```c
    // Set PB0 and PB1 as output
    DDRB |= (1 << DDB0) | (1 << DDB1);

    // Set the first four LSBs (0, 1, 2, 3) as output
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3);
    ```

#### To Clear a Bit (Set to `0`)
* **Use-Case:** Make a pin an **INPUT** (`DDRx`) or set an output pin **LOW** (`PORTx`).
* **Operators:** Bitwise **AND (`&`)** and **NOT (`~`)**.
* **Example:** Set `PB0` as an input.
    ```c
    DDRB = DDRB & (~(1 << DDB0));
    // Or the common shorthand:
    DDRB &= ~(1 << DDB0);
    ```
* **Explanation:**
    1.  `(1 << DDB0)` creates the mask: `00000001`.
    2.  `~(1 << DDB0)` *inverts* the mask: `11111110`.
    3.  The `AND` operation keeps all bits the same *except* for the 0th bit, which is forced to `0`.

* **Example (Setting LSBs as input):**
    ```c
    DDRB &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3));
    ```

### Method 2: The 8-Bit "Fast" Method
* This is useful when I want to configure the *entire port* at once.
* **Example:** Configure LSBs (0-3) as input and MSBs (4-7) as output.
    ```c
    // Using binary is easy to read
    DDRB = 0b11110000;
    
    // Or using hexadecimal (which is shorter)
    DDRB = 0xF0;
    ```
* **Example:** Set MSBs high and LSBs low on an output port.
    ```c
    // Note: This assumes DDRB is already set correctly
    PORTB = 0xF0; // 0b11110000
    ```

---

## 💡 Project 1: Blinking an LED

Time to put the theory to the test. This code will blink the built-in LED on the Arduino Uno board, which is connected to **Arduino Pin 13**. Looking at the pinout, this is `PB5` (Pin 5 on Port B).

### `main.c` (Bare-Metal Blink)

```c
/*
 * This is the bare-metal equivalent of the Arduino "Blink" sketch.
 * We must define the clock speed for the _delay_ms() function to work.
 */
#define F_CPU 16000000UL // Arduino Uno clock speed is 16MHz

#include <avr/io.h>      // This file has all the register definitions (DDRB, PORTB, etc.)
#include <util/delay.h>  // This file has the _delay_ms() function

int main(void)
{
    // --- SETUP ---
    // This part runs once.

    // Configure led pin (PB5) as an output.
    // We set the 5th bit of the Data Direction Register for Port B to 1.
    DDRB |= (1 << DDB5); 


    // --- LOOP ---
    // This part runs forever.
    while (1) 
    {
        // 1. Turn LED ON
        // Set the 5th bit of the PORTB register to 1 (HIGH).
        PORTB |= (1 << PB5);
        
        _delay_ms(1000); // Wait for 1000ms (1 second)

        // 2. Turn LED OFF
        // Clear the 5th bit of the PORTB register to 0 (LOW).
        PORTB &= ~(1 << PB5);
        
        _delay_ms(1000); // Wait for 1 second
    }
}
```

---

## How to Read an Input Pin (The `PINx` Register)

The final register, `PINx`, is for reading. Its main purpose is to get the status of input pins, for example, in an `if` statement.

* **Example:** Check if a button connected to `PINB0` is pressed.

```c
// --- IN SETUP ---
// 1. Set PB0 as an input (clear the bit)
DDRB &= ~(1 << DDB0);  

// 2. Enable the internal pull-up resistor for PB0 (set the bit)
// This means the pin will be HIGH by default and LOW when pressed
PORTB |= (1 << PB0);   

// --- IN LOOP ---
while(1)
{
    // We read the PINB register
    // We use the bitwise AND to "mask" out all bits except PINB0.
    // If PINB0 is 0 (LOW), the whole expression becomes 0.
    
    // The "!" inverts the logic, so "if NOT (HIGH)" means "if (LOW)"
    if ( !(PINB & (1 << PINB0)) )
    {
        // Button is pressed (pin is LOW)
        // Do something... (e.g., turn on an LED)
    }
    else
    {
        // Button is not pressed (pin is HIGH)
        // Do something else... (e.g., turn off the LED)
    }
}
```
