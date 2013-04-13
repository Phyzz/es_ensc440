#include <iostream>
#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"
#include "../lib/ffft/FFTReal.h"

#define SAMPLING_FREQ 84210.52631578947
#define SAMPLE_NUMBER 1024

int main(int argc, char *argv[]){
    unsigned char rx_buf[SAMPLE_NUMBER+4];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0", 1, 970000);
    
    sleep(1);
    
    interface.recieve(rx_buf, SAMPLE_NUMBER+4, true);
    
    std::cout << std::hex << (int) rx_buf[0] << std::endl;
    std::cout << std::hex << (int) rx_buf[1] << std::endl;
    std::cout << std::hex << (int) rx_buf[2] << std::endl;
    std::cout << "****" << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER-4] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER-3] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER-2] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER-1] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER+0] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER+1] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER+2] << std::endl;
    std::cout << std::hex << (int) rx_buf[SAMPLE_NUMBER+3] << std::endl;
    
    unsigned int results[SAMPLE_NUMBER/2];
    for(int i = 0; i < SAMPLE_NUMBER/2; ++i) {
        results[i] = (rx_buf[i*2 + 2] << 8);
        results[i] |= rx_buf[i*2 + 1];
    }
    
    for (int i = 0; i < SAMPLE_NUMBER/2; i += 16) {
        for (int j = 0; j < 16 && i +j < SAMPLE_NUMBER/2; ++j) {
            std::cout << std::hex << results[i + j] << "\t";
        }
        std::cout << std::endl;
    }
    
    float samples[SAMPLE_NUMBER/2];
    for (int i = 0; i < SAMPLE_NUMBER/2; ++i) {
        samples[i] = (float) results[i];
    }
    float fft_results[SAMPLE_NUMBER/2];
    ffft::FFTReal <float> fft_object (SAMPLE_NUMBER/2);
    fft_object.do_fft(fft_results, samples);
    
    int highest_index = 0;
    float highest_result = 0.0;
    for (int i = 1; i < SAMPLE_NUMBER/4-1; ++i) {
        float result = fft_results[i] * fft_results[i] + fft_results[SAMPLE_NUMBER/2+i] * fft_results[SAMPLE_NUMBER/2+i];
        if (result > highest_result) {
            highest_result = result;
            highest_index = i;
        }
    }
 
    float freq_per_bin = SAMPLING_FREQ / (SAMPLE_NUMBER/2); 
    std::cout << std::endl << "Highest frequency is " << freq_per_bin * highest_index << " at power level " << highest_result << std::endl;
    
    return 0;
}
