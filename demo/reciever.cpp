#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"

es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 119, 125);

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

std::vector<int> recieve_message() {
    sampler.takeSample();
    std::vector<int> ret;
    std::vector<FreqPower> freqs = sampler.getFreqsAboveThresh();
    if (freqs.size() != 0) {
        ret.push_back(reciever_map[sampler.getStrongestFreq()]);
    }
    return ret;
}

int main(int argc, char *argv[]){
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
                    std::cout << "Recieved bad bit." << std::endl;
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
                std::cout << "Recieved " << bla << std::endl;
            }
            message.clear();
        } else {
            for (std::vector<int>::iterator it = bit.begin(); it != bit.end() ; ++it) {
                message.push_back(*it);
            }
        }
    }

    return 0;
}
