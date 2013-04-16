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

volatile byte samples[512] = {}; //each sample is 10 bits, with no packing 2 bytes/sample so this is 256 samples
volatile unsigned int sample_index;

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
      
      // re-enable data collection timer
      sbi(TIMSK1,OCIE1A);
    // falling edge
    } else {
        dumping = true;
        SPDR = READY;
        dump_index = sample_index;
        dumped_bytes = 0;
        
        //disable data collection timer
        cbi(TIMSK1,OCIE1A);
    }

  }
  prev_pin2 = pin2;
}
void setup() {
  cli();
  
  // ADC setup
  // set ADC prescaler to 4 for max sample rate of 308 kHz
  cbi(ADCSRA,ADPS2);
  sbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
  
  // Disable ADC conversion interrupt
  cbi(ADCSRA,ADIE);
  // clear auto-trigger/freerunning
  cbi(ADCSRA, ADATE);
  
  //set 5V internal ref
  cbi(ADMUX,REFS1);
  sbi(ADMUX,REFS0);
  
  //set right aligned data
  cbi(ADMUX,ADLAR);
  
  //choose ADC pin 0;
  cbi(ADMUX,MUX3);
  cbi(ADMUX,MUX2);
  cbi(ADMUX,MUX1);
  cbi(ADMUX,MUX0);
  
  //SPI setup
  //MISO and MOSI stay the same in Slave mode, in other words connect straight across not cross over
  pinMode(MISO, OUTPUT);
  
  SPCR = 0;
  // enable the SPI interface in slave mode
  SPCR |= _BV(SPE);
  
  // enable SPI interrupts
  SPCR |= _BV(SPIE);


  // set up the sample timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  
  //sampling at approx 84,210.5 Hz
  OCR1A = 190;
  
  sbi(TCCR1B,WGM12);
  sbi(TCCR1B,CS10);
  
  sbi(TIMSK1,OCIE1A);
  
  //interupt 0 on pin 2, pin 2 must be externally connected to SS pin 10
  attachInterrupt(0, ss_change, CHANGE);


  // initialize all constants
  sample_index = 0;
  
  dump_index = 0;
  dumped_bytes = 0;
  
  dumping = false;
//  ready_to_dump = false;
  prev_pin2 = 1;
  
  sei();
}

ISR (SPI_STC_vect) {
  if (dumped_bytes == 512) {
    SPDR = EOD;
  } else  {
    SPDR = samples[dump_index++];
    ++dumped_bytes;
    // cut off any bits over 512 to cause roll over
    dump_index &= 0x1FF;
  }
}

ISR(TIMER1_COMPA_vect) {

  //start an ADC conversion
  sbi(ADCSRA, ADSC);
  //busy wait until the conversion is done
  while(bit_is_set(ADCSRA, ADSC));
  
  samples[sample_index++] = ADCL;
  samples[sample_index++] = ADCH;

  // cut off any bits at 512 or above to cause roll over
  sample_index &= 0x1FF;
}

void loop() {
}
