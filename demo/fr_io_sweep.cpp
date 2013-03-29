#include "../lib/es_DAC.hpp"
#include "../lib/es_ADC.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>

#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"
#include "../lib/ffft/FFTReal.h"

#define SAMPLING_FREQ 100000.0

int main(int argc, char *argv[]){

    float freq_per_bin = SAMPLING_FREQ / 512.0; 
    unsigned char rx_buf[1025];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0", 1, 970000);
    
    es_DAC dac = es_DAC("/dev/spidev0.1");
    //turn off channel b
    dac.setChannelLevel(CH_B, 0, false, true);
    
    float b = 3080;
    float m = 22.4;
    for (int desired_freq = 3250; desired_freq < 26000; desired_freq += 250) {
        float level = (desired_freq-b)/m;
        dac.setChannelLevel(CH_A, (int) level, true, false);
    
        sleep(1);
    
        interface.recieve(rx_buf, 1025, true);
        
        unsigned int results[512];
        for(int i = 0; i < 512; ++i) {
            results[i] = (rx_buf[i*2 + 2] << 8);
            results[i] |= rx_buf[i*2 + 1];
        }
        
        float samples[512];
        for (int i = 0; i < 512; ++i) {
            samples[i] = (float) results[i];
        }
        float fft_results[512];
        ffft::FFTReal <float> fft_object (512);
        fft_object.do_fft(fft_results, samples);
        
        int highest_index = 0;
        float highest_result = 0.0;
        for (int i = 1; i < 255; ++i) {
            float result = fft_results[i] * fft_results[i] + fft_results[256+i] * fft_results[256+i];
            if (result > highest_result) {
                highest_result = result;
                highest_index = i;
            }
        }
     
        float highest_freq = freq_per_bin * highest_index;
        float diff = highest_freq - desired_freq;
        std::cout << "Desired Frequency: " << desired_freq << "\t";
        std::cout << "Recieved Frequency: " << highest_freq << "\t";
        std::cout << "Difference: " << diff << std::endl;
        sleep(2);
    }
    return 0;
}
