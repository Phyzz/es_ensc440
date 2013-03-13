#include <SPI.h>
#include "pins_arduino.h"
int SS_PIN =

#define START_TX      0xFA
#define CONTINUE_TX   0xF5
#define EOD           0xFF //end of data
#define ERROR         0xFE
#define NOT_READY     0xFD
#define READY         0xFC

int samples[10] = {0, 1, 2, 3, 4, 5, 6, 7, 63 , 9};
int sample_index = 0;

int dump_index = 0;
boolean high_byte = true;

enum State {DATA_COLLECTION_INCOMPLETE, DATA_COLLECTION_COMPLETE, DATA_DUMP, DUMP_COMPLETE, ERROR_STATE};

State current_state = DATA_COLLECTION_COMPLETE;

// Reset everything when an SPI communication finishes
void ss_rise () {
  current_state = DATA_COLLECTION_COMPLETE;
  dump_index = 0;
  high_byte = true;
}

void setup() {
  //MISO and MOSI are stay the same in Slave mode, in other words connect straight across not cross over
  pinMode(MISO, OUTPUT);
  
  // set the SPI interface to slave mode
  SPCR |= _BV(SPE);
  
  // enable SPI interrupts
  SPCR |= _BV(SPIE);
  
  //interupt 0 on pin 2
  attachInterrups(0, ss_rise, RISING);
}

ISR (SPI_STC_vect) {
  byte recieved_byte = SPDR;
  
  if (current_state == DATA_COLLECTION_INCOMPLETE) {
    if (recieved_byte == START_TX) {
      SPDR = NOT_READY;
    } else {
      current_state = ERROR_STATE;
      SPDR = ERROR;
    }
  } else if (current_state == DATA_COLLECTION_COMPLETE) {
    if (recieved_byte == START_TX) {
      SPDR = READY;
      current_state = DATA_DUMP;
    } else {
      current_state = ERROR_STATE;
      SPDR = ERROR;
    }
  } else if (current_state == DATA_DUMP) {
    if (recieved_byte == CONTINUE_TX) {
      if(dump_index >= 10) {
        current_state = DUMP_COMPLETE;
        SPDR = EOD;
      } else {
        if (high_byte) {
          SPDR = (byte) (samples[dump_index] >> 8);
        } else {
          SPDR = (byte) (samples[dump_index++]);
        }
        high_byte = !high_byte;
      }
    } else {
      current_state = ERROR_STATE;
      SPDR = ERROR;
    }
  } else if (current_state == DUMP_COMPLETE) {
    if (recieved_byte = CONTINUE_TX) {
      SPDR = EOD;
    } else {
      current_state = ERROR_STATE;
      SPDR = ERROR;
    }
    
  } else if (current_state == ERROR) {
      SPDR = ERROR;
  } else {
      current_state = ERROR_STATE;
      SPDR = ERROR;
  }
}

void loop() {
}
