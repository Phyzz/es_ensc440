#ifndef ES_FFT_SAMPLER
#define ES_FFT_SAMPLER

#include "es_SPI_EX.hpp"
#include "es_SPIDEV.hpp"
#include "ffft/FFTReal.h"
#include "ffft/FFTRealFixLen.h"
#include <math.h>
#include <vector>

#define SAMPLING_FREQ 84210.52631578947
#define SAMPLE_NUMBER 512
#define THRESHOLD_MULTIPLIER 15
#define FREQ_PER_BIN (SAMPLING_FREQ / (SAMPLE_NUMBER/2))

struct FreqPower {
    int frequency;
    int power;
};

class es_FFTSampler {

    public:
        es_FFTSampler(string device_path, int min_bin = 1, int max_bin = (SAMPLE_NUMBER/4 -1));
        
        void takeSample();
        int getStrongestFreq();
        int getStrongestFreqPower();
        std::vector<FreqPower> getFreqsAboveThresh();
        int getAveragePower();
        
        ~es_FFTSampler();
        
    private:
        const int min_bin;
        const int max_bin;
        es_SPIDEV interface;
        ffft::FFTRealFixLen<8> *fft_object;
        unsigned char rx_buf[SAMPLE_NUMBER+1];
        unsigned int highest_freq;
        float highest_freq_power;
        float average_power;
        std::vector<FreqPower> freqs_above_thresh;
};

#endif
