cmake_minimum_required(VERSION 3.12)

# Enable C++17 as this is used to compile Juce code.
set(CMAKE_CXX_STANDARD 17)

# JSON for Modern C++
# Fetch JSON C++ dependency
include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# JUCE static library, compiled just for audio device manager
SET( JUCE_LIB 
     ${CMAKE_SOURCE_DIR}/external/AAStandaloneJUCE/Builds/MacOSX/build/Release/libAAStandaloneJUCE.a)

add_definitions(-DNDEBUG)

include_directories(
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/AAStandaloneJUCE/JuceLibraryCode/ 
    ${CMAKE_SOURCE_DIR}/external/AAStandaloneJUCE/JuceLibraryCode/modules/
)

# Setup the AA static library
SET( AA_LIB  aa)

ADD_LIBRARY( ${AA_LIB} STATIC
             src/lib.cpp
             include/aa_types.h
             include/aa.h
             include/global.hpp )

# Include the JSON library 
target_link_libraries(${AA_LIB} 
                      ${JUCE_LIB}
                      nlohmann_json::nlohmann_json)

# Setup examples

SET( INFO_EXE aaInfo )

SET( SINE_EXE aaSine )

ADD_EXECUTABLE( ${INFO_EXE}
                examples/info/info.cpp ) 

ADD_EXECUTABLE( ${SINE_EXE}
                examples/sine/sine.cpp ) 

if(WIN32)
    #Windows specific code
elseif(APPLE)
    #OSX specific code
    target_link_libraries(${INFO_EXE}
    "-framework CoreFoundation" 
    "-framework CoreAudioKit" 
    "-framework ApplicationServices"
    "-framework CoreServices" 
    "-framework Foundation" 
    "-framework Cocoa"
    "-framework CoreMidi"
    "-framework CoreAudio"
    "-framework Accelerate"
    )

    target_link_libraries(${SINE_EXE}
    "-framework CoreFoundation" 
    "-framework CoreAudioKit" 
    "-framework ApplicationServices"
    "-framework CoreServices" 
    "-framework Foundation" 
    "-framework Cocoa"
    "-framework CoreMidi"
    "-framework CoreAudio"
    "-framework Accelerate"
    )
endif()
            
# Link our test app
TARGET_LINK_LIBRARIES( ${INFO_EXE}
                       ${AA_LIB}
                       nlohmann_json::nlohmann_json)

TARGET_LINK_LIBRARIES( ${SINE_EXE}
                       ${AA_LIB})