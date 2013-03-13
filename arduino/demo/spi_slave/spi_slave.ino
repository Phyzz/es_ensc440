#include <SPI.h>
#include "pins_arduino.h"

#define START_TX      0xFA
#define CONTINUE_TX   0xF5
#define EOD           0xFF //end of data
#define ERROR         0xFE
#define NOT_READY     0xFD
#define READY         0xFC

byte samples[1024] = {}; //each sample is 10 bits, with no packing 2 bytes/sample so this is 512 samples
unsigned int sample_index = 0;
unsigned int sample_interval = 1000; //in microseconds
unsigned int next_sample = 0;


volatile unsigned int dump_index;
unsigned int dump_end_index;

enum State {DATA_COLLECTION_INCOMPLETE, DATA_COLLECTION_COMPLETE, DATA_DUMP};

volatile State current_state = DATA_COLLECTION_COMPLETE;
volatile byte prev_pin2;

void ss_change () {
  byte pin2 = PIND & 0x04;
  if (pin2 != prev_pin2) {
    // rising edge after a dump. rising edge after saying not ready doesn't require any changes
    if(pin2 && current_state == DATA_DUMP) {
      current_state = DATA_COLLECTION_INCOMPLETE;
      sample_index = 0;
    // falling edge
    } else {
      if (current_state == DATA_COLLECTION_COMPLETE) {
        // prep for a dump
        current_state = DATA_DUMP;
        SPDR = READY;
        dump_end_index = sample_index - 1;
        dump_index = sample_index;
      } else {
        SPDR = NOT_READY;
      }
    }
    prev_pin2 = pin2;
  }
}
void ss_fall () {
  if (current_state == DATA_COLLECTION_COMPLETE) {
    SPDR = ERROR;
  } else {
    SPDR = EOD;
    current_state = DATA_COLLECTION_COMPLETE;
  }
}

void setup() {
  //MISO and MOSI are stay the same in Slave mode, in other words connect straight across not cross over
  pinMode(MISO, OUTPUT);
  
  // set the SPI interface to slave mode
  SPCR |= _BV(SPE);
  
  // enable SPI interrupts
  SPCR |= _BV(SPIE);
  
  //interupt 0 on pin 2, pin 2 connected to pin 10
  attachInterrupt(0, ss_change, CHANGE);
}

ISR (SPI_STC_vect) {
  if (dump_index == dump_end_index) {
    SPDR = EOD;
  } else  {
    SPDR = samples[dump_index++];
    // cut off any bits at 1024 or above to cause roll over
    dump_index &= 0x3FF;
  }
}

void loop() {
  // Only do stuff if not dumping data
  if (current_state != DATA_DUMP) {
    unsigned long current_time = micros();
    if (current_time > next_sample) {
      next_sample += sample_interval;
      unsigned int sample = analogRead(0);
      //This part needs to be atomic or assumption on the indexes may be broken.
      noInterrupts();
      samples[sample_index++] = (byte) (sample >> 8);
      if (sample_index == 1023) {
        current_state = DATA_COLLECTION_COMPLETE;
      }
      samples[sample_index++] = (byte) sample;
      // cut off any bits at 1024 or above to cause roll over
      sample_index &= 0x3FF;
      interrupts();

    }
  }  
}
