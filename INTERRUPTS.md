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
