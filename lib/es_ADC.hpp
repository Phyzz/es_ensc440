#ifndef es_ADC_H
#define es_ADC_H

#include "es_SPIDEV.hpp"

#include <string>

using std::string;

enum ADCChannelEnum { CH_0, CH_1, CH_2, CH_3 };

class es_ADC {

    public:
        es_ADC(string device_path);
        
        int takeReading(ADCChannelEnum channel, bool differential_reading);
        
        ~es_ADC();
        
    private:
        es_SPIDEV interface;
};

#endif
