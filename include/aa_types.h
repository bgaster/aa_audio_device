/** @file aa.h
 *      @brief Audio Anywhere Audio Device Interface Machine Type Mappings.
 *  @author Benedict R. Gaster <benedict.gaster@uwe.ac.uk>
 *  @copyright Benedict R. Gaster, 2020
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef int aa_int;
typedef unsigned int aa_uint;
typedef char aa_char;
typedef float aa_float;
typedef void aa_void;

typedef aa_void (*aa_audio_callback) (
        const aa_float **input_channel_data, 
        aa_int num_input_channels, 
        aa_float **output_channel_data, 
        aa_int num_output_channels, 
        aa_int num_samples);

#ifdef __cplusplus
}
#endif