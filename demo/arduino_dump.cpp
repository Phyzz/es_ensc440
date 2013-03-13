#include <iostream>
#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"

int main(int argc, char *argv[]){
    unsigned char tx_buf[] = { 0xFA, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
                               0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
                               0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5,
                               0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5, 0xF5
                             };
    unsigned char rx_buf[32];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0");
    interface.full_duplex(tx_buf, rx_buf, 32, true);
    for (int i = 0; i < 32; ++i) {
        std.cout << rx_buf[i];
    }
    return 0;
}
