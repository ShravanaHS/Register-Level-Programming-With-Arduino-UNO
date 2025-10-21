void setup() {
  // Configure led pin as output that is pin 13 or PB5 19TH IC PIN

  DDRB = DDRB | (1<<DDB5);

}

void loop() {
 PORTB = PORTB | (1<<PB5); // setting PB5 as high
 delay(1000); //delay of 1sec
 PORTB = PORTB & ~(1<<PB5); //setting PB5 as low
 delay(1000);

}
