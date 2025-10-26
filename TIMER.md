# Module 3: Timers and Counters

This module is the key to unlocking real multitasking in a microcontroller. In the GPIO module, we used `_delay_ms()` to create a pause. This is a **blocking** function—it forces the CPU to stop and do nothing but count, wasting millions of cycles.

Timers are the professional, **non-blocking** solution. A timer is a hardware peripheral inside the ATmega328P that can count time *on its own*, in the background. This lets the main CPU loop run freely to do other tasks, and the timer will simply tap it on the shoulder (using an interrupt) when the time is up.

## 📚 Table of Contents

1.  [What is a Timer? (The Kitchen Timer Analogy)](#what-is-a-timer-the-kitchen-timer-analogy)
2.  [Why Use Timers? (The `_delay_ms()` Problem)](#why-use-timers-the-_delay_ms-problem)
3.  [How a Timer Works: The Core Principle](#how-a-timer-works-the-core-principle)
4.  [The Prescaler: Controlling the Timer's Speed](#the-prescaler-controlling-the-timers-speed)
5.  [Key Applications](#key-applications)
6.  [How to Configure a Timer (General Steps)](#how-to-configure-a-timer-general-steps)
7.  [Project 1: Non-Blocking LED Blink (with Timer1 Overflow)](#-project-1-non-blocking-led-blink-with-timer1-overflow)
    * [Why Timer1 (16-bit)?](#why-timer1-16-bit)
    * [Configuring Timer1 for a 1-Second Delay](#configuring-timer1-for-a-1-second-delay)
    * [Simulation & Result](#simulation--result)
    * [Full Code (`main.c`)](#full-code-mainc)
    * [Line-by-Line Code Explanation](#line-by-line-code-explanation)
  
    
## What is a Timer? (The Kitchen Timer Analogy)

A timer is a peripheral that is, at its heart, a **counter**. It's a special register in the microcontroller that just... counts. I can configure *what* it counts (e.g., CPU clock cycles) and *how fast* it counts.

**The best analogy is a kitchen timer:**

* **`_delay_ms(1000);` (Blocking):** You need to boil an egg for 10 minutes. You stare at your watch, counting every single second: "1... 2... 3...". For those 10 minutes, you can do *nothing* else. Your full attention is on counting. This is what `_delay_ms()` does to your CPU.

* **Timer Peripheral (Non-Blocking):** You set a kitchen timer for 10 minutes and *walk away*. You are now free to do other tasks—make coffee, read the news, etc. When the 10 minutes are up, the timer *rings*. This "ring" is the **Timer Interrupt**. You stop what you're doing, turn off the stove (the **ISR**), and then go right back to your other tasks.

The ATmega328P has three of these "kitchen timers":
* `Timer0` (8-bit)
* `Timer1` (16-bit)
* `Timer2` (8-bit)

---

## Why Use Timers? (The `_delay_ms()` Problem)

As explained above, `_delay_ms()` is a "blocking" function. While `_delay_ms(1000)` is running, the CPU cannot:
* Read any buttons
* Check for serial data
* Update an LCD
* Do *anything* else.

This is terrible for a responsive program.

A **Timer Interrupt** solves this. I can configure `Timer0` to overflow every 1ms. Then, I just need a global variable:

`volatile unsigned int milliseconds = 0;`

`ISR(TIMER0_OVF_vect) { milliseconds++; }`

Now, my main loop is completely free. If I need to wait 1000ms, I can just check the value of my `milliseconds` variable in a non-blocking way, all while doing other work.

---

## How a Timer Works: The Core Principle

A timer is just a register that counts clock pulses. Let's look at `Timer0`, which is an 8-bit timer.

1.  **The Counter Register (`TCNT0`):** This is an 8-bit register. It starts counting from a value (usually 0).
2.  **Counting:** On every "tick" of its clock, `TCNT0` increments: `0`, `1`, `2`, `3`...
3.  **Max Value:** Because it's an 8-bit register, the highest value it can hold is `255` (which is `0b11111111`).
4.  **Rollover (Overflow):** What happens on the next tick after `255`? The register "rolls over" back to `0`. This event, the transition from `255` to `0`, is called an **Overflow**.
5.  **The Flag:** When this overflow happens, the hardware *automatically* sets a flag (a bit) in the **Timer Interrupt Flag Register (`TIFR0`)**. This flag is named `TOV0` (Timer Overflow 0).
6.  **The Interrupt:** If I have enabled the **Timer Overflow Interrupt** (by setting a bit in the `TIMSK0` register), the `TOV0` flag will *also* trigger the `TIMER0_OVF_vect` **Interrupt Service Routine (ISR)**.

This is the "ring" of our kitchen timer.



---

## The Prescaler: Controlling the Timer's Speed

There's one big problem. The ATmega328P's system clock is **16,000,000 Hz** (16MHz).

If `Timer0` increments on every clock cycle, it will count from 0 to 255 in:
`256 / 16,000,000` = **0.000016 seconds (or 16 microseconds)**.

This is *way* too fast. It would overflow 62,500 times per second. My ISR would be called so often the chip wouldn't be able to do anything else.

The solution is the **Prescaler**.

A prescaler is a "clock divider." It sits between the 16MHz system clock and the timer, slowing the clock down to a more useful speed.

The formula is:
**`Timer Clock = System Clock / Prescaler Value`**

The ATmega328P gives us several prescaler options (set in the `TCCR0B` register):

| Prescaler Setting | Timer Clock Frequency (at 16MHz) | Time for one "Tick" |
| :--- | :--- | :--- |
| 1 | 16,000,000 Hz (16MHz) | 0.0625 µs |
| 8 | 2,000,000 Hz (2MHz) | 0.5 µs |
| 64 | 250,000 Hz (250kHz) | 4 µs |
| 256 | 62,500 Hz (62.5kHz) | 16 µs |
| 1024 | 15,625 Hz (15.625kHz)| 64 µs |

**Example:**
* If I choose a **1024** prescaler:
* The timer's clock is now 15,625 Hz.
* The `TCNT0` register increments every 64µs.
* The time to overflow (count 256 ticks) is: `256 * 64µs` = **16.384ms**.
* This is a *much* more useful speed. My `TIMER0_OVF_vect` ISR will now be called approximately 61 times per second.

---

## Key Applications

By using timers and their interrupts, I can accomplish many tasks:

* **Generate non-blocking delays:** As described above, this is the proper way to use time in a program.
* **Measure Frequency:** I can use a timer to count how many external pulses arrive on a pin in one second.
* **Generate PWM Signals:** This is a *huge* application. I can set a timer to "Compare" its count to a value. When the count matches, it can automatically set a pin HIGH or LOW. This is how we'll do LED dimming (`analogWrite()`) without using any CPU time.
* **Trigger other peripherals:** The ADC can be set to start a conversion automatically on a timer's overflow event.

---

## How to Configure a Timer (General Steps)

Configuring a timer in the ATmega328P to fire an interrupt (like the `Timer1` project) involves a specific 5-step process. You must set the right bits in the right control registers.

### Step 1: Set the Timer Mode

First, I need to decide *how* the timer will operate. Will it count to its max value and overflow (Normal Mode)? Will it count to a specific value and reset (CTC Mode)? Or will it generate a waveform (PWM Mode)?

* **Registers:** `TCCR1A` and `TCCR1B` (for Timer1)
* **Bits:** `WGM` (Waveform Generation Mode) bits.
* **Example (Normal Mode):** For a simple overflow, I set all `WGM` bits to `0`.
    `TCCR1A = 0x00;`
    `TCCR1B = 0x00;`

### Step 2: Set the Prescaler (Timer Speed)

The 16MHz system clock is too fast, so I must "divide" it to a slower, more useful speed.

* **Register:** `TCCR1B`
* **Bits:** `CS` (Clock Select) bits.
* **Example (1024 Prescaler):** To get the slowest clock (15,625 Hz), I set the `CS` bits to `101`.
    `TCCR1B |= (1 << CS12) | (1 << CS10);`

### Step 3: Set the Period (Preload Value)

I must tell the timer *when* to overflow. For a 1-second delay (15,625 ticks), I "preload" the counter with a starting value.

* **Register:** `TCNT1` (Timer/Counter1 Register)
* **Value:** `MAX_COUNT - TICKS_NEEDED`
* **Example (1-second delay):** `65535 - 15625 = 49910`
    `TCNT1 = 49910;`

### Step 4: Enable the Specific Timer Interrupt

Now I tell the chip to *please generate an interrupt* when this specific timer event (the overflow) happens.

* **Register:** `TIMSK1` (Timer/Counter1 Interrupt Mask Register)
* **Bit:** `TOIE1` (Timer/Counter1 Overflow Interrupt Enable)
* **Example:**
    `TIMSK1 |= (1 << TOIE1);`

### Step 5: Enable Global Interrupts

This is the **master switch**. No interrupts will ever run, no matter what, until this is set.

* **Function:** `sei();`
* **Register:** This function sets the 7th bit of the `SREG` (Status Register), also known as the Global Interrupt Enable (GIE) bit.
* **Example:**
    `sei();`
## 💡 Project 1: Non-Blocking LED Blink (with Timer1 Overflow)

My first timer project will be to recreate the "Blink" sketch, but the *right* way. The goal is to blink the built-in LED (`PB5`) every 1 second using a timer interrupt, leaving the main `loop()` completely free for other tasks.

### Why Timer1 (16-bit)?

For a 1-second delay, the 8-bit `Timer0` is not ideal. As we calculated, even with the largest (1024) prescaler, it overflows every **16.384ms**. To get 1 second, I'd need to let it overflow `1000 / 16.384 = ~61` times. This requires an extra counter variable in the ISR.

The 16-bit **`Timer1`** is perfect. It can count from 0 up to **65,535**. This much larger range makes it easy to create long delays like 1 second in a single overflow event.

### Configuring Timer1 for a 1-Second Delay

Here are the 5 steps I need to perform to set up `Timer1` to fire an interrupt every second.

#### 1. Set Timer Mode (Normal Mode)

The timer can run in different modes (Fast PWM, Phase Correct PWM, CTC, etc.). For a simple overflow, I need "Normal Mode," where the timer counts from `0` to its max value (`0xFFFF` or 65,535) and then overflows.

* According to the datasheet (Table 15-5), "Normal Mode" requires the mode bits `WGM13`, `WGM12`, `WGM11`, and `WGM10` to all be `0`.
* These bits are in the `TCCR1A` and `TCCR1B` registers. I'll clear them to ensure we are in Normal Mode.
    * `TCCR1A &= ~((1 << WGM10) | (1 << WGM11));`
    * `TCCR1B &= ~((1 << WGM12) | (1 << WGM13));`

#### 2. Set the Prescaler (1024)

I need to slow the timer's clock down from the 16MHz system clock. A large prescaler gives me a slower, more controllable count. I'll use **1024**.

* The prescaler is set by the `CS12`, `CS11`, and `CS10` bits in the `TCCR1B` register.
* For a 1024 prescaler, the bits must be `101`.
    * `TCCR1B |= (1 << CS12) | (1 << CS10);`
    * `TCCR1B &= ~(1 << CS11);`

#### 3. Calculate and Set the Preload Value

Now I can calculate how many "ticks" I need for 1 second.

* **System Clock:** 16,000,000 Hz
* **Prescaler:** 1024
* **Timer Clock Freq:** 16,000,000 / 1024 = **15,625 Hz**
* **Time per Tick:** 1 / 15,625 Hz = 0.000064 s = **64 µs**

To get a 1-second (1,000,000 µs) delay:
* **Ticks Needed:** 1,000,000 µs / 64 µs = **15,625 ticks**

The timer overflows when it hits 65,535. I don't want it to count all 65,535 ticks. I want it to overflow after only 15,625 ticks.

I can do this by "preloading" the counter register (`TCNT1`) with a starting value.

* **Max Value:** 65,535
* **Preload Value:** (Max Value) - (Ticks Needed) = 65,535 - 15,625 = **49,910**
    *(Note: A more precise way is 65536 - 15625 = 49911, but 49910 is close enough and works)*

By setting `TCNT1 = 49910;`, the timer will start counting *from* 49910. It will take exactly 15,625 more ticks to reach 65,535 and overflow, giving me a perfect 1-second interrupt.

#### 4. Enable the Timer1 Overflow Interrupt

I need to tell the timer to *actually trigger* an interrupt when it overflows.

* This is done in the `TIMSK1` (Timer/Counter1 Interrupt Mask) register.
* I must set the `TOIE1` (Timer Overflow Interrupt Enable 1) bit.
    * `TIMSK1 |= (1 << TOIE1);`

#### 5. Enable Global Interrupts

Finally, I must turn on the "master switch" for all interrupts.

* `sei();`

### Simulation & Result



* **[Simulate NOW](https://wokwi.com/projects/445851563987053569)**
### Full Code (`main.c`)

```c
/*
 * Module 3, Project 1: Non-Blocking Blink
 * Blinks the built-in LED (PB5) every 1 second
 * using the Timer1 overflow interrupt.
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h> // Required for ISRs and sei()

int main(void)
{
    // --- SETUP ---
    // 1. Set built-in LED (PB5) as an OUTPUT
    DDRB |= (1 << DDB5);

    // 2. Set Timer1 Mode to "Normal" (overflow at 0xFFFF)
    TCCR1A = 0x00; // WGM10 and WGM11 are 0
    TCCR1B = 0x00; // WGM12 and WGM13 are 0
    
    // 3. Set Prescaler to 1024
    // CS12=1, CS11=0, CS10=1
    TCCR1B |= (1 << CS12) | (1 << CS10);

    // 4. Set the Preload Value for 1 second
    // 65535 - 15625 = 49910
    TCNT1 = 49910;

    // 5. Enable the Timer1 Overflow Interrupt
    // TIMSK1 (Timer/Counter1 Interrupt Mask Register)
    // TOIE1 (Timer/Counter1 Overflow Interrupt Enable)
    TIMSK1 |= (1 << TOIE1);

    // 6. Enable Global Interrupts (Master Switch)
    sei();

    // --- LOOP ---
    // The main loop is empty! The CPU is free.
    // It could be reading sensors, updating an LCD, etc.
    while (1) 
    {
        // Do other work...
    }
}

// --- INTERRUPT SERVICE ROUTINE ---
// This function is called AUTOMATICALLY every 1 second
ISR(TIMER1_OVF_vect)
{
    // 1. Toggle the LED
    // Using XOR (^) is a fast way to flip the bit
    PORTB ^= (1 << PB5);
    
    // 2. IMPORTANT: Reload the preload value
    // The timer reset to 0 after overflowing, so we must
    // set it back to 49910 for the *next* 1-second interrupt.
    TCNT1 = 49910;
}
```



### Line-by-Line Code Explanation

#### `int main(void)` - Setup

DDRB |= (1 << DDB5);

* **Purpose:** Standard GPIO setup. We set the 5th bit of the `DDRB` register.
* **Result:** `PB5` (Arduino Pin 13) is configured as an **OUTPUT** pin to drive our LED.

TCCR1A = 0x00;
TCCR1B = 0x00;

* **Purpose:** To clear the Timer/Counter Control Registers for Timer1.
* **Result:** This sets all bits to `0`, which guarantees the timer is in **"Normal Mode"** (all `WGM` bits are `0`). In this mode, the timer simply counts up to its maximum value (`0xFFFF`) and then overflows.

TCCR1B |= (1 << CS12) | (1 << CS10);

* **Purpose:** To set the prescaler (the timer's speed) for Timer1.
* **Result:** This sets the `CS12` and `CS10` bits to `1`, while `CS11` remains `0`. This `101` bit pattern selects the **1024 prescaler**. The timer clock is now slowed from 16,000,000 Hz to `16,000,000 / 1024 = 15,625 Hz`.

TCNT1 = 49910;

* **Purpose:** To "preload" the timer's 16-bit counter register.
* **Result:** Instead of starting to count from 0, the timer starts at **49910**. Since the timer overflows at 65,535, it will now take `65,535 - 49910 = 15,625` ticks to overflow. At 15,625 ticks/second, this will take exactly **1 second**.

TIMSK1 |= (1 << TOIE1);

* **Purpose:** To enable the specific interrupt for the Timer1 overflow event.
* **Result:** This sets the **Timer Overflow Interrupt Enable (`TOIE1`)** bit in the `TIMSK1` register. This "un-masks" the interrupt, telling the CPU to run the ISR when the timer overflows.

sei();

* **Purpose:** This stands for **Set Enable Interrupts**. It is the "master switch" for all interrupts.
* **Result:** This function sets the 7th bit of the **Status Register (`SREG`)**, also known as the **Global Interrupt Enable (GIE)** bit. No interrupts will run until this is called.

#### `int main(void)` - Loop

while (1)
{
// Do other work...
}

* **Purpose:** This is the main program loop.
* **Result:** It's **empty**. This is the key to this project. The CPU is completely free to do other tasks (like read sensors or update an LCD) while the Timer1 hardware runs in the background, handling the LED blinking all by itself.

#### The `ISR`

ISR(TIMER1\_OVF\_vect)
{
// 1. Toggle the LED
PORTB ^= (1 << PB5);

// 2. IMPORTANT: Reload the preload value
TCNT1 = 49910;
}

* **Purpose:** This is the **Interrupt Service Routine for Timer1 Overflow**. `TIMER1_OVF_vect` is the special name (vector) that links this function to the hardware event.
* **Result:** This code runs *automatically* every 1 second.
    1.  `PORTB ^= (1 << PB5);`: The LED is toggled. The `^` (XOR) operator flips the bit (from 1 to 0, or 0 to 1).
    2.  `TCNT1 = 49910;`: This is **CRITICAL**. After overflowing, `TCNT1` resets to `0`. This line immediately **reloads the preload value**, so the timer will once again count 15,625 ticks for the *next* 1-second interrupt.
