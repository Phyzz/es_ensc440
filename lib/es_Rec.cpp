#include "es_Rec.hpp"
#include "es_EX.hpp"

es_Rec::es_Rec(es_FFTSampler* sampler, pthread_mutex_t* cout_mutex) {
     this->receiver_map = {
            {39145, 0},
            {39474, 0},
            {39803, 0},
            {40132, 2},
            {40461, 1},
            {40789, 1},
            {41118, 1},
        };
        
        this->sampler = sampler;
        
        if(NULL == cout_mutex) {
            this->cout_mutex = new pthread_mutex_t;
            pthread_mutex_init(this->cout_mutex, NULL);
            this->created_cout_mutex = true;
        } else {
            this->cout_mutex = cout_mutex;
            this->created_cout_mutex = false;
        }
}
        
void es_Rec::enterReceiveLoop() {
    timespec next_sample;
    clock_gettime(CLOCK_MONOTONIC,&next_sample);

    std::vector<int> byte_buf;
    std::string message;
    while(1){
        this->add_to_time_spec(&next_sample, 14000000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_sample, NULL);
        std::vector<int> bit = this->receive_bit();
        if(bit.size() == 0) {
            if(byte_buf.size() == 0 && message.size() != 0) {
                pthread_mutex_lock ( this->cout_mutex );
                std::cout << "Recieved: " << message << std::endl;
                pthread_mutex_unlock ( this->cout_mutex );
                message.clear();
            } else if(byte_buf.size() != 0 && byte_buf.size() != 8) {
                message.append("?");
            } else if(byte_buf.size() != 0) {
                char byte[2] = {'\0','\0'};
                for (std::vector<int>::iterator it = byte_buf.begin(); it != byte_buf.end(); ++ it) {
                  if (*it == 2) {
                        if (message.size() != 0) {
                            message.append("?");
                        }
                        break;
                    } else {
                        byte[0] <<= 1;
                        byte[0] |= (*it);
                    }
                }
                message.append(byte);
                byte[0] = 0;
            }
            byte_buf.clear();
        } else {
            for (std::vector<int>::iterator it = bit.begin(); it != bit.end() ; ++it) {
                byte_buf.push_back(*it);
            }
        }
    }
}

std::vector<int> es_Rec::receive_bit() {
    this->sampler->takeSample();
    std::vector<int> ret;
    std::vector<FreqPower> freqs = this->sampler->getFreqsAboveThresh();
    if (freqs.size() != 0) {
        ret.push_back(this->receiver_map[sampler->getStrongestFreq()]);
    }
    return ret;
}

void es_Rec::add_to_time_spec(timespec* tp, long nanos) {
    long nsecs = tp->tv_nsec + nanos;
    tp->tv_nsec = nsecs % 1000000000;
    tp->tv_sec += nsecs / 1000000000;
}

es_Rec::~es_Rec(){
    if(this->created_cout_mutex) {
        pthread_mutex_destroy(this->cout_mutex);
        delete this->cout_mutex;
    }
}

