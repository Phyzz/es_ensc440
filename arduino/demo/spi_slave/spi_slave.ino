#include <SPI.h>
#include "pins_arduino.h"

#define EOD           0xFF //end of data
#define NOT_READY     0xFD
#define READY         0xFC

#ifndef cbi
#define cbi(sfr,bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr,bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile byte samples[1024] = {}; //each sample is 10 bits, with no packing 2 bytes/sample so this is 512 samples
volatile unsigned int sample_index;
//unsigned int sample_interval = 16; //in microseconds
//unsigned long next_sample;

volatile unsigned int dump_index;
volatile unsigned int dumped_bytes;

volatile boolean dumping;
volatile byte prev_pin2;

void ss_change () {
  byte pin2 = PIND & 0x04;
  if (pin2 != prev_pin2) {
    // rising edge after a dump. rising edge after saying not ready doesn't require any changes
    if(pin2 && dumping) {
      dumping = false;
      sample_index = 0;
      dump_index = 0;
      TIMSK1 |= ( 1<< OCIE1A);
    // falling edge
    } else {
        dumping = true;
        SPDR = READY;
        dump_index = sample_index;
        dumped_bytes = 0;
        TIMSK1 &= ~( 1<< OCIE1A);
    }

  }
  prev_pin2 = pin2;
}
void setup() {
  // set ADC prescaler to 4 for max sample rate of 308 kHz
  cbi(ADCSRA,ADPS2);
  sbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
  
  //MISO and MOSI stay the same in Slave mode, in other words connect straight across not cross over
  pinMode(MISO, OUTPUT);
  
  SPCR = 0;
  // enable the SPI interface in slave mode
  SPCR |= _BV(SPE);
  
  // enable SPI interrupts
  SPCR |= _BV(SPIE);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 177;
  
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  
  TIMSK1 |= ( 1<< OCIE1A);
  sei();
  
  //interupt 0 on pin 2, pin 2 must be externally connected to SS pin 10
  attachInterrupt(0, ss_change, CHANGE);

  sample_index = 0;
//  next_sample = 0;
  
  dump_index = 0;
  dumped_bytes = 0;
  
  dumping = false;
//  ready_to_dump = false;
  prev_pin2 = 1;
}

ISR (SPI_STC_vect) {
  if (dumped_bytes == 1024) {
    SPDR = EOD;
  } else  {
    SPDR = samples[dump_index++];
    ++dumped_bytes;
    // cut off any bits over 1024 to cause roll over
    dump_index &= 0x3FF;
  }
}

ISR(TIMER1_COMPA_vect) {
  unsigned int sample = analogRead(0);
  samples[sample_index++] = (byte) (sample >> 8);
  samples[sample_index++] = (byte) sample;
  // cut off any bits at 1024 or above to cause roll over
  sample_index &= 0x3FF;
}

void loop() {
}
