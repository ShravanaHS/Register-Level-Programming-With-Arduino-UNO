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
