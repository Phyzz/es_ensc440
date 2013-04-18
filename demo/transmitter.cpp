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
    {BEACON0, {379, 396}},
    {BEACON1, {414, 431}},
    {BEACON2, {449, 467}},
    {BEACON3, {485, 502}},
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

void add_timespec(timespec* tp, long nanos) {
    long addition = tp->tv_nsec + nanos;
    tp->tv_nsec = addition % 1000000000;
    tp->tv_sec += addition / 1000000000;
}

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
    timespec next_sample;
    clock_gettime(CLOCK_MONOTONIC, &next_sample);
    
    std::vector<std::vector<int> > message;
    while(1){
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_sample, NULL);
        add_timespec(&next_sample, 7800000);
        std::vector<std::vector<int> > bit = recieve_message();
        if(bit.size() == 0) {
            std::string bla;
            char byte = 0;
            int i = 0;
            for (std::vector<std::vector<int> >::iterator it = message.begin(); it != message.end(); ++ it) {
                byte <<= 1;
                byte = (*it)[1];
                
                if( ++i >= 8) {
                    bla.append(&byte);
                    byte = 0;
                    i = 0;
                }
                
                /*pthread_mutex_lock ( &cout_mutex );
                std::cout << "Recieved " << (*it)[1] << " from beacon " << (*it)[0] << std::endl;
                pthread_mutex_unlock ( &cout_mutex );*/
            }
            if(bla.size() != 0) {
                pthread_mutex_lock ( &cout_mutex );
                std::cout << "Recieved " << bla << std::endl;
                pthread_mutex_unlock ( &cout_mutex );
            }
            message.clear();
        } else {
            for (std::vector<std::vector<int> >::iterator it = bit.begin(); it != bit.end() ; ++it) {
                message.push_back(*it);
            }
        }
    }
}

int main(int argc, char *argv[]){  
    pthread_t thread2;
    pthread_create(&thread2, NULL, &reciever_fcn, NULL);
    
    std::cin.tie(static_cast<std::ostream*>(0));
    while(1) {
        std::string input = "";
        getline(std::cin, input);
        timespec next_send;
        clock_gettime(CLOCK_MONOTONIC, &next_send);
        for (std::string::iterator it = input.begin(); it != input.end(); ++it) {
            char byte = *it;
            for( int i = 0; i < 8; ++i ) {
                char bit_to_send = (byte & 0x80) >> 7;
                send_message(0, bit_to_send);
                byte >>= 1;
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_send, NULL);
                add_timespec(&next_send, 2*7800000);
            }
        }
        
        dac.setChannelLevel(CH_A, 0, false, false);
    }
    sleep(1);
    
    return 0;
}
