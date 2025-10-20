## 🗺️ Project Roadmap

My planned learning path, module by module:

- **GPIO (General Purpose Input/Output):** LEDs, 7-Segment Display, LCD, Switches
- **EXTERNAL INTERRUPTS:** INT0, INT1
- **TIMERS:** Input and Output Capture, Overflow Interrupt
- **PWM (Pulse Width Modulation):** LED Dimmer
- **ADC (Analog-to-Digital Converter):** Potentiometer Connection
- **COMMUNICATION:** UART, I2C

## 🔬 The Target: ATmega328P

The ATmega328P is a popular 8-bit microcontroller. All programming will be done by directly manipulating its hardware registers.

### Core Specifications
* **Architecture:** 8-bit
* **Flash Memory (Code):** 32KB
* **EEPROM (Non-volatile Data):** 1KB
* **SRAM (RAM):** 2KB
* **Max Clock Speed:** 20MHz

### Key Peripherals & Features
This chip comes with a standard set of peripherals that we will be learning to control:

* **GPIO Ports:** 3 (Port B, Port C, Port D)
* **Timers:** 3 (Timer 0, Timer 1, Timer 2)
* **ADC:** 10-bit resolution with 8 channels
* **PWM:** 6 channels
* **External Interrupts:** 2 (INT 0, INT 1)
* **Communication Interfaces:** USART, I2C, SPI

  ![ATmega328P Pinout Diagram](https://www.bing.com/images/search?view=detailV2&ccid=4Mt3pVu9&id=173AA64CE7F26A1667F3B0E121FFD1A8F0E1A14F&thid=OIP.4Mt3pVu93VkZVdvFNzpx9gHaGK&mediaurl=https%3a%2f%2fwww.microchip.com%2f_images%2fics%2fmedium-ATmega328P-TQFP-32.png&exph=460&expw=553&q=atmega328p&FORM=IRPRST&ck=6A46E9486789E032A838A8B74A6A808B&selectedIndex=5&itb=1)
---

## 📍 Pin Layout & Port Structure (28-Pin Package)

Understanding the physical pinout is the first step to connecting components.



### Key System Pins
* **Pin 1:** `RST` (Reset)
* **Pin 7:** `VCC` (Digital Supply Voltage)
* **Pin 20:** `AVCC` (Analog Supply Voltage)
* **Pin 8, 22:** `GND` (Ground)
* **Pin 21:** `AREF` (Analog Reference)
* **Pin 9, 10:** Clock Pins (Used for external crystal on an Arduino board)

### GPIO Port Mapping
The chip's pins are organized into three ports: B, C, and D.

* **Port B (PB0 - PB7):**
    * Physical Pins: 14-19
    * *Note: PB6 and PB7 are used for the external crystal on an Arduino.*
* **Port C (PC0 - PC6):**
    * Physical Pins: 23-28
    * *Note: PC6 is also the `RST` pin.*
* **Port D (PD0 - PD7):**
    * Physical Pins: 2-6, 11-13

        ![ATmega328P Pinout Diagram](https://www.bitfoic.com/upload/20231213/25844c0c0e8684550735d6f25012063d.jpg)

---

## 💡 Fundamental Concepts

To control these registers, we won't be using high-level functions. Instead, we will rely on **bitwise logic**.

* **AND (`&`) and OR (`|`)** logic will be the primary tools used to set, clear, and toggle individual bits within the 8-bit registers without disturbing the other bits. This is the core skill required for all subsequent modules.

### Next Module:
* [Module 1: GPIO](./01-GPIO/README.md)
