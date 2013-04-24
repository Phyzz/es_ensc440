#ifndef es_CAL_H
#define es_CAL_H

#include "es_SPIDEV.hpp"
#include "es_DAC.hpp"
#include "es_FFTSampler.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <time.h>
#include <map>
#include <vector>
#include <set>

class es_Calibrator {

    public:
        es_Calibrator(es_DAC* dac, es_FFTSampler* sampler, pthread_mutex_t* dac_mutex = NULL, pthread_mutex_t* sampler_mutex = NULL );
        
        std::map<int, int> doCalibration();
        void loadCachedSetVals();
        void saveCachedSetVals();
        bool testCachedSetVals();
        
        ~es_Calibrator();
        
    private:
        es_DAC* dac;
        es_FFTSampler* sampler;
        pthread_mutex_t* dac_mutex;
        bool created_dac_mutex;
        pthread_mutex_t* sampler_mutex;
        bool created_sampler_mutex;
        std::map<int, int> set_val_cache;
};

#endif
