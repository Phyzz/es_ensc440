#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"

#define BEACON0 0
#define BEACON1 1
#define BEACON2 2
#define BEACON3 3

pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;

es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 116, 123);
es_DAC dac = es_DAC("/dev/spidev0.1");

std::map<int, std::vector<int> > transmit_map = {
    {BEACON0, {363, 380}},
    {BEACON1, {396, 412}},
    {BEACON2, {429, 445}},
    {BEACON3, {461, 478}},
};

std::map<int, std::vector<int> > reciever_map = {
    {38158, {BEACON0, 0}},
    {38487, {BEACON0, 1}},
    {38816, {BEACON1, 0}},
    {39145, {BEACON1, 1}},
    {39474, {BEACON2, 0}},
    {39803, {BEACON2, 1}},
    {40132, {BEACON3, 0}},
    {40461, {BEACON3, 1}},
};

void send_message(int beacon, unsigned short bit) {
    assert(bit <= 1);
    assert(beacon < 4);
    dac.setChannelLevel(CH_A, transmit_map[beacon][bit], false, false);
}

std::vector<std::vector<int> > recieve_message() {
    sampler.takeSample();
    std::vector<FreqPower> freqs = sampler.getFreqsAboveThresh();
    std::vector<std::vector<int> > ret;
    for (std::vector<FreqPower>::iterator it = freqs.begin(); it != freqs.end() ; ++it) {
        ret.push_back(reciever_map[it->frequency]);
    }
    return ret;
}

void * reciever_fcn(void *ptr) {
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 3040000;
    
    while(1){
        clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
        std::vector<std::vector<int> > message = recieve_message();
        for (std::vector<std::vector<int> >::iterator it = message.begin(); it != message.end() ; ++it) {
            pthread_mutex_lock ( &cout_mutex );
            std::cout << "Recieved " << (*it)[1] << " from beacon " << (*it)[0] << std::endl;
            pthread_mutex_unlock ( &cout_mutex );    
        }
    }
}

int main(int argc, char *argv[]){
    
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 3040000;
    
    pthread_t thread2;
    pthread_create(&thread2, NULL, &reciever_fcn, NULL);
    
    for (int i = BEACON0; i <= BEACON3; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            pthread_mutex_lock ( &cout_mutex );
            std::cout << std::endl << "Sending " << j << " from beacon " << i << " using level " << transmit_map[i][j] << std::endl;
            pthread_mutex_unlock ( &cout_mutex );
            send_message(i, j);
            clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
            
        }
    }
    
    return 0;
}
