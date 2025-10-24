# Module 2: Interrupts (The Asynchronous World)

This module explores one of the most powerful features of any microcontroller: **Interrupts**.

So far, all my projects have been **synchronous**. The CPU runs one instruction at a time, in a predictable order. To check if a button is pressed, I have to *constantly* ask the pin, "Are you pressed? ... Are you pressed? ... Are you pressed?" This is called **polling**, and as we'll see, it's very inefficient.

Interrupts allow the microcontroller to work **asynchronously**. Instead of us checking for an event, the event *tells us* when it happens.

## 📚 Table of Contents

1.  [What is an Interrupt?](#what-is-an-interrupt-the-doorbell-analogy)
2.  [The Problem: Why Polling is Inefficient](#the-problem-why-polling-is-inefficient)
3.  [The Solution: How Interrupts Save Time](#the-solution-how-interrupts-save-time)
4.  [The Interrupt Service Routine (ISR)](#the-interrupt-service-routine-isr)
5.  [Types of Interrupts](#types-of-interrupts)
    * [External (Hardware) Interrupts](#1-external-hardware-interrupts)
    * [Internal (Peripheral) Interrupts](#2-internal-peripheral-interrupts)
6.  [Project 1: Dual Button LED Control with INT0 & INT1](#-project-1-dual-button-led-control-with-int0--int1)
    * [Hardware Setup](#hardware-setup)
    * [Simulation & Result](#simulation--result)
    * [Full Code (`main.c`)](#full-code-mainc)
    * [Line-by-Line Code Explanation](#line-by-line-code-explanation)

---

## What is an Interrupt? (The Doorbell Analogy)

An interrupt is a signal that **pauses the main program** to execute a special piece of code in response to an important event.

Think of your `main()` loop as you, in your kitchen, cooking a complex meal.

* **Polling (The Inefficient Way):** You are expecting a guest, but you have no doorbell. You have to stop cooking every 30 seconds, walk to the front door, and check if anyone is there. You waste most of your time checking, not cooking.
* **Interrupts (The Efficient Way):** You have a doorbell. You can focus 100% on cooking. When your guest arrives, they press the doorbell (the **interrupt event**). You hear it, *immediately* pause your cooking, go to the door, let them in (the **interrupt service routine**), and then *resume cooking* exactly where you left off.

This is the power of interrupts: the CPU can focus on its main tasks, and it only diverts its attention when an event *actually happens*.

## The Problem: Why Polling is Inefficient

Here is the "polling" method for turning on an LED with a button:

```c
void setup() {
    // Set LED pin as output
    // Set Button pin as input
}

void loop() {
    // The CPU is "stuck" in this loop forever
    while(1)
    {
        // It must CONSTANTLY check the button pin
        if (PIND & (1 << PIND2))
        {
            // Button is pressed, turn on LED
            PORTB |= (1 << PB5);
        }
        else
        {
            // Button not pressed, turn off LED
            PORTB &= ~(1 << PB5);
        }
        // ...What if I wanted to do other things, like
        // read a sensor or update an LCD?
        // This loop makes multitasking very difficult.
    }
}
```
The processor is 100% engaged, wasting millions of clock cycles just reading a pin that isn't changing.

## The Solution: How Interrupts Save Time
With an interrupt, our code looks completely different:
```c
void setup() {
    // 1. Configure LED pin as output
    // 2. Configure Button pin as an interrupt
    // 3. Enable interrupts
}

void loop() {
    // The main loop is now EMPTY.
    // The CPU is free to do other tasks, or even sleep
    // to save power. It is no longer checking the button.
}

// This special function is called AUTOMATICALLY
// when the button is pressed.
ISR(INT0_vect) {
    // Button was pressed! Turn on the LED.
    PORTB |= (1 << PB5);
}
```
The CPU only does work when the button is pressed. This allows us to multitask. The main loop() could be running a timer or updating an LCD, and the button press will interrupt it, run the ISR, and then resume the main loop.

### The Interrupt Service Routine (ISR)

The Interrupt Service Routine (ISR) is the special function that the CPU "jumps" to when an interrupt occurs. The name (e.g., `INT0_vect`) is a special name from the AVR library that links our function to the hardware.

Here's the in-depth process of what happens:

1.  **Event:** A rising edge signal arrives on the `INT0` pin.
2.  **Hardware Flag:** The ATmega328P's hardware sets an "interrupt flag" for `INT0` inside a special register.
3.  **Check:** The CPU finishes its current line of code (e.g., `a = b + c;`).
4.  **Pause & Save:** The CPU sees the flag. It automatically saves its current location (the Program Counter) and other key registers onto a part of memory called the **stack**. This is its "bookmark."
5.  **Jump:** The CPU jumps to the `INT0_vect` function's location in memory.
6.  **Execute ISR:** Our code inside `ISR(INT0_vect)` runs (e.g., `PORTB |= (1<PB5);`).
7.  **Return:** The ISR finishes.
8.  **Restore:** The CPU automatically retrieves its "bookmark" (the saved location and registers) from the stack.
9.  **Resume:** The `main()` loop resumes executing from the exact place it was paused, with no idea it was ever interrupted.

### Types of Interrupts

#### 1. External (Hardware) Interrupts:

* Triggered by a signal change on a physical pin.
* **`INT0` and `INT1`:** These are the primary external interrupts on pins `PD2` and `PD3` (Arduino pins 2 and 3). We can set them to trigger on a LOW level, any change, a falling edge, or a rising edge.
* **Pin Change Interrupts (`PCINT`):** Most other pins can be set to trigger an interrupt when their state changes.

#### 2. Internal (Peripheral) Interrupts:

* Triggered by the microcontroller's *internal* hardware modules, not an external pin.
* **Timer Interrupt:** Instead of `delay()`, you can set a timer to "overflow" every 1ms. The timer's overflow fires an interrupt. You can count these interrupts to create a perfect, non-blocking clock.
* **ADC Interrupt:** An Analog-to-Digital Conversion takes time. You can start a conversion, let the CPU do other work, and have the ADC fire an interrupt (`ADC_vect`) *only* when the result is ready to be read.
* **USART Interrupt:** Instead of polling ("is there a new byte of serial data?"), you can have the USART module fire an interrupt (`USART_RX_vect`) the *instant* a new byte arrives, so you can process it immediately.

---

## 💡 Project 1: Dual Button LED Control with INT0 & INT1

This project is the perfect demonstration of interrupts.

* The `main()` loop will be **completely empty**.
* We will connect two push buttons to the two external interrupt pins: `INT0` and `INT1`.
* `INT0` will be programmed to **Turn an LED ON**.
* `INT1` will be programmed to **Turn an LED OFF**.

### Hardware Setup

| Component | ATmega328P Pin | Arduino Uno Pin |
| :--- | :---: | :---: |
| Button 1 (ON) | `PD2` | D2 (`INT0`) |
| Button 2 (OFF)| `PD3` | D3 (`INT1`) |
| LED | `PB5` | D13 (built-in LED) |

*(Note: The code uses `PB5` (Pin 13), the built-in LED, for easy testing. The user prompt also mentioned `PD5`, which would also work fine, just change `DDRB` to `DDRD` and `PORTB` to `PORTD` in the code.)*

### Simulation & Result
![Simulation Result](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/images/Screenshot%202025-10-20%20222104.png)

- [Simulate NOW](https://wokwi.com/projects/445685629186064385)

### Full Code 
```c

#include <avr/io.h>
#include <avr/interrupt.h> // Required for ISR() and sei()

void setup() {
  
  // 1. Set LED Pin (PB5 / Pin 13) as an OUTPUT
  DDRB |= (1 << DDB5); 

  // --- Interrupt Configuration ---

  // 2. Enable Global Interrupts ("Master Switch")
  // SREG |= (1 << 7); // This is the direct register method
  sei(); // This is the built-in function that does the same thing

  // 3. Enable the specific interrupts we want (INT0 and INT1)
  // EIMSK = External Interrupt Mask Register
  EIMSK |= (1 << INT0) | (1 << INT1);

  // 4. Set the trigger type for INT0 and INT1
  // EICRA = External Interrupt Control Register A
  // We want to trigger on the RISING edge (11)
  EICRA |= (1 << ISC00) | (1 << ISC01); // Set INT0 to rising edge
  EICRA |= (1 << ISC10) | (1 << ISC11); // Set INT1 to rising edge
  
  // Note: We don't need to set the button pins as INPUT.
  // When an interrupt is enabled on a pin, it is
  // automatically configured as an input.
  // For a rising-edge, a pull-down resistor is assumed.
}

// The main loop is empty! The CPU is free.
void loop() {}

// --- Interrupt Service Routines ---

// ISR for INT0 (Button 1)
ISR(INT0_vect)
{
  // Turn the LED ON
  PORTB |= (1 << PB5);
}

// ISR for INT1 (Button 2)
ISR(INT1_vect)
{
  // Turn the LED OFF
  PORTB &= ~(1 << PB5);
}
```

### Line-by-Line Code Explanation

#### `void setup()`
```c
DDRB |= (1 << DDB5);
```
* **Purpose:** Standard GPIO setup. We set the 5th bit of the `DDRB` register.
* **Result:** `PB5` (Arduino Pin 13) is configured as an **OUTPUT** pin to drive our LED.
```c
sei();
```

* **Purpose:** This stands for **Set Enable Interrupts**. It is a built-in function from `<avr/interrupt.h>`.
* **Result:** This function sets the 7th bit of the **Status Register (`SREG`)**, also known as the **Global Interrupt Enable (GIE)** bit. This is the **master switch** for *all* interrupts on the chip. If this is not set, no interrupts will ever run.
```c
EIMSK |= (1 << INT0) | (1 << INT1);
```

* **Purpose:** This modifies the **External Interrupt Mask Register (`EIMSK`)**.
* **Result:** We are "un-masking" (enabling) the two interrupts we want to use. We set the `INT0` bit (bit 0) and the `INT1` bit (bit 1) to `1`. Now, the CPU will *specifically* listen for events on `INT0` and `INT1`.
```c
EICRA |= (1 << ISC10) | (1 << ISC11) | (1 << ISC00) | (1 << ISC01);
```
* **Purpose:** This modifies the **External Interrupt Control Register A (`EICRA`)**. This register tells the CPU *what kind* of signal to trigger on.
* **Result:** We are setting 4 bits.
    * `ISC00` and `ISC01` control `INT0`.
    * `ISC10` and `ISC11` control `INT1`.
* According to the ATmega328P datasheet, setting these bits to `11` (e.g., `(1<<ISC00) | (1<<ISC01)`) configures the interrupt to trigger on a **rising edge** (when the signal goes from LOW to HIGH). We do this for both `INT0` and `INT1`.

#### `void loop()`
```c
void loop() {}
```
* **Purpose:** This is the main program loop.
* **Result:** It's empty. This perfectly demonstrates that the CPU is not polling or wasting time. It's free to do other tasks (or just idle) while it waits for the hardware to trigger an interrupt.

#### The `ISR`s
```c
ISR(INT0_vect)
{
PORTB |= (1 < PB5);
}
```
* **Purpose:** This is the **Interrupt Service Routine for `INT0`**. `INT0_vect` is a special name (vector) that links this function to the `INT0` hardware.
* **Result:** When a rising edge is detected on the `PD2` (`INT0`) pin, the CPU *automatically* pauses `loop()` and runs this code. This code sets the `PB5` bit in `PORTB` to `1`, turning the LED **ON**.
```c
ISR(INT1_vect)
{
PORTB &= ~(1 < PB5);
}
```
* **Purpose:** This is the **Interrupt Service Routine for `INT1`**.
* **Result:** When a rising edge is detected on the `PD3` (`INT1`) pin, the CPU pauses `loop()` and runs this code. This code clears the `PB5` bit in `PORTB` to `0`, turning the LED **OFF**.
