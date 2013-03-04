#include "es_ADC.hpp"
#include "es_EX.hpp"

es_ADC::es_ADC(string device_path) : interface(device_path) {
}
        
int es_ADC::takeReading(ADCChannelEnum channel, bool differential_reading) {
    unsigned char tx_buffer[1];
    if (differential_reading) {
        tx_buffer[0] = 0x10;
    } else {
        tx_buffer[0] = 0x18;
    }
    if (channel == CH_0) {
        tx_buffer[0] |= 0x00;
    } else if (channel == CH_1) {
        tx_buffer[0] |= 0x01;
    } else if (channel == CH_2) {
        tx_buffer[0] |= 0x02;
    } else if (channel == CH_3) {
        tx_buffer[0] |= 0x03;
    }
    
    unsigned char rx_buffer[2];
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    
    this->interface.half_duplex(tx_buffer, 1, rx_buffer, 2, false, true);
    
    int reading = 0;
    reading |= rx_buffer[0];
    reading &= 0x7F;
    reading <<= 4;
    rx_buffer[1] >>= 4;
    reading |= rx_buffer[1];
    
    return reading;
}
        
es_ADC::~es_ADC(){
}

