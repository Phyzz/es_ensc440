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

es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0");
es_DAC dac = es_DAC("/dev/spidev0.1");

std::map<int, std::vector<int> > transmit_map = {
    {BEACON0, {392, 410}},
    {BEACON1, {428, 444}},
    {BEACON2, {462, 480}},
    {BEACON3, {497, 515}},
};

std::map<int, std::vector<int> > reciever_map = {
    {38158, {BEACON0, 0}},
    {38322, {BEACON0, 0}},
    {38487, {BEACON0, 1}},
    {38651, {BEACON0, 1}},
    {38816, {BEACON1, 0}},
    {38980, {BEACON1, 0}},
    {39145, {BEACON1, 1}},
    {39309, {BEACON1, 1}},
    {39474, {BEACON2, 0}},
    {39638, {BEACON2, 0}},
    {39803, {BEACON2, 1}},
    {39967, {BEACON2, 1}},
    {40132, {BEACON3, 0}},
    {40296, {BEACON3, 0}},
    {40461, {BEACON3, 1}},
    {40625, {BEACON3, 1}},
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

int main(int argc, char *argv[]){
    
    for (int i = BEACON0; i <= BEACON3; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            std::cout << "Sending " << j << " from beacon " << i << " using level " << transmit_map[i][j] << std::endl;
            send_message(i, j);
            sleep(1);
            std::vector<std::vector<int> > message = recieve_message();
            for (std::vector<std::vector<int> >::iterator it = message.begin(); it != message.end() ; ++it) {
                std::cout << "Recieved " << (*it)[1] << " from beacon " << (*it)[0] << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}
