void setup() {
  //make PB0 8 AS output
  //make PD7 7 as inpot

  DDRB |= (1<<DDB0);
  DDRD &= ~(1<<DDD7);
}

void loop() {
  // check the status of PD2 BASED ON that change the output value
  //for pull down logic
  if(PIND &(1<<PIND7)){
    PORTB |= (1<<PB0);
  }
  else{
    PORTB &= ~(1<<PB0);
  }

}
