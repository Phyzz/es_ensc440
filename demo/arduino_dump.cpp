#include <iostream>
#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"

int main(int argc, char *argv[]){
    unsigned char rx_buf[1032];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0");
    interface.recieve(rx_buf, 32, true);
    
    unsigned int results[516];
    for(int i = 0; i < 516; ++i) {
        results[i] = (rx_buf[i*2] << 8);
        results[i] |= rx_buf[i*2 + 1];
    }
    for (int i = 0; i < 512; i += 4) {
        for (int j = 0; j < 4; ++j) {
            std::cout << std::hex << results[i + j] << "\t";
        }
        std::cout << std::endl;
    }
    return 0;
}
