#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "../lib/es_DAC.hpp"
#include "../lib/es_FFTSampler.hpp"
#include "../lib/es_Rec.hpp"

int main(int argc, char *argv[]){
    es_FFTSampler sampler = es_FFTSampler("/dev/spidev0.0", 119, 125, 5);
    es_Rec receiver = es_Rec(&sampler, NULL, 1);
    receiver.enterReceiveLoop();

    return 0;
}
