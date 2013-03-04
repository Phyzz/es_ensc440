#ifndef es_DAC_H
#define es_DAC_H

#include "es_SPIDEV.hpp"

#include <string>

using std::string;

enum ChannelEnum { CH_A, CH_B };

class es_DAC {

    public:
        es_DAC(string device_path);
        
        void setChannelLevel(ChannelEnum channel, unsigned int level, bool gain, bool shutdwon);
        
        ~es_DAC();
        
    private:
        es_SPIDEV interface;
};

#endif
