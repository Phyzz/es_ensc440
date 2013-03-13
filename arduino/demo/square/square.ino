#include <SPI.h>

int DAC_SS = 9;

int PERIOD = 1; //in microseconds, freq of 10kHz
boolean wave_state = LOW;
unsigned long next_action_time = 0;

//unsigned char level_bytes[] = { 0x10, 0x00, 0x1F, 0xFF};
//char i = 0;


void setup() {
  SPI.setBitOrder(MSBFIRST);
//  SPI.setDataMode(SPI_MODE0);
//  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(DAC_SS, OUTPUT);
//  SPI.begin();
//  noInterrupts();
}

void loop() {
  if( micros() > next_action_time) {
    next_action_time += (PERIOD);
    digitalWrite(DAC_SS, wave_state);
    wave_state = !wave_state;
//    digitalWrite(DAC_SS, LOW);
//    SPI.transfer(level_bytes[i++]);
//    SPI.transfer(level_bytes[i]);
//    digitalWrite(DAC_SS, HIGH);
//    i = i ^ 0x03;
  }
}
