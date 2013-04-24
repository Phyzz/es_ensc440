#ifndef es_REC_H
#define es_REC_H

#include "es_FFTSampler.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <time.h>

using std::string;

class es_Rec {

    public:
        es_Rec(es_FFTSampler* sampler, pthread_mutex_t* cout_mutex = NULL);
        
        void enterReceiveLoop();
        
        ~es_Rec();
        
    private:
        std::vector<int> receive_bit();
        void add_to_time_spec(timespec* tp, long nanos);

        es_FFTSampler*  sampler;
        std::map<int, int > receiver_map;
        pthread_mutex_t* cout_mutex;
        bool created_cout_mutex;
};

#endif
