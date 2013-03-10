#include <SPI.h>

int DAC_SS = 9;

int PERIOD = 100; //in microseconds, freq of 10kHz
boolean wave_state = false;
unsigned long next_action_time = 0;

//unsigned char[4] level_bytes = { 0x10, 0x00, 0x1F, 0xFF};
//char i = 0;


void setup() {
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(DAC_SS, OUTPUT);
  SPI.begin();
}

void loop() {
  if( micros() > next_action_time) {
    next_action_time += (PERIOD/2);
    unsigned char level_bytes[2];
    if (wave_state) {
      level_bytes = {0x10, 0x00};
    } else {
      level_bytes = {0x1F, 0xFF};
    }
    digitalWrite(DAC_SS, LOW);
    for(int i = 0; i < 2; ++i) {
      SPI.transfer(level_bytes[i]);
    }
    digitalWrite(DAC_SS, HIGH);
    wave_state = !wave_state;
  }
}
