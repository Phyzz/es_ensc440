#include <iostream>
#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"

int main(int argc, char *argv[]){
    unsigned char rx_buf[1032];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0", 1);
    interface.recieve(rx_buf, 1032, true);
    
    unsigned int results[516];
    for(int i = 0; i < 516; ++i) {
        results[i] = (rx_buf[i*2 + 1] << 8);
        results[i] |= rx_buf[i*2 + 2];
    }
    for (int i = 0; i < 512; i += 16) {
        for (int j = 0; j < 16 && i +j < 515; ++j) {
            std::cout << std::hex << results[i + j] << "\t";
        }
        std::cout << std::endl;
    }
    return 0;
}
