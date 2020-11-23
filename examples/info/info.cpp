/** @file info.cpp
 *      @brief Display Audio device info.
 *  @author Benedict R. Gaster <benedict.gaster@uwe.ac.uk>
 */
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <math.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono> 
#include <nlohmann/json.hpp>

//==============================================================================

// AA JUCE Wrapper
#include <aa.h>

using namespace std;
using namespace nlohmann;

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

    err = aa_initialise(audio, 1, 2); 
    check_err(err, "aa_initialise");

    aa_int size;
    err = aa_get_device_info(audio, nullptr, &size);
    check_err(err, "aa_get_device_info");
    aa_char * info = new aa_char[size];

    err = aa_get_device_info(audio, info, nullptr);
    check_err(err, "aa_get_device_info");

    std::string device_info{info};
    auto json_result  = json::parse(device_info, nullptr, false);
    if (json_result != json::value_t::discarded) {
        if (json_result.is_object() && 
            json_result["output_device"].is_string() &&
            json_result["input_device"].is_string() &&
            json_result["sample_rate"].is_number() &&
            json_result["buffer_size"].is_number() &&
            json_result["input_channels"].is_number_integer() &&
            json_result["output_channels"].is_number_integer()) {  

            std::string output = json_result["output_device"].get<std::string>();
            std::string input = json_result["input_device"].get<std::string>();
            double sample_rate = json_result["sample_rate"].get<double>();
            int buffer_size = json_result["buffer_size"].get<int>();
            const auto input_channels = json_result["input_channels"].get<int>();
            auto output_channels = json_result["output_channels"].get<int>();

            cout << "Device Info:" << endl;
            cout  << "\tOutput: " << output << endl
                   << "\tInput: " << input << endl
                   << "\tSample rate: " << sample_rate << endl
                   << "\tBuffer size: " << buffer_size << endl
                   << "\tInput channels: " << bitset<8>{static_cast<unsigned long long>(input_channels)} << endl
                   << "\tOutput channels: " << bitset<8>{static_cast<unsigned long long>(output_channels)} << endl
                   << endl;
        }
    }
    else {
        check_err(-1, "failed to parse device_info JSON");
    }

    delete[] info;

    err = aa_get_devices(audio, nullptr, &size);
    check_err(err, "aa_get_devices");
    aa_char * device_names = new aa_char[size];
    err = aa_get_devices(audio, device_names, nullptr);
    check_err(err, "aa_get_devices");

    std::string dns{device_names};
    json_result  = json::parse(dns, nullptr, false);
    if (json_result != json::value_t::discarded) {
        if (json_result.is_object() && json_result["devices"].is_array()) {
            cout  << "Device Type: " << endl;
            for (auto& dt: json_result["devices"].get<vector<json>>()) {
                if (dt.is_object() &&
                    dt["device_type"].is_string() &&
                    dt["input_devices"].is_array() &&
                    dt["output_devices"].is_array()) {

                    cout  << "\t" << dt["device_type"].get<std::string>() << endl;
                    cout  << "\t" << "Input Devices" << endl;
                    for (auto& input_device: dt["input_devices"].get<vector<json>>()) {
                        if (input_device.is_string()) {
                            cout  << "\t\t" << input_device.get<std::string>() << endl;
                        }
                    }
                    cout  << "\t" << "Output Devices" << endl;
                    for (auto& output_device: dt["output_devices"].get<vector<json>>()) {
                        if (output_device.is_string()) {
                            cout  << "\t\t" << output_device.get<std::string>() << endl;
                        }
                    }
                }
            }
        }
    }

    err = aa_destory_audio(audio);
    check_err(err,  "aa_destory_audio");

    cout  << "SUCCESS" << endl;

    return 0;
}