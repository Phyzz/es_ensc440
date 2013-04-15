#include "es_FFTSampler.hpp"

es_FFTSampler::es_FFTSampler(string device_path) : interface(device_path, 1, 970000) {
}

void es_FFTSampler::takeSample() {
    this->freqs_above_thresh.clear();

    interface.recieve(rx_buf, SAMPLE_NUMBER+1, true);
    float samples[SAMPLE_NUMBER/2];
    for(int i = 0; i < SAMPLE_NUMBER/2; ++i) {
        unsigned int result = 0;
        result = (rx_buf[i*2 + 2] << 8);
        result |= rx_buf[i*2 + 1];
        samples[i] = (float) result;
    }
    ffft::FFTReal <float> fft_object (SAMPLE_NUMBER);
    fft_object.do_fft(this->fft_results, samples);
    
    unsigned int highest_index = 0;
    float highest_result = 0.0;
    this->average_power = 0;
    for (int i = 1; i < SAMPLE_NUMBER/4-1; ++i) {
        float result = sqrt(this->fft_results[i] * this->fft_results[i] + this->fft_results[SAMPLE_NUMBER/4+i] * this->fft_results[SAMPLE_NUMBER/4+i]);
        this->average_power += result;
        if (i >= MIN_BIN && i <= MAX_BIN) {
            if (result > THRESHOLD) {
                this->freqs_above_thresh.push_back({(int) (highest_index*FREQ_PER_BIN + 0.5), (int) result});
            }
            if (result > highest_result) {
                highest_result = result;
                highest_index = i;
            }
        }
    }
    this->average_power = average_power / (SAMPLE_NUMBER/4-1);
    this->highest_freq = (int) (highest_index*FREQ_PER_BIN + 0.5);
}
int es_FFTSampler::getStrongestFreq() {
    return this->highest_freq;
}

int es_FFTSampler::getStrongestFreqPower() {
    return (int) this->highest_freq_power;
}

std::vector<FreqPower> es_FFTSampler::getFreqsAboveThresh() {
    return this->freqs_above_thresh;
}

int es_FFTSampler::getAveragePower() {
    return (int) this->average_power;
}

es_FFTSampler::~es_FFTSampler() {
}

