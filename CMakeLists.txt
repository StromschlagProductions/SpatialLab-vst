cmake_minimum_required(VERSION 3.22)
project(SpatialLab VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CTest)
option(SPATIALLAB_BUILD_PLUGIN "Build JUCE VST3 and processor tests" ON)
if(BUILD_TESTING)
    add_executable(SpatialLabDspTests Tests/DspTests.cpp)
    target_include_directories(SpatialLabDspTests PRIVATE Source)
    add_test(NAME DSP COMMAND SpatialLabDspTests)
endif()
if(SPATIALLAB_BUILD_PLUGIN)
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 51a8a6d7aeae7326956d747737ccf1575e61e209
        GIT_PROGRESS TRUE)
    FetchContent_MakeAvailable(JUCE)
    juce_add_plugin(SpatialLab
        COMPANY_NAME "Stromschlag Productions"
        BUNDLE_ID "com.stromschlagproductions.spatiallab"
        PLUGIN_MANUFACTURER_CODE StPr
        PLUGIN_CODE SpLb
        FORMATS VST3
        PRODUCT_NAME "SpatialLab"
        VST3_CATEGORIES Fx Reverb
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE)
    target_sources(SpatialLab PRIVATE Source/PluginProcessor.cpp Source/PluginProcessor.h Source/RoomEngine.h)
    target_compile_definitions(SpatialLab PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
    target_link_libraries(SpatialLab PRIVATE juce::juce_audio_processors
        PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
    if(BUILD_TESTING)
        juce_add_console_app(SpatialLabProcessorTests PRODUCT_NAME "SpatialLab Processor Tests")
        target_sources(SpatialLabProcessorTests PRIVATE Tests/ProcessorTests.cpp)
        target_include_directories(SpatialLabProcessorTests PRIVATE Source)
        target_link_libraries(SpatialLabProcessorTests PRIVATE SpatialLab juce::juce_audio_processors)
        add_test(NAME Processor COMMAND SpatialLabProcessorTests)
    endif()
endif()
