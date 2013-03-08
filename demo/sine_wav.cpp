#include "../lib/es_DAC.hpp"
#include "../lib/es_ADC.hpp"
#include <iostream>
#include <time.h>

int main(int argc, char *argv[]){
    es_DAC dac = es_DAC("/dev/spidev0.1");
    es_ADC adc = es_ADC("/dev/spidev0.0");
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec=100000;
    bool state = false;
    int a = 0x1FF;
    int result;
    
    while(true) {
        if(state) {
            dac.setChannelLevel(CH_A, a, false, false);
        } else {
            dac.setChannelLevel(CH_A, 0, false, false);
        }
        state = !state;
        result = nanosleep(&interval, NULL);
        if(result) {return -1;}
    }
    
    return 0;
}
