#include "es_ADC.hpp"
#include "es_SPI_EX.hpp"

es_ADC::es_ADC(string device_path) : interface(device_path) {
}
        
int es_ADC::takeReading(ADCChannelEnum channel, bool differential_reading) {
    unsigned char tx_buffer[1];
    if (differential_reading) {
        tx_buffer[0] = 0x20;
    } else {
        tx_buffer[0] = 0x30;
    }
    if (channel == CH_0) {
        tx_buffer[0] |= 0x00;
    } else if (channel == CH_1) {
        tx_buffer[0] |= 0x02;
    } else if (channel == CH_2) {
        tx_buffer[0] |= 0x04;
    } else if (channel == CH_3) {
        tx_buffer[0] |= 0x06;
    }
    
    unsigned char rx_buffer[2];
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    
    this->interface.half_duplex(tx_buffer, 1, rx_buffer, 2, false, true);
    if (rx_buffer[0] & 0x80) {
        throw es_SPI_EX_TRANSFER();
    }
    
    int reading = rx_buffer[0];
    reading <<= 3;
    rx_buffer[1] >>= 5;
    reading |= rx_buffer[1];
    
    return reading;
}
        
es_ADC::~es_ADC(){
}

