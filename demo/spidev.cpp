#include "../lib/es_SPIDEV.hpp"

int main(int argc, char *argv[]){
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0");
    unsigned char tx_buf[1];
    tx_buf[0] = 0xFF;
    interface.transmit(tx_buf, true);
    return 0;
}
