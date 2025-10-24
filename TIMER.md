# Module 3: Timers and Counters

This module is the key to unlocking real multitasking in a microcontroller. In the GPIO module, we used `_delay_ms()` to create a pause. This is a **blocking** function—it forces the CPU to stop and do nothing but count, wasting millions of cycles.

Timers are the professional, **non-blocking** solution. A timer is a hardware peripheral inside the ATmega328P that can count time *on its own*, in the background. This lets the main CPU loop run freely to do other tasks, and the timer will simply tap it on the shoulder (using an interrupt) when the time is up.

## 📚 Table of Contents

1.  [What is a Timer? (The Kitchen Timer Analogy)](#what-is-a-timer-the-kitchen-timer-analogy)
2.  [Why Use Timers? (The `_delay_ms()` Problem)](#why-use-timers-the-_delay_ms-problem)
3.  [How a Timer Works: The Core Principle](#how-a-timer-works-the-core-principle)
4.  [The Prescaler: Controlling the Timer's Speed](#the-prescaler-controlling-the-timers-speed)
5.  [Key Applications](#key-applications)
6.  [Project 1: Non-Blocking LED Blink](#-project-1-non-blocking-led-blink-with-timer0-overflow)

---

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

## 💡 Project 1: Non-Blocking LED Blink (with Timer0 Overflow)

My first timer project will be to recreate the "Blink" sketch, but the *right* way.

* The `main()` loop will be used to blink one LED (e.g., on `PB0`).
* The `TIMER0_OVF_vect` ISR will be used to blink a *different* LED (e.g., on `PB1`).
* This will prove that both tasks can run at different speeds, completely independent of each other, demonstrating true multitasking.

*(...Project 1 code and explanation will go here...)*
