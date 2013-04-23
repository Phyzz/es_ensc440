#include "es_Cal.hpp"

es_Calibrator::es_Calibrator(es_DAC* dac, es_FFTSampler* sampler, pthread_mutex_t* dac_mutex, pthread_mutex_t* sampler_mutex) {
    this->dac = dac;
    this->sampler = sampler;
    if(NULL == dac_mutex) {
        this->dac_mutex = new pthread_mutex_t;
        pthread_mutex_init(this->dac_mutex, NULL);
        created_dac_mutex = true;
    }
    if(NULL == sampler_mutex) {
        this->sampler_mutex = new pthread_mutex_t;
        pthread_mutex_init(this->sampler_mutex, NULL);
        created_sampler_mutex = true;
    }
}
        
std::map<int, int> es_Calibrator::doCalibration() {
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 6000000;
    
    std::set<int> frequencies;
    std::map<int, std::vector<std::vector<int> > > raw_vals;
    
    std::map<int, int> set_vals;
    
    sleep(1);
    for (int i = 0; i < 20; ++i) {
        int last_freq = 0 ;
        for (int level = 330; level < 600; ++level) {
            pthread_mutex_lock ( this->dac_mutex );
            this->dac->setChannelLevel(CH_A, (int) level, false, false);
            pthread_mutex_unlock( this->dac_mutex );
            
            clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
        
            pthread_mutex_lock( this->sampler_mutex );
            this->sampler->takeSample();
            int highest_freq = this->sampler->getStrongestFreq();
            pthread_mutex_unlock( this->sampler_mutex );
            
            if(highest_freq > last_freq) {
                frequencies.insert(last_freq);
                while (raw_vals[last_freq].size() <= i) {
                    raw_vals[last_freq].push_back({0,0});
                }
                frequencies.insert(highest_freq);
                while (raw_vals[highest_freq].size() <= i) {
                    raw_vals[highest_freq].push_back({0,0});
                }
                raw_vals[last_freq][i][1] = level-1;
                raw_vals[highest_freq][i][0] = level;
            }
            last_freq = highest_freq;
        }
    }
    
    for(std::set<int>::iterator it = frequencies.lower_bound(38000); it != frequencies.lower_bound(41200); ++it) {
        int frequency = *it;
        float lower_bound = 0;
        float upper_bound = 0;
        for(std::vector<std::vector<int> >::iterator in_it = raw_vals[frequency].begin(); in_it != raw_vals[frequency].end(); ++in_it) {
            lower_bound += (*in_it)[0];
            upper_bound += (*in_it)[1];
        }
        lower_bound = lower_bound / raw_vals[frequency].size();
        upper_bound = upper_bound / raw_vals[frequency].size();
        float set = (lower_bound + upper_bound) / 2;
        set_vals[frequency] = (int) (set + 0.5);
   }
   return set_vals;
}

es_Calibrator::~es_Calibrator() {
    if(created_dac_mutex) {
        pthread_mutex_destroy(this->dac_mutex);
        delete this->dac_mutex;
    }
    if(created_sampler_mutex) {
        pthread_mutex_destroy(this->sampler_mutex);
        delete this->sampler_mutex;
    }
}
	
