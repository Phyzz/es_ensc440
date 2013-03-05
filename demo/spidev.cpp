#include "../lib/es_DAC.hpp"
#include "../lib/es_ADC.hpp"
#include <iostream>

int main(int argc, char *argv[]){
    es_DAC dac = es_DAC("/dev/spidev0.1");
    es_ADC adc = es_ADC("/dev/spidev0.0");
    dac.setChannelLevel(CH_A, 0x3FF, false, false);
    dac.setChannelLevel(CH_B, 0x3FF, false, false);
    int ch0 = adc.takeReading(CH_0, false);
    int ch1 = adc.takeReading(CH_1, false);
    std::cout << "CH_0 reading: " << std::hex << ch0 << "\n";
    std::cout << "CH_1 reading: " << std::hex << ch1 << "\n";
    sleep(5);
    dac.setChannelLevel(CH_A, 0x000, false, false);
    dac.setChannelLevel(CH_B, 0x000, false, false);
    ch0 = adc.takeReading(CH_0, false);
    ch1 = adc.takeReading(CH_1, false);
    std::cout << "CH_0 reading: " << std::hex << ch0 << "\n";
    std::cout << "CH_1 reading: " << std::hex << ch1 << "\n";
    
    for (int i = 0; i < 0x3FF; i += 10) {
        dac.setChannelLevel(CH_B, i, false, false);
        int ch0 = adc.takeReading(CH_1, false);
        float ratio = ch0 / (float) i;
        std::cout << "Input: " << std::hex << i << "\t" << "Output: " << std::hex << ch0 << "\t" << "Ratio: " << std::defaultfloat << ratio << "\n";
        sleep(5);
    }
    return 0;
}
