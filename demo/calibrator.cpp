#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"
#include "../lib/es_Cal.hpp"
#include <iostream>
#include <time.h>
#include <map>
#include <vector>
#include <set>
#include <iostream>

int main(int argc, char *argv[]){
    
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 114, 127);
    
    es_DAC dac = es_DAC("/dev/spidev0.1");
    
    es_Calibrator calibrator = es_Calibrator(&dac, &sampler);
    std::map<int,int> set_vals = calibrator.doCalibration();
std::cout << "saving" << std::endl;
    calibrator.saveCachedSetVals();
    calibrator.loadCachedSetVals();
    
    for(std::map<int, int>::iterator it = set_vals.begin(); it != set_vals.end(); ++it) {
        std::cout << "Frequency " << it->first << " has setpoint " << it->second << std::endl;
   }
    return 0;
    return 0;
}
