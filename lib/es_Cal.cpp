#include "es_Cal.hpp"
#include <iostream>
#include <stdexcept>

es_Calibrator::es_Calibrator(es_DAC* dac, es_FFTSampler* sampler, pthread_mutex_t* dac_mutex, pthread_mutex_t* sampler_mutex) {
    this->dac = dac;
    this->sampler = sampler;
    if(NULL == dac_mutex) {
        this->dac_mutex = new pthread_mutex_t;
        pthread_mutex_init(this->dac_mutex, NULL);
        this->created_dac_mutex = true;
    } else {
        this->created_dac_mutex = false;
    }
    if(NULL == sampler_mutex) {
        this->sampler_mutex = new pthread_mutex_t;
        pthread_mutex_init(this->sampler_mutex, NULL);
        this->created_sampler_mutex = true;
    } else {
        this->created_sampler_mutex = false;
    }
}
        
std::map<int, int> es_Calibrator::doCalibration() {
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 6000000;
    this->set_val_cache.clear();
    
    std::set<int> frequencies;
    std::map<int, std::vector<std::vector<int> > > raw_vals;
    
    std::map<int, int> set_vals;
    
    sleep(1);
    for (int i = 0; i < 20; ++i) {
        int last_freq = 0 ;
        for (int level = 330; level < 600; ++level) {
            pthread_mutex_lock ( this->dac_mutex );
            this->dac->setChannelLevel(CH_A, level, false, false);
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
    
    for(std::set<int>::iterator it = frequencies.lower_bound(38000); it != frequencies.lower_bound(40800); ++it) {
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
        this->set_val_cache[frequency] = (int) (set + 0.5);
   }
   return this->set_val_cache;
}

std::map<int, int> es_Calibrator::getCachedSetVals() {
    return this->set_val_cache;
}

void es_Calibrator::loadCachedSetVals() {
    std::ifstream cache_file ("/root/vco_cal_cache.txt");
    std::string line;

    while( cache_file.good() ) {
        getline(cache_file, line);
        int freq    = 0;
        int set_val = 0;
        std::string freq_string;
        std::string set_val_string;
        try {
            freq_string    = line.substr(0, line.find(" : "));
            set_val_string = line.substr(line.find(" : ") + 3);
        } catch(std::out_of_range&) {
            continue;
        }
        std::stringstream convert(freq_string);
        convert >> freq;
        std::stringstream convert2(set_val_string);
        convert2 >> set_val;
        if(freq != 0 && set_val != 0 ) {
            this->set_val_cache[freq] = set_val;
        }
    }
    cache_file.close();
}

void es_Calibrator::saveCachedSetVals() {
    std::ofstream cache_file ("/root/vco_cal_cache.txt");
    for (std::map<int, int>::iterator it = this->set_val_cache.begin(); it != this->set_val_cache.end(); ++it) {
        cache_file << it->first << " : " << it->second << std::endl;
    }
    cache_file.close();
}

bool es_Calibrator::testCachedSetVals() {
    timespec interval;
    interval.tv_sec = 0;
    interval.tv_nsec = 50000000;

    if(this->set_val_cache.size() == 0) {
        return false;
    }
    
    for (std::map<int, int>::iterator it = this->set_val_cache.begin(); it != this->set_val_cache.end(); ++it) {
        for (int i = -1; i <= 1; ++i) {
std::cout << it->first << " " << it->second << " " << i << std::endl;
            pthread_mutex_lock ( this->dac_mutex );
            this->dac->setChannelLevel(CH_A, it->second + i, false, false);
            pthread_mutex_unlock( this->dac_mutex );
            
            clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
        
            pthread_mutex_lock( this->sampler_mutex );
            this->sampler->takeSample();
            int highest_freq = this->sampler->getStrongestFreq();
            pthread_mutex_unlock( this->sampler_mutex );
std::cout << highest_freq << std::endl;
            
            if (highest_freq != it->first) {
                return false;
            }
        }
    }
    return true;
}

es_Calibrator::~es_Calibrator() {
    if(this->created_dac_mutex) {
        pthread_mutex_destroy(this->dac_mutex);
        delete this->dac_mutex;
    }
    if(this->created_sampler_mutex) {
        pthread_mutex_destroy(this->sampler_mutex);
        delete this->sampler_mutex;
    }
}
	
