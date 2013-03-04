#include "../lib/es_SPIDEV.hpp"

int main(int argc, char *argv[]){
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.1");
    unsigned char tx_buf[2];
    tx_buf[0] = 0x1F;
    tx_buf[1] = 0xFF;
    interface.transmit(tx_buf, true);
    return 0;
}
