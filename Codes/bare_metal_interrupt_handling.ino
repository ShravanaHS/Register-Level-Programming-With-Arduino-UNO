void setup() {
 
  DDRB |= (1<<DDB5); //PB5 output 13

  //enable the gie(global interrupt enable bit)
  //to use any interrupt in our programm we need to set this bit GIE
  // for avr  we have AVR Status register that AREG its 7th bit is SREG (access through data sheet)

  SREG |= (1<<7);
  //ARDUINO IN BUILT FUNCTION TO DO THE SAME TASK
  //sei();

  // enabling required external interrupt from corresponding register
  // int0 nd int1 we are using so enable those 2
  //EIMSK this registers 0,1 bits are int0 and int 1 we need to enable it 

  EIMSK |= (1<<INT0) |(1<<INT1);

  //set the edge select for external interrupts
  // we are giving rising pulese using push button so
  //we need to configure in such a waqy that whenever we see rising edge we need to generate interrupt
  // we have eicra register using 0,1 and 2,3 bits we can set edge for iinit 0 an dint 1
  // table is there 00,01,10,11 to configure it we have to set 11
  EICRA |= (1<<ISC10) | (1<<ISC11) | (1<<ISC00) | (1<<ISC01);
  
  
   }

void loop() {}
  
  //INPLEMENTING INTERRUPT HANDLER

  ISR(INT0_vect)
  {
    PORTB |= (1<PB5);
  }

  ISR(INT1_vect)
  {
    PORTB &= ~(1<PB5);
  }

