unsigned char segment[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x6F};

void setup() {
  //set port d as output

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
