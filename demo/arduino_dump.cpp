#include <iostream>
#include "../lib/es_SPI_EX.hpp"
#include "../lib/es_SPIDEV.hpp"
#include "../lib/ffft/FFTReal.h"

#define SAMPLING_FREQ 84210.52631578947

int main(int argc, char *argv[]){
    unsigned char rx_buf[1032];
    
    es_SPIDEV interface = es_SPIDEV("/dev/spidev0.0", 1, 970000);
    
    sleep(1);
    
    interface.recieve(rx_buf, 1032, true);
    
    std::cout << std::hex << (int) rx_buf[0] << std::endl;
    std::cout << std::hex << (int) rx_buf[1] << std::endl;
    std::cout << std::hex << (int) rx_buf[2] << std::endl;
    std::cout << "****" << std::endl;
    std::cout << std::hex << (int) rx_buf[1024] << std::endl;
    std::cout << std::hex << (int) rx_buf[1025] << std::endl;
    std::cout << std::hex << (int) rx_buf[1026] << std::endl;
    std::cout << std::hex << (int) rx_buf[1027] << std::endl;
    std::cout << std::hex << (int) rx_buf[1028] << std::endl;
    std::cout << std::hex << (int) rx_buf[1029] << std::endl;
    std::cout << std::hex << (int) rx_buf[1031] << std::endl;
    std::cout << std::hex << (int) rx_buf[1032] << std::endl;
    
    unsigned int results[516];
    for(int i = 0; i < 516; ++i) {
        results[i] = (rx_buf[i*2 + 2] << 8);
        results[i] |= rx_buf[i*2 + 1];
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
        float result = fft_results[i] * fft_results[i] + fft_results[256+i] * fft_results[256+i];
        if (result > highest_result) {
            highest_result = result;
            highest_index = i;
        }
    }
 
    float freq_per_bin = SAMPLING_FREQ / 512.0; 
    std::cout << std::endl << "Highest frequency is " << freq_per_bin * highest_index << " at power level " << highest_result << std::endl;
    
    return 0;
}
