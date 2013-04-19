#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"
#include <iostream>
#include <time.h>
#include <map>
#include <vector>
#include <set>

int main(int argc, char *argv[]){
    
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 114, 127);
    
    es_DAC dac = es_DAC("/dev/spidev0.1");
    //turn off channel b
    dac.setChannelLevel(CH_B, 0, false, true);
    
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 6000000;
    
    std::set<int> frequencies;
    std::map<int, std::vector<std::vector<int> > > raw_vals;
    
    std::map<int, int> set_vals;
    
    sleep(1);
    for (int i = 0; i < 20; ++i) {
        int last_freq = 0 ;
        for (int level = 330; level < 520; ++level) {
            dac.setChannelLevel(CH_A, (int) level, false, false);
        
            clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
        
            sampler.takeSample();
            int highest_freq = sampler.getStrongestFreq();
            if(highest_freq > last_freq) {
                frequencies.insert(last_freq);
                while (raw_vals[last_freq].size() <= i) {
                    raw_vals[last_freq].push_back({0,0});
                }
                frequencies.insert(highest_freq);
                while (raw_vals[highest_freq].size() <= i) {
                    raw_vals[highest_freq].push_back({0,0});
                }
                raw_vals[last_freq][i][1] = level-1;
                raw_vals[highest_freq][i][0] = level;
            }
            last_freq = highest_freq;
        }
    }
    
    for(std::set<int>::iterator it = frequencies.lower_bound(38000); it != frequencies.lower_bound(40500); ++it) {
        int frequency = *it;
        float lower_bound = 0;
        float upper_bound = 0;
        for(std::vector<std::vector<int> >::iterator in_it = raw_vals[frequency].begin(); in_it != raw_vals[frequency].end(); ++in_it) {
            lower_bound += (*in_it)[0];
            upper_bound += (*in_it)[1];
        }
        lower_bound = lower_bound / raw_vals[frequency].size();
        upper_bound = upper_bound / raw_vals[frequency].size();
        float set = (lower_bound + upper_bound) / 2;
        set_vals[frequency] = (int) (set + 0.5);
        std::cout << "Frequency " << frequency << " has setpoint " << set_vals[frequency] << std::endl;
   }
    return 0;
}
