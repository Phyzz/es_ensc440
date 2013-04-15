#include <iostream>
#include "../lib/es_FFTSampler.hpp"

int main(int argc, char *argv[]){
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0");
    
    sleep(1);
    
    sampler.takeSample();
    std::vector<FreqPower> above_thresh = sampler.getFreqsAboveThresh();
    std::cout << "Highest frequency is " << sampler.getStrongestFreq();
    std::cout << " at power level " << sampler.getStrongestFreqPower();
    std::cout << " against an average power level of " << sampler.getAveragePower();
    std::cout << " with  " << above_thresh.size() << " over 30,000" << std::endl;
    for (std::vector<FreqPower>::iterator it = above_thresh.begin(); it != above_thresh.end() ; ++it) {
        std::cout << it->frequency << " : " << it->power << std::endl;
    }
    return 0;
}
