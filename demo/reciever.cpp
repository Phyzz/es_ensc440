#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"

#define BEACON0 0
#define BEACON1 1
#define BEACON2 2
#define BEACON3 3

es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 116, 123);

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
}

int main(int argc, char *argv[]){
    timespec next_sample;
    clock_gettime(CLOCK_MONOTONIC,&next_sample);

    std::vector<std::vector<int> > message;
    while(1){
        add_to_time_spec(&next_sample, 14000000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_sample, NULL);
        std::vector<std::vector<int> > bit = recieve_message();
        if(bit.size() == 0) {
            std::string bla;
            char byte[2] = {'\0','\0'};
            int i = 0;
            for (std::vector<std::vector<int> >::iterator it = message.begin(); it != message.end(); ++ it) {
                byte[0] <<= 1;
                byte[0] |= (*it)[1];

                if( ++i >= 8) {
                    bla.append(byte);
                    byte[0] = 0;
                    i = 0;
                }
                if ((*it)[0] != 0) {
                    std::cout << "Recieved " << (*it)[1] << " from beacon " << (*it)[0] << std::endl;
                }
            }
            if(bla.size() != 0) {
                std::cout << "Recieved " << bla << std::endl;
            }
            message.clear();
        } else {
            for (std::vector<std::vector<int> >::iterator it = bit.begin(); it != bit.end() ; ++it) {
                message.push_back(*it);
            }
        }
    }

    return 0;
}
