
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