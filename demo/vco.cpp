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

    return 0;
}
