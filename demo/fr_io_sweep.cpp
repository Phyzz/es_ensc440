#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"
#include <iostream>

int main(int argc, char *argv[]){
    
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0");
    
    es_DAC dac = es_DAC("/dev/spidev0.1");
    //turn off channel b
    dac.setChannelLevel(CH_B, 0, false, true);
    
    int last_freq = 0 ;
    for (int level = 380; level < 580; level += 1) {
        dac.setChannelLevel(CH_A, (int) level, false, false);
    
        sleep(1);
    
        sampler.takeSample();
        int highest_freq = sampler.getStrongestFreq();
        if(highest_freq > last_freq) {
            std::cout << level - 1 << " : " << last_freq << "\t";
            std::cout << level << " : " << highest_freq << std::endl;
        } else if (highest_freq < last_freq) {
            std::cout << "Reversion at " << level " : " << highest_freq << std::endl;
        }
        last_freq = highest_freq;
    }
    return 0;
}
