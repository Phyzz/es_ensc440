#include <iostream>
#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"
#include "../lib/ffft/FFTReal.h"

int main(int argc, char *argv[]){
    unsigned char rx_buf[1032];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0", 1);
    interface.recieve(rx_buf, 1032, true);
    
    unsigned int results[516];
    for(int i = 0; i < 516; ++i) {
        results[i] = (rx_buf[i*2 + 1] << 8);
        results[i] |= rx_buf[i*2 + 2];
    }
    
    for (int i = 0; i < 512; i += 16) {
        for (int j = 0; j < 16 && i +j < 515; ++j) {
            std::cout << std::hex << results[i + j] << "\t";
        }
        std::cout << std::endl;
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
        float result = results[i] * results[i] + results[256+i] * results[256*i];
        if (result > highest_result) {
            highest_result = result;
            highest_index = i;
        }
    }
    
    std::cout << std::endl << "Highest frequency is " << 1.953125f * highest_index << std::endl;
    
    return 0;
}
