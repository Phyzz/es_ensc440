#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"
#include "../lib/es_Cal.hpp"
#include "../lib/es_Rec.hpp"

pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;

es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 119, 125);
es_DAC dac = es_DAC("/dev/spidev0.1");

std::map<int, int> transmit_map = {
    {0, 0},
    {1, 0},
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

void * receiver_fcn(void *ptr) {
    es_Rec receiver = es_Rec(&sampler, &cout_mutex, 1);
    receiver.enterReceiveLoop();
}

void do_calibration() {
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 114, 127);
    es_Calibrator calibrator = es_Calibrator(&dac, &sampler);

    calibrator.loadCachedSetVals();
    std::map<int,int> set_vals;
    if(calibrator.testCachedSetVals()) {
        set_vals = calibrator.getCachedSetVals();
    } else {
        pthread_mutex_lock ( &cout_mutex );
        std::cout << "Recalibrating the transmitter..." << std::endl;
        pthread_mutex_unlock ( &cout_mutex );
        for (int i = 0; i < 5; ++i) {
            set_vals = calibrator.doCalibration();
            if (calibrator.testCachedSetVals()) {
                break;
            }
        }
        if (!calibrator.testCachedSetVals()) {
            return;
        }
        calibrator.saveCachedSetVals();
        pthread_mutex_lock ( &cout_mutex );
        std::cout << "Calibration complete" << std::endl;
        pthread_mutex_unlock ( &cout_mutex );
    }
    transmit_map[0] = set_vals[39474];
    transmit_map[1] = set_vals[40789];
}

int main(int argc, char *argv[]){
    timespec next_send;   

    do_calibration();
    dac.setChannelLevel(CH_A, 0, false, false);
    
    if(transmit_map[0] == 0 || transmit_map[1] == 0) {
        std::cout << "Calibration failed" << std::endl;
        return 1;
    }

    std::cin.tie(static_cast<std::ostream*>(0));
    pthread_t thread2;
    pthread_create(&thread2, NULL, &receiver_fcn, NULL);

    int seq = 0;
    std::vector<unsigned char> message;
    while(1) {
        message.clear();
        sleep(4);
        timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        struct tm * timeinfo;
        timeinfo = localtime(&(current_time.tv_sec));
        
        pthread_mutex_lock ( &cout_mutex );
        std::cout << "Transmitting: " << seq << ": " << asctime(timeinfo) << current_time.tv_nsec << "ns" << std::endl;
        pthread_mutex_unlock ( &cout_mutex );
        
        message.push_back((unsigned char) seq >> 8);
        message.push_back((unsigned char) seq);
        seq++;
        message.push_back((unsigned char) current_time.tv_sec >> 24);
        message.push_back((unsigned char) current_time.tv_sec >> 16);
        message.push_back((unsigned char) current_time.tv_sec >> 8);
        message.push_back((unsigned char) current_time.tv_sec);
        message.push_back((unsigned char) current_time.tv_nsec >> 8);
        message.push_back((unsigned char) current_time.tv_nsec);
        
        clock_gettime(CLOCK_MONOTONIC,&next_send);
        for (std::vector<unsigned char>::iterator it = message.begin(); it != message.end(); ++it) {
            unsigned char byte = *it;
            for( int i = 0; i < 8; ++i ) {
                add_to_time_spec(&next_send,14000000);
                char bit_to_send = (byte & 0x80) >> 7;
                send_message(bit_to_send);
                byte <<= 1;
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_send, NULL);
            }
            add_to_time_spec(&next_send,14000000);
            dac.setChannelLevel(CH_A, 0, false, false);
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_send, NULL);
        }
    }

    return 0;
}
