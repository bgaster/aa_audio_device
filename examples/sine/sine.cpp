/** @file sine.cpp
 *      @brief Play a sine wave for a few seconds.
 *  @author Benedict R. Gaster <benedict.gaster@uwe.ac.uk>
 */
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <math.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono> 

#include <aa.h>

using namespace std;

const int OUTPUT_CHANNELS = 2;
const int INPUT_CHANNELS = 2;
const int NUM_SECONDS = 5;
const size_t TABLE_SIZE = 200;

typedef struct {
    float sine[TABLE_SIZE];
    int left_phase;
    int right_phase;
    char message[20];
} aaSineData;

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

aaSineData data_;

//==============================================================================

void callback(
    const float **inputChannelData, 
    int numInputChannels, 
    float **outputChannelData, 
    int numOutputChannels, 
    int numSamples) {

    float *outL = *outputChannelData;
    float *outR = *(outputChannelData+1);

    for(unsigned long i=0; i<numSamples; i++ ) {
        *outL++ = data_.sine[data_.left_phase];  /* left */
        *outR++ = data_.sine[data_.right_phase];  /* right */
        data_.left_phase += 1;
        if( data_.left_phase >= TABLE_SIZE ) {
            data_.left_phase -= TABLE_SIZE;
        }
        data_.right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if( data_.right_phase >= TABLE_SIZE ) {
            data_.right_phase -= TABLE_SIZE;
        }
    }
}

//==============================================================================

void check_err(aa_int err, std::string err_msg = "") {
    if (err != AA_SUCCESS) {
        cerr << "ERROR " << err_msg << ": " << err << endl;
        exit(0);
    }
}

int main (int argc, char* argv[]) {
    aa_audio * audio;
    aa_int err;
    
    err = aa_create_audio(&audio);
    check_err(err, "aa_create_audio");

    err = aa_initialise(audio, INPUT_CHANNELS, OUTPUT_CHANNELS); 
    check_err(err, "aa_initialise");

    /* initialise sinusoidal wavetable */
    for(int i=0; i<TABLE_SIZE; i++ ) {
        data_.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
    data_.left_phase = data_.right_phase = 0;

    aa_set_audio_callback(audio, callback);

    std::cout << "Sine wave for " << NUM_SECONDS << " secs" << std::endl;
    std::this_thread::sleep_for (std::chrono::seconds(NUM_SECONDS));

    err = aa_destory_audio(audio);
    check_err(err,  "aa_destory_audio");

    return 0;
}