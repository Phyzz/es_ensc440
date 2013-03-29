#include "../lib/es_DAC.hpp"
#include "../lib/es_ADC.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

int main(int argc, char *argv[]){
    
    int level;
    if (argc > 1) {
        level = atoi(argv[1]);
    } else {
        level = 0;
    }
    
    if (level < 0 || level > 0x3FF) {
        std::cout << "Out of Range: 0 - 0x3FF" << std::endl;
        return 1;
    }

    es_DAC dac = es_DAC("/dev/spidev0.1");
    dac.setChannelLevel(CH_A, level, false, false);
    
    
    std::string input = "";
    do {
        std::cin >> input;
        level = atoi(input.c_str());
        std::cout << level << std::endl;
        if (level >= 0 && level <= 0x3FF) {
            dac.setChannelLevel(CH_A, level, false, false);
        }
    } while( input != "exit" );
    
 /*   for (int i = 0; i < 0x3FF; i += 10) {
        dac.setChannelLevel(CH_B, i, false, false);
        int ch0 = adc.takeReading(CH_1, false);
        float ratio = ch0 / (float) i;
        std::cout << "Input: " << std::hex << i << "\t" << "Output: " << std::hex << ch0 << "\t";
        std::cout << "Ratio: " << ratio << "\n";
        sleep(5);
    }*/
    return 0;
}
