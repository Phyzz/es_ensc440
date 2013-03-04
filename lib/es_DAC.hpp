#ifndef es_DAC_H
#define es_DAC_H

#include "es_SPIDEV.hpp"

#include <string>

using std::string;

enum DACChannelEnum { CH_A, CH_B };

class es_DAC {

    public:
        es_DAC(string device_path);
        
        void setChannelLevel(DACChannelEnum channel, unsigned int level, bool gain, bool shutdwon);
        
        ~es_DAC();
        
    private:
        es_SPIDEV interface;
};

#endif
