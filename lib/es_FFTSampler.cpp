#include "es_FFTSampler.hpp"

es_FFTSampler::es_FFTSampler(string device_path, int min_bin, int max_bin) : interface(device_path, 1, 970000), min_bin(min_bin), max_bin(max_bin) {
    this->fft_object = new ffft::FFTRealFixLen <8>;
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
    float fft_results[SAMPLE_NUMBER/2];
    this->fft_object->do_fft(fft_results, samples);
    
    unsigned int highest_index = 0;
    float highest_result = 0.0;
    this->average_power = 0;
    for (int i = 1; i < SAMPLE_NUMBER/4-1; ++i) {
        float result = sqrt(fft_results[i] * fft_results[i] + fft_results[SAMPLE_NUMBER/4+i] * fft_results[SAMPLE_NUMBER/4+i]);
        this->average_power += result;
        if (i >= this->min_bin && i <= this->max_bin) {
            if (result > highest_result) {
                highest_result = result;
                highest_index = i;
            }
        }
    }
    this->average_power = average_power / (SAMPLE_NUMBER/4-1);
    int threshold = (int) (average_power * THRESHOLD_MULTIPLIER);
    for (int i = this->min_bin; i < this->max_bin; ++i) {
        float result = sqrt(fft_results[i] * fft_results[i] + fft_results[SAMPLE_NUMBER/4+i] * fft_results[SAMPLE_NUMBER/4+i]);
        if (result > threshold) {
            this->freqs_above_thresh.push_back({(int) (i*FREQ_PER_BIN + 0.5), (int) result});
        }
    }
    
    this->highest_freq = (int) (highest_index*FREQ_PER_BIN + 0.5);
    this->highest_freq_power = highest_result;
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
    delete this->fft_object;
}

