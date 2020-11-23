/** @file aa.h
 *      @brief Audio Anywhere Audio Device Interface.
 *  @author Benedict R. Gaster <benedict.gaster@uwe.ac.uk>
 *  @copyright Benedict R. Gaster, 2020
 */
#pragma once

#include <aa_types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum AA_ERROR {
    AA_SUCCESS = 0,
    AA_ERROR = 1,
    AA_MEMORY_ERROR = 2,
    AA_JSON_ERROR = 3,
    AA_DEVICE_SETUP = 4,
    AA_NO_DEVICES = 5, 
};

struct aa_audio;

aa_int aa_create_audio(aa_audio** audio);
aa_int aa_destory_audio(aa_audio* aa_audio);

aa_int aa_set_audio_callback(aa_audio* audio, aa_audio_callback callback);

aa_int aa_set_device_info(aa_audio* audio, const aa_char * device_info);
aa_int aa_get_device_info(
    const aa_audio* audio, 
    aa_char * device_info, 
    aa_int *device_info_size);

aa_int aa_get_devices(
    const aa_audio* audio, 
    aa_char * devices, 
    aa_int *devices_size);

aa_int aa_initialise(
    aa_audio* audio,
    int num_input_channels_needed, 
    int num_outputChannels_needed);

#ifdef __cplusplus
}
#endif