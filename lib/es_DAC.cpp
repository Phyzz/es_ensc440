#include "es_DAC.hpp"
#include "es_EX.hpp"

es_DAC::es_DAC(string device_path) : interface(device_path, 0, 1000000) {
}
        
void es_DAC::setChannelLevel(DACChannelEnum channel, unsigned int level, bool gain, bool shutdown) {
    if (level > 0x3FF || level < 0) {
        throw es_INVALID_ARGS_EX();
    }
    unsigned char tx_buffer[2];
    if (channel == CH_A) {
        tx_buffer[0] = 0x00;
    } else {
        tx_buffer[0] = 0x80;
    }
    tx_buffer[1] = 0x00;
    if (!gain) {
        tx_buffer[0] |= 0x20;
    }
    if (!shutdown) {
        tx_buffer[0] |= 0x10;
    }
    
    tx_buffer[1] = (unsigned char) level;
    tx_buffer[1] <<= 2;
    level >>= 6;
    tx_buffer[0] |= level;
    
    this->interface.transmit(tx_buffer, 2, true);
}
        
es_DAC::~es_DAC(){
}

