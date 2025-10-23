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

![Simulation Result](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/images/Screenshot%202025-10-20%20222104.png)

- [Simulate NOW](https://wokwi.com/projects/445351244852802561)

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



---

## 💡 Project 2: Interfacing a 7-Segment Display

The next logical step after blinking a single LED is controlling multiple LEDs at once. A 7-segment display is perfect for this, as it's essentially 7+ LEDs arranged in a figure-8 pattern to display numbers and some letters.

### Working Principle: Common Cathode vs. Common Anode

There are two types of 7-segment displays, and the only difference is the polarity of the LEDs:

1.  **Common Cathode (CC):** All the negative terminals (cathodes) of the LEDs are connected to a single pin. This common pin must be connected to **GND**. To turn on a segment, we send a **HIGH (`1`)** signal to its individual pin.
2.  **Common Anode (CA):** All the positive terminals (anodes) of the LEDs are connected. This common pin must be connected to **VCC (5V)**. To turn on a segment, we send a **LOW (`0`)** signal to its individual pin (the logic is inverted).

For this project, I will be using a **Common Cathode** display.



### Hardware Connections

I will use the entire `PORTD` of the ATmega328P to drive the display. This makes the code simple as I can send one byte to control all segments at once.

* A **current-limiting resistor** (typically 220Ω or 330Ω) must be connected in series with each segment pin (a-g) to prevent burning out the LEDs.
* The **common pin** of the 7-segment display must be connected to **GND**.

| Segment Pin | ATmega328P Pin |
| :---------: | :------------: |
|      a      |      PD0       |
|      b      |      PD1       |
|      c      |      PD2       |
|      d      |      PD3       |
|      e      |      PD4       |
|      f      |      PD5       |
|      g      |      PD6       |

![Real 7-Segment Display](https://media.geeksforgeeks.org/wp-content/uploads/20200413202916/Untitled-Diagram-237.png)

### Theory & Segment Mapping

To display a number, I need to send a specific 8-bit pattern to `PORTD`. The bits of the `PORTD` register correspond to the display's segments as follows (bit 7, the most significant bit, will control the decimal point, which we won't use here).

| Bit         | 7  | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| :---------- | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| **Segment** | dp | g | f | e | d | c | b | a |

Here is the truth table that translates each decimal digit into its 8-bit binary pattern and the final hex code I'll use in my C code array.

| Decimal Number | Binary `(dp g f e d c b a)` | Hex Equivalent |
| :------------: | :-------------------------: | :------------: |
|       0        |        `0011 1111`          |     `0x3F`     |
|       1        |        `0000 0110`          |     `0x06`     |
|       2        |        `0101 1011`          |     `0x5B`     |
|       3        |        `0100 1111`          |     `0x4F`     |
|       4        |        `0110 0110`          |     `0x66`     |
|       5        |        `0110 1101`          |     `0x6D`     |
|       6        |        `0111 1100`          |     `0x7C`     |
|       7        |        `0000 0111`          |     `0x07`     |
|       8        |        `0111 1111`          |     `0x7F`     |
|       9        |        `0110 1111`          |     `0x6F`     |

### Simulation & Result
![Wokwi Simulation](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/images/Screenshot%202025-10-21%20195853.png)
<br>
- [Simulate NOW](https://wokwi.com/projects/445432374040690689)


### `main.c` (0-9 Counter)

This code will configure `PORTD` as an output port and then loop through the array of hex codes, sending each pattern to the display with a delay to create a 0-9 counter.

```c
//// This array stores the hex codes for digits 0-9.
unsigned char segment[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x6F};

void setup() {
  //set port D as output or Configure all 8 pins of PORTD as outputs.
// 0xFF is the hexadecimal equivalent of 0b11111111.
  DDRD = 0xFF;
}

void loop() {
  // printing numbers
  for(int i = 0; i<10; i++){
    PORTD = segment[i];
    delay(1000);
  }
  delay(2000);

}

```
---

## 💡 Project 3: Interfacing a 16x2 LCD Display

Displaying numbers is useful, but for showing text and full words, I need to use an LCD (Liquid Crystal Display). For this project, I'll interface a standard 16x2 character LCD.

### How a 16x2 LCD Works

A 16x2 LCD can display 16 characters per row, with 2 rows in total. Each character is not a simple segment, but is actually formed by a **5x8 dot matrix** of tiny pixels.


Turning individual dots on and off is complex, so these displays have a built-in controller chip to handle that job. The most common one is the **Hitachi HD44780**. My task is simply to send commands and character data to this controller.

The controller has two important registers:
1.  **Instruction Register (IR):** I send commands here, like "clear the screen," "move the cursor to the second row," or "turn the display on."
2.  **Data Register (DR):** I send the actual character data (ASCII values) here to be displayed at the current cursor location.

### LCD Memory Map

Each of the 32 character positions has a specific address in the Display Data RAM (DDRAM). To print something at a specific location, I first send a command to move the cursor to that address.

* **Row 1:** Addresses `0x80` to `0x8F`
* **Row 2:** Addresses `0xC0` to `0xCF`

### LCD Pinout

The standard 16-pin LCD has the following layout:

| Pin | Name | Function                                                                  |
| :--: | :--: | :------------------------------------------------------------------------ |
| 1   | VSS  | Ground (0V)                                                               |
| 2   | VDD  | Power Supply (+5V)                                                        |
| 3   | V0/VE | Contrast Adjustment (Connected to potentiometer)                          |
| 4   | **RS** | **Register Select:** `0` = Command (Instruction), `1` = Data              |
| 5   | **RW** | **Read/Write:** `0` = Write to LCD, `1` = Read from LCD (usually tied to GND) |
| 6   | **EN** | **Enable:** A HIGH-to-LOW pulse latches the data on the data pins.         |
| 7-14| D0-D7| **Data Bus Pins:** Carries the 8 bits of data or command.                 |
| 15  | A/LED+ | Backlight Anode (+5V)                                                     |
| 16  | K/LED- | Backlight Cathode (GND)                                                   |


 ![LCD Display](https://www.etechnophiles.com/wp-content/uploads/2023/02/16x2-LCD-Pinout-Specifications-Applications.jpg) 

### 8-bit vs. 4-bit Mode

The HD44780 controller can be operated in two modes:

* **8-bit Mode:** Uses all 8 data pins (D0-D7). It's faster and the code is simpler, but it requires 8 GPIO pins from the microcontroller just for data.
* **4-bit Mode:** Uses only 4 data pins (D4-D7). It saves GPIO pins but is slightly slower and more complex to program, as each 8-bit command/data byte must be sent in two 4-bit chunks (called "nibbles").

For this project, I will implement the simpler **8-bit mode**.

### Hardware Connections (ATmega328P)

* The 3 control pins (`RS`, `RW`, `EN`) will be connected to `PORTB`.
* The 8 data pins (`D0`-`D7`) will be connected to `PORTD`.

| LCD Pin | ATmega328P Pin |
| :-----: | :------------: |
|   RS    |      PB0       |
|   RW    |      PB1       |
|   EN    |      PB2       |
|  D0-D7  |     PD0-PD7    |

### Essential LCD Commands

These are the basic commands I need to send to the Instruction Register (`RS=0`) to initialize and control the LCD.

| Hex Code | Command Description                     |
| :------: | :-------------------------------------- |
|  `0x38`  | Initialize the LCD in 8-bit mode, 2 lines, 5x8 font. |
|  `0x0C`  | Turn Display ON, Cursor OFF.            |
|  `0x06`  | Auto-increment cursor position after each character. |
|  `0x01`  | Clear the entire display screen.        |
|  `0x80`  | Force cursor to the beginning of the 1st line. |
|  `0xC0`  | Force cursor to the beginning of the 2nd line. |

### Simulation & Result

 ![Wokwi LCD Simulation](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/images/Screenshot%202025-10-21%20224514.png) 
 
 - [Simulate NOW](https://wokwi.com/projects/445443029364988929)

### `main.c` (LCD Interfacing)

The code is broken down into helper functions to make it clean:
* `lcd_command()`: Sends a command byte to the LCD.
* `lcd_data()`: Sends a data byte (a character) to the LCD.
* `lcd_string()`: Sends a whole string of characters.
* `lcd_init()`: Runs the sequence of commands to initialize the display.

```c

#define RS_PIN PB0
#define RW_PIN PB1
#define EN_PIN PB2

// Function Prototypes
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_string(const char *str);
void lcd_init(void);

void setup()
{
   
    // Configure PORTD (D0-D7) as output for data lines
    DDRD = 0xFF;
    // Configure PORTB pins (RS, RW, EN) as output for control lines
    DDRB |= (1 << RS_PIN) | (1 << RW_PIN) | (1 << EN_PIN);

    // Initialize the LCD
    lcd_init();
}

void loop()
    {
        lcd_command(0x83); // Move cursor to column 3 of row 1
        lcd_string("Shravana");
        
        lcd_command(0xC3); // Move cursor to column 3 of row 2
        lcd_string("SHS");

        _delay_ms(2000);
        lcd_command(0x01); // Clear screen

        lcd_command(0x83);
        lcd_string("engineering");
        
        lcd_command(0xC3);
        lcd_string("HII");
        
        _delay_ms(2000);
        lcd_command(0x01); // Clear screen
    }


// Sends a command to the LCD
void lcd_command(unsigned char cmd)
{
    PORTD = cmd; // Send command to PORTD
    PORTB &= ~(1 << RS_PIN); // RS = 0 (command)
    PORTB &= ~(1 << RW_PIN); // RW = 0 (write)

    PORTB |= (1 << EN_PIN);  // EN = 1 (latch)
    _delay_ms(5);
    PORTB &= ~(1 << EN_PIN); // EN = 0
}

// Sends data (a character) to the LCD
void lcd_data(unsigned char data)
{
    PORTD = data; // Send data to PORTD
    PORTB |= (1 << RS_PIN);  // RS = 1 (data)
    PORTB &= ~(1 << RW_PIN); // RW = 0 (write)

    PORTB |= (1 << EN_PIN);  // EN = 1 (latch)
    _delay_ms(5);
    PORTB &= ~(1 << EN_PIN); // EN = 0
}

// Sends a string to the LCD
void lcd_string(const char *str)
{
    while (*str != '\0') // Loop until null terminator
    {
        lcd_data(*str);
        str++;
    }
}

// Initializes the LCD
void lcd_init(void)
{
    _delay_ms(20); // Wait for LCD to power on
    lcd_command(0x38); // 8-bit mode, 2 lines, 5x8 font
    lcd_command(0x0C); // Display ON, Cursor OFF
    lcd_command(0x06); // Auto-increment cursor
    lcd_command(0x01); // Clear display
    _delay_ms(2);
}
```
---
---

## 💡 Project 4: Reading a Push Button (Switch Interface)

After controlling outputs (LEDs, LCDs), the next step is to get inputs from the user. The simplest form of input is a push button or switch.

### The Problem: "Floating" Pins

I can't just connect a button to a GPIO pin and leave it at that. When the button is **not** pressed, the input pin is connected to... nothing. This is called a **"floating"** state.

A floating pin is like a tiny antenna; it can randomly pick up electrical noise from the air and report `HIGH` or `LOW` back to the chip, making my program completely unreliable.

To fix this, I must use a **pull-up** or **pull-down** resistor. This resistor ensures the pin is always in a known, stable state (`HIGH` or `LOW`) when the button isn't being pressed.

---
 ![pull up](https://github.com/user-attachments/assets/18085af6-9969-42a6-9795-a74250633242) 
### Logic 1: The Pull-Down Resistor

In a pull-down setup, a resistor (e.g., 10kΩ) connects the GPIO pin directly to **Ground (GND)**. The button is set up to connect the pin to **VCC (5V)** when pressed.

* **Button NOT Pressed:** The resistor "pulls" the pin's voltage down to GND. The pin reads **LOW (0)**.
* **Button IS Pressed:** The button creates a direct path to VCC, which "overpowers" the resistor. The pin reads **HIGH (1)**.

This logic is straightforward: **LOW = Off, HIGH = On**.



---

### Logic 2: The Pull-Up Resistor

In a pull-up setup, the resistor connects the GPIO pin directly to **VCC (5V)**. The button is set up to connect the pin to **GND** when pressed.

* **Button NOT Pressed:** The resistor "pulls" the pin's voltage up to VCC. The pin reads **HIGH (1)**.
* **Button IS Pressed:** The button creates a direct path to GND. The pin reads **LOW (0)**.

This logic is **inverted**: **HIGH = Off, LOW = On**.



---

### Special Feature: The ATmega328P's Internal Pull-Ups

The ATmega328P (and most modern microcontrollers) has a very useful feature: **built-in pull-up resistors** that I can enable or disable in software. This saves me from having to add an external resistor to my circuit.

I can enable the internal pull-up on any pin by:
1.  Setting the pin as an **INPUT** (e.g., `DDRD &= ~(1 << PD2);`).
2.  Writing a `1` to that pin's bit in the **`PORTx`** register (e.g., `PORTD |= (1 << PD2);`).

When I use the internal pull-up, I must use the **inverted logic (Logic 2)** in my code: I check for a **LOW (0)** signal to detect a button press. This is the most common and efficient method.

### Simulation & Result

 ![Wokwi LCD Simulation](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/images/Screenshot%202025-10-21%20224514.png) 
 
 - [Simulate NOW](https://wokwi.com/projects/445627076538241025)


#### `main.c` 

This code checks if `PD2` (Arduino Pin 2) is `HIGH`. If it is, it turns on the LED at `PB5` (Arduino Pin 13).

```c

void setup() {
  //make PB5 13 AS output
  //make PD2 2 as inpot

  DDRB |= (1<<DDB5);
  DDRD &= ~(1<<DDD2);
}

void loop() {
  // check the status of PD2 BASED ON that change the output value
  //for pull down logic
  if(PIND &(1<<PIND2)){
    PORTB |= (1<<PB5);
  }
  else{
    PORTB &= ~(1<<PB5);
  }

}


```

---

## 💡 Project 5: Interfacing a Relay (Controlling High-Power Devices)

This project is a major step up. So far, I've controlled low-power components like LEDs. But what about controlling a high-voltage device, like a 220V AC light bulb or a fan?

The GPIO pins of the ATmega328P can only provide a tiny amount of current at 5V. Connecting them directly to a high-power device would instantly destroy the chip.

The solution is to use a **Relay**.

### What is a Relay?

A relay is an **electromechanical switch**. It's a way to control a high-power circuit using a low-power signal.



### How Does a Relay Work?

A relay has two separate parts, electrically isolated from each other:

1.  **The Control Side (Low-Voltage):**
    * This side has a small **electromagnet (coil)**.
    * When I send a 5V signal (from my microcontroller) to this coil, it becomes a magnet.
    * This magnetism *pulls* a metal switch on the other side.

2.  **The Switch Side (High-Voltage):**
    * This side is just a physical switch, exactly like a light switch on your wall. It has three terminals:
    * **COM (Common):** The "input" for the high-voltage power.
    * **NO (Normally Open):** This contact is *disconnected* by default. The high-voltage circuit is OFF. When the coil is energized, the switch is pulled to this position, turning the circuit ON.
    * **NC (Normally Closed):** This contact is *connected* by default. The high-voltage circuit is ON. When the coil is energized, the switch is pulled *away* from this position, turning the circuit OFF.

By sending a 5V signal to the coil, I can safely open or close a 220V AC circuit.

### The Transistor Driver Circuit

There's one more problem: The relay's coil, while low-voltage, still needs more current (e.g., 50-100mA) than the ATmega328P pin (max 40mA) can safely supply.

I must use a **transistor (like a BC547 or 2N2222)** to act as a *second* switch.

1.  The GPIO pin sends a small, safe signal (5V, ~5mA) to the transistor's **Base**.
2.  This small signal "opens the gate" of the transistor.
3.  This allows a much larger current to flow from an external 5V supply, through the relay's coil, and through the transistor's **Collector** and **Emitter** to GND.
4.  A **flyback diode** (e.g., 1N4007) is placed across the coil to protect the transistor from a voltage spike when the relay turns off.

 ![Relay](https://1.bp.blogspot.com/-kNcTcL4HhlI/X5Bm5h8j-JI/AAAAAAAAByc/ytxkI2e4VboQ4NHQFsDpcV1jHibJmR5BACLcBGAsYHQ/s742/5%2Bpin%2Brelay%2Bwiring%2Bdiagram.png) 

### Hardware Connections

For this project, I'll control the relay with a push button.

* **Input:** Push Button on `PD7` (Arduino Pin 7), using **pull-down logic**.
* **Output:** Transistor Base connected (via a 1kΩ resistor) to `PB0` (Arduino Pin 8).
* **Relay Coil:** Connected to 5V and the transistor's Collector.
* **High-Voltage Device:** Connected to the COM and NO (Normally Open) terminals.

### Simulation & Result

 ![Wokwi LCD Simulation](https://github.com/ShravanaHS/Register-Level-Programming-With-Arduino-UNO/blob/main/images/Screenshot%202025-10-21%20224514.png) 
 
 - [Simulate NOW](https://wokwi.com/projects/445627485643387905)






### `main.c` (Button-Controlled Relay)

This code will read the button on `PD7`. If the button is pressed (`HIGH`), it will send a `HIGH` signal to `PB0`, which turns on the transistor, which in turn energizes the relay and switches on the high-voltage device.

```c
/*
 * Project 5: Button-Controlled Relay
 * Reads a push button on PD7 (pull-down) to control a relay
 * via a transistor on PB0.
 */
#define F_CPU 16000000UL // 16MHz clock

#include <avr/io.h>
#include <util/delay.h>

#define RELAY_PIN   PB0  // (Arduino Pin 8) -> Transistor Base
#define BUTTON_PIN  PIND7 // (Arduino Pin 7) -> Button Input

int main(void)
{
    // --- SETUP ---
    // 1. Set RELAY_PIN (PB0) as an OUTPUT
    DDRB |= (1 << RELAY_PIN);
    
    // 2. Set BUTTON_PIN (PD7) as an INPUT
    DDRD &= ~(1 << BUTTON_PIN);
    
    // --- LOOP ---
    while (1)
    {
        // Check the status of the PIND register using pull-down logic
        if (PIND & (1 << BUTTON_PIN))
        {
            // Button is pressed (HIGH), turn Relay ON
            // (Send 5V to the transistor base)
            PORTB |= (1 << RELAY_PIN);
        }
        else
        {
            // Button is not pressed (LOW), turn Relay OFF
            // (Send 0V to the transistor base)
            PORTB &= ~(1 << RELAY_PIN);
        }
    }
}
```
