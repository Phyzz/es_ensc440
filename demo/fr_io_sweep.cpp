#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"
#include <iostream>
#include <time.h>

int main(int argc, char *argv[]){
    
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 114, 125);
    
    es_DAC dac = es_DAC("/dev/spidev0.1");
    //turn off channel b
    dac.setChannelLevel(CH_B, 0, false, true);
    
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 3080000;
    
    sleep(1);
    int last_freq = 0 ;
    for (int level = 330; level < 510; ++level) {
        dac.setChannelLevel(CH_A, (int) level, false, false);
    
        clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
    
        sampler.takeSample();
        int highest_freq = sampler.getStrongestFreq();
        if(highest_freq > last_freq) {
            std::cout << level - 1 << " : " << last_freq << "\t";
            std::cout << level << " : " << highest_freq << std::endl;
        } else if (highest_freq < last_freq) {
            std::cout << "Reversion at " << level << " : " << highest_freq << std::endl;
        }
        last_freq = highest_freq;
    }
    return 0;
}
