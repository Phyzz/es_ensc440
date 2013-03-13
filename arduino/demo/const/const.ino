#include <SPI.h>

int DAC_SS = 9;

void setup() {
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(DAC_SS, OUTPUT);
  SPI.begin();
  digitalWrite(DAC_SS, LOW);
  SPI.transfer(0x17);
  SPI.transfer(0xFF);
  digitalWrite(DAC_SS, HIGH);
}

void loop() {
}
