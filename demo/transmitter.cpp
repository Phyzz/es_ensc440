#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"

pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;

es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 119, 125);
es_DAC dac = es_DAC("/dev/spidev0.1");

std::map<int, int> transmit_map = {
    {0, 448},
    {1, 520},
};

std::map<int, int > reciever_map = {
    {39145, 0},
    {39474, 0},
    {39803, 0},
    {40132, 2},
    {40461, 1},
    {40789, 1},
    {41118, 1},
};

void add_to_time_spec(timespec* tp, long nanos) {
    long nsecs = tp->tv_nsec + nanos;
    tp->tv_nsec = nsecs % 1000000000;
    tp->tv_sec += nsecs / 1000000000;
}

void add_to_time_spec(timespec* tp, int secs, long nanos) {
    long nsecs = tp->tv_nsec + nanos;
    tp->tv_nsec = nsecs % 1000000000;
    tp->tv_sec += secs + nsecs / 1000000000;
}

void add_to_time_spec(timespec* tp, timespec* to_add) {
    long nsecs = tp->tv_nsec + to_add->tv_nsec;
    tp->tv_nsec = nsecs % 1000000000;
    tp->tv_sec += to_add->tv_sec + nsecs / 1000000000;
}

void sub_from_time_spec(timespec* tp, long nanos) {
    long nsecs = tp->tv_nsec - nanos;
    if(nsecs < 0) {
        nsecs += 1000000000;
        tp->tv_sec--;
    }
    tp->tv_nsec = nsecs % 1000000000;
}

void sub_from_time_spec(timespec* tp, int secs, long nanos) {
    long nsecs = tp->tv_nsec - nanos;
    if(nsecs < 0) {
        nsecs += 1000000000;
        tp->tv_sec--;
    }
    tp->tv_nsec = nsecs % 1000000000;
    tp->tv_sec -= secs;
}

void sub_from_time_spec(timespec* tp, timespec* to_sub) {
    long nsecs = tp->tv_nsec - to_sub->tv_nsec;
    if(nsecs < 0) {
        nsecs += 1000000000;
        tp->tv_sec--;
    }
    tp->tv_nsec = nsecs % 1000000000;
    tp->tv_sec -= to_sub->tv_sec;
}

void send_message(unsigned short bit) {
    assert(bit <= 1);
    dac.setChannelLevel(CH_A, transmit_map[bit], false, false);
}

std::vector<int> recieve_message() {
    sampler.takeSample();
    std::vector<int> ret;
    std::vector<FreqPower> freqs = sampler.getFreqsAboveThresh();
    if (freqs.size() != 0) {
        ret.push_back(reciever_map[sampler.getStrongestFreq()]);
    }
    return ret;
}

void * reciever_fcn(void *ptr) {
    timespec next_sample;
    clock_gettime(CLOCK_MONOTONIC,&next_sample);

    std::vector<int> message;
    while(1){
        add_to_time_spec(&next_sample, 14000000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_sample, NULL);
        std::vector<int> bit = recieve_message();
        if(bit.size() == 0) {
            std::string bla;
            char byte[2] = {'\0','\0'};
            int i = 0;
            for (std::vector<int>::iterator it = message.begin(); it != message.end(); ++ it) {
              if (*it == 2) {
                    pthread_mutex_lock ( &cout_mutex );
                    std::cout << "Recieved bad bit." << std::endl;
                    pthread_mutex_unlock ( &cout_mutex );
                    bla.clear();
                    break;
                } else {
                    byte[0] <<= 1;
                    byte[0] |= (*it);

                    
                    if( ++i >= 8) {
                        bla.append(byte);
                        byte[0] = 0;
                        i = 0;
                    }
                }
                
            }
            if(bla.size() != 0) {
                pthread_mutex_lock ( &cout_mutex );
                std::cout << "Recieved " << bla << std::endl;
                pthread_mutex_unlock ( &cout_mutex );
            }
            message.clear();
        } else {
            for (std::vector<int>::iterator it = bit.begin(); it != bit.end() ; ++it) {
                message.push_back(*it);
            }
        }
    }
}

int main(int argc, char *argv[]){
    timespec next_send;

    std::cin.tie(static_cast<std::ostream*>(0));
    pthread_t thread2;
    pthread_create(&thread2, NULL, &reciever_fcn, NULL);

    while(1) {
        std::string input = "";
        getline(std::cin, input);
        clock_gettime(CLOCK_MONOTONIC,&next_send);
        for (std::string::iterator it = input.begin(); it != input.end(); ++it) {
            char byte = *it;
            for( int i = 0; i < 8; ++i ) {
                add_to_time_spec(&next_send,14000000);
                char bit_to_send = (byte & 0x80) >> 7;
                send_message(bit_to_send);
                byte <<= 1;
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_send, NULL);
            }
        }
        dac.setChannelLevel(CH_A, 0, false, false);
    }

    return 0;
}
