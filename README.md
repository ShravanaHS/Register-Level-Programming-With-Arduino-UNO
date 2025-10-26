# ATmega328P Bare-Metal Programming Journey
[![AVR](https://img.shields.io/badge/Microcontroller-ATmega328P-orange?style=for-the-badge&logo=microchip)](https://www.microchip.com/en-us/product/ATmega328P)
[![Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/Platform-Arduino%20Uno-cyan?style=for-the-badge&logo=arduino)](https://store.arduino.cc/products/arduino-uno-rev3)
[![Simulation](https://img.shields.io/badge/Simulation-Wokwi%20%7C%20SimulIDE-brightgreen?style=for-the-badge)](https://wokwi.com/)

## 🔰 Introduction

Welcome! This repository documents my learning journey into **bare-metal C programming** for the **ATmega328P** microcontroller, the chip at the heart of the popular Arduino Uno.

This approach, also known as **register-level programming**, involves moving beyond the simple Arduino libraries (like `digitalWrite()`) to control the hardware directly. We do this by reading and writing to the microcontroller's special function registers.

The goals of this project are to:
- ⚡ Gain a deep understanding of the ATmega328P architecture.
- 🛠️ Learn to configure and control peripherals from the ground up.
- 🚀 Write more efficient and optimized C code.
- 🐧 Get comfortable with the bare-metal development workflow.

💡 The aim is to **learn → implement → document** each concept step-by-step.

---
## 🗺️ Learning Roadmap

This repository is organized into modules, each focusing on a core peripheral or concept.

| Module | Focus Area | Status |
| :---: | :--- | :---: |
| **0**  | [Brief Overview and Basics](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/BASICS.md) | Completed |
| **01** | [GPIO (General Purpose I/O)](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/GPIO.md) | Completed |
| **02** | [Interrupts](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/INTERRUPTS.md) | Started |
| **03** | [Timers & Counters](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/TIMER.md) | Started |
| **04** | [PWM (Pulse Width Modulation)](./04-PWM/) | ◻️ Not Started |
| **05** | [ADC (Analog-to-Digital Converter)](./05-ADC/) | ◻️ Not Started |
| **06** | [UART Communication](./06-UART/) | ◻️ Not Started |
| **07** | [I2C Communication](./07-I2C/) | ◻️ Not Started |
<br>

---


## 🚀 Wokwi Simulation Projects

Here is a quick-access list of all the projects documented in this repository, complete with my technical notes and a live simulation link.

| Project | Module | Explanation (My Notes) | Wokwi Simulation |
| :--- | :---: | :---: | :---: |
| **Blinking an LED** | 01 - GPIO | [View Notes](GPIO.md#-project-1-blinking-an-led) | [Simulate NOW](https://wokwi.com/projects/445351244852802561) |
| **7-Segment Counter** | 01 - GPIO | [View Notes](GPIO.md#-project-2-interfacing-a-7-segment-display) | [Simulate NOW](https://wokwi.com/projects/445432374040690689) |
| **LCD Display** | 01 - GPIO | [View Notes](GPIO.md#-project-3-interfacing-a-16x2-lcd-display) | [Simulate NOW](https://wokwi.com/projects/445443029364988929) |
| **Switch Interface** | 01 - GPIO | [View Notes](GPIO.md#-project-4-reading-a-push-button-switch-interface) | [Simulate NOW](https://wokwi.com/projects/445627076538241025) |
| **Relay Control** | 01 - GPIO | [View Notes](GPIO.md#-project-5-interfacing-a-relay-controlling-high-power-devices) | [Simulate NOW](https://wokwi.com/projects/445627485643387905) |
| **LED Toggling Using Interrupts** | 02 - INTERRUPT | ... | [Simulate NOW](...) |
| **Simple Example** | 02 - INTERRUPT | ... | [Simulate NOW](...) |
| **Simple Example** | 03 - Timer | [View Notes](TIMER.md#-project-1-non-blocking-led-blink-with-timer1-overflow) | [Simulate NOW](https://wokwi.com/projects/445851563987053569) |
| **UART Communication** | 06 - UART | ... | [Simulate NOW](...) |

<br>

## 🛠️ Tools & Environment

To follow along, you'll need a few hardware and software tools.

### Hardware (Physical)
* **Arduino Uno Board:** (or any other board with an ATmega328P).
* **Peripherals:** LEDs, resistors, push-buttons, 7-segment display, LCD, potentiometer, etc.
* **Breadboard & Jumper Wires**

### Software & Toolchain
* **Arduino IDE:** We won't use its libraries, but it provides the complete **AVR-GCC toolchain** (`avr-gcc`, `avr-objcopy`) and **AVRDUDE** (the uploader) in one easy install.
* **Code Editor:** Any text editor (like VS Code) to write C code.

### Simulation (recommended)
You can test most of this code without any physical hardware using these excellent simulators:
* **[Wokwi](https://wokwi.com/):** A powerful, free, in-browser simulator for Arduino, ESP32, and more.
* **[SimulIDE](https://www.simulide.com/):** A free, open-source real-time electronic circuit simulator for your desktop.

---
