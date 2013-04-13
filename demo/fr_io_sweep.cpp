#include "../lib/es_DAC.hpp"
#include "../lib/es_ADC.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"
#include "../lib/ffft/FFTReal.h"

#define SAMPLING_FREQ 84210.52631578947
#define SAMPLE_NUMBER 1024

int main(int argc, char *argv[]){

    float freq_per_bin = SAMPLING_FREQ / (SAMPLE_NUMBER/2); 
    unsigned char rx_buf[SAMPLE_NUMBER+1];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0", 1, 970000);
    
    es_DAC dac = es_DAC("/dev/spidev0.1");
    //turn off channel b
    dac.setChannelLevel(CH_B, 0, false, true);
    
    for (int level = 400; level < 600; level += 4) {
        dac.setChannelLevel(CH_A, (int) level, false, false);
    
        sleep(1);
    
        interface.recieve(rx_buf, SAMPLE_NUMBER+1, true);
        
        unsigned int results[SAMPLE_NUMBER/2];
        for(int i = 0; i < SAMPLE_NUMBER/2; ++i) {
            results[i] = (rx_buf[i*2 + 2] << 8);
            results[i] |= rx_buf[i*2 + 1];
        }
        
        float samples[SAMPLE_NUMBER/2];
        for (int i = 0; i < SAMPLE_NUMBER; ++i) {
            samples[i] = (float) results[i];
        }
        float fft_results[SAMPLE_NUMBER];
        ffft::FFTReal <float> fft_object (SAMPLE_NUMBER);
        fft_object.do_fft(fft_results, samples);
        
        int highest_index = 0;
        float highest_result = 0.0;
        for (int i = 1; i < SAMPLE_NUMBER/4-1; ++i) {
            float result = fft_results[i] * fft_results[i] + fft_results[SAMPLE_NUMBER/4+i] * fft_results[SAMPLE_NUMBER/4+i];
            if (result > highest_result) {
                highest_result = result;
                highest_index = i;
            }
        }
     
        float highest_freq = freq_per_bin * highest_index;
        std::cout << "Level: " << level << "\t";
        std::cout << "Recieved Frequency: " << highest_freq << std::endl;
    }
    return 0;
}
