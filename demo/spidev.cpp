#include "../lib/es_DAC.hpp"

int main(int argc, char *argv[]){
    es_DAC dac = es_DAC("/dev/spidev0.1");
    dac.setChannelLevel(CH_A, 0x3FF, false, false);
    sleep(5);
    dac.setChannelLevel(CH_A, 0x000, false, false);
    return 0;
}
