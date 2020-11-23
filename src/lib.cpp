/** @file lib.cpp
 *      @brief Audio Anywhere Audio Device API, implemenetation.
 *  @author Benedict R. Gaster <benedict.gaster@uwe.ac.uk>
 */
#define JUCE_APP_CONFIG_HEADER "global.hpp"
#include <JuceHeader.h>

#include <nlohmann/json.hpp>
#include <vector>
#include <tuple>
#include <string>

#include <aa.h>

using namespace juce;
using namespace nlohmann;

// Wrapper around JUCE's audio IO callback
class aa_callback : public AudioIODeviceCallback {
public:
    aa_callback() : audio_callback_{nullptr} {

    }

    ~aa_callback() {

    }

    void set_callback(aa_audio_callback callback) {
        audio_callback_ = callback;
    }

    void audioDeviceIOCallback (
        const float **inputChannelData, 
        int numInputChannels, 
        float **outputChannelData, 
        int numOutputChannels, 
        int numSamples) override {
        audio_callback_(inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples);
    }

    void audioDeviceAboutToStart (AudioIODevice *device) override {

    }

    void audioDeviceStopped () override {

    }
private:
    /// user defined callback
    aa_audio_callback audio_callback_;
};

// Note: Must not be exposed pubically
struct aa_audio {
    AudioDeviceManager *device_manager_;
    aa_callback *aa_callback_;

    aa_audio(AudioDeviceManager *deviceManager) : device_manager_{deviceManager} { }
    ~aa_audio() {}
};

aa_int aa_create_audio(aa_audio ** audio) {
    auto device_manager = new AudioDeviceManager{};
    if (device_manager == nullptr) {
        return AA_MEMORY_ERROR;
    }

    *audio = new aa_audio{device_manager};
    if (*audio == nullptr) {
        return AA_MEMORY_ERROR;
    }
    
    (*audio)->aa_callback_ = new aa_callback{};
    if ((*audio)->aa_callback_ == nullptr) {
        return AA_MEMORY_ERROR;
    }

    return AA_SUCCESS;
}

aa_int aa_destory_audio(aa_audio* audio) {
    if (audio == nullptr) {
        return AA_MEMORY_ERROR;
    }

    if (audio->aa_callback_ != nullptr) {
        delete audio->aa_callback_;
    }

    delete audio;
    return AA_SUCCESS;
}

aa_int aa_set_audio_callback(aa_audio * audio, aa_audio_callback callback) {
    if (audio == nullptr) {
        return AA_MEMORY_ERROR;
    }

    audio->aa_callback_->set_callback(callback);
    audio->device_manager_->addAudioCallback(audio->aa_callback_);

    return AA_SUCCESS;
}

aa_int aa_set_device_info(aa_audio* audio, const aa_char * device_info) {
    if (audio == nullptr) {
        return AA_MEMORY_ERROR;
    }

    auto error_code = AA_JSON_ERROR;
    AudioDeviceManager::AudioDeviceSetup device_setup = audio->device_manager_->getAudioDeviceSetup();

    auto json = json::parse(device_info, nullptr, false);
    if (json != json::value_t::discarded) {
        if (json.is_object() && 
            json["output_device"].is_string() &&
            json["input_device"].is_string() &&
            json["sample_rate"].is_number() &&
            json["buffer_size"].is_number()) {    
            
            std::string output = json["output_device"].get<std::string>();
            device_setup.outputDeviceName = String{output};
    
            std::string input = json["input_device"].get<std::string>();
            device_setup.inputDeviceName = String{input};
    
            double sample_rate = json["sample_rate"].get<double>();
            device_setup.sampleRate = sample_rate;
            
            int buffer_size = json["buffer_size"].get<int>();
            device_setup.bufferSize = buffer_size;
    
            if (audio->device_manager_->setAudioDeviceSetup(device_setup, true).length() == 0) {
                error_code = AA_SUCCESS;
            } 
            else {
                error_code = AA_DEVICE_SETUP;
            }
        }
    }

    return error_code;
}

aa_int aa_get_device_info(const aa_audio* audio, aa_char * device_info, aa_int *device_info_size) {
    if (audio == nullptr) {
        return AA_MEMORY_ERROR;
    }

    AudioDeviceManager::AudioDeviceSetup device_setup = audio->device_manager_->getAudioDeviceSetup();

    json info = {
        {"output_device", device_setup.outputDeviceName.toStdString() },
        {"input_device", device_setup.inputDeviceName.toStdString() },
        {"sample_rate", device_setup.sampleRate },
        {"buffer_size", device_setup.bufferSize },
        {"input_channels", device_setup.inputChannels.toInteger() },
        {"output_channels", device_setup.outputChannels.toInteger() },
    };

    auto str_info = info.dump();
    if (device_info_size != nullptr) {
        *device_info_size = str_info.size()+1; // include \0 in size
    }

    if (device_info != nullptr) {
        memcpy(device_info, str_info.c_str(), str_info.size()+1);
    }

    return AA_SUCCESS;
}

aa_int aa_get_devices(
    const aa_audio* audio, 
    aa_char * devices, 
    aa_int *devices_size) {
    if (audio == nullptr) {
        return AA_MEMORY_ERROR;
    }

    auto& device_types = audio->device_manager_->getAvailableDeviceTypes();

    std::vector<json> io_devices;

    for (auto& dt: device_types) {
              
        std::vector<std::string> names;
        dt->scanForDevices();

        // get Input devices
        auto input_names = dt->getDeviceNames(true);
        std::vector<std::string> input_devices;

        for (auto iname: input_names) {
            names.push_back(iname.toStdString());
            input_devices.push_back(iname.toStdString());
        }

        // get Output devices
        auto output_names = dt->getDeviceNames(false);
        std::vector<std::string> output_devices;

        for (auto oname: output_names) {
            names.push_back(oname.toStdString());
            output_devices.push_back(oname.toStdString());
        }
        
        json jdevice_type = {
            { "device_type", dt->getTypeName().toStdString() },
            { "input_devices", input_devices},
            { "output_devices", output_devices}
        };
        io_devices.push_back(jdevice_type);
    }

    json devices_ = {
         { "devices", io_devices }
    }; 

    auto str_devices = devices_.dump();
    if (devices_size != nullptr) {
        *devices_size = str_devices.size()+1; // include \0 in size
    }

    if (devices != nullptr) {
        memcpy(devices, str_devices.c_str(), str_devices.size()+1);
    }

    return AA_SUCCESS;
}

aa_int aa_initialise(
    aa_audio* audio, 
    int num_input_channels_needed, 
    int num_outputChannels_needed) {
     audio->device_manager_->initialise(
         num_input_channels_needed, 
         num_outputChannels_needed, 
         nullptr, 
         true, 
         {}, 
         nullptr);

    return AA_SUCCESS;
}

