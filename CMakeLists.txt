cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_ARCHITECTURES "arm64")

project(Dnks VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4   
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Dnks
    COMPANY_NAME                    "Rainbow Circuit"
    BUNDLE_ID                       "com.rainbowcircuit.rift"
    IS_SYNTH                        FALSE
    NEEDS_MIDI_INPUT                FALSE
    NEEDS_MIDI_OUTPUT               FALSE
    IS_MIDI_EFFECT                  FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS     TRUE
    COPY_PLUGIN_AFTER_BUILD         TRUE
    PLUGIN_MANUFACTURER_CODE        Rnbo
    PLUGIN_CODE                     Dnks
    FORMATS                         VST3 AU Standalone
    PRODUCT_NAME                    "DnkSaus"
)

juce_generate_juce_header(Dnks)

target_sources(Dnks PRIVATE
    source/main/PluginProcessor.cpp
    source/main/PluginProcessor.h
    source/main/PluginEditor.cpp
    source/main/PluginEditor.h
)

target_compile_definitions(Dnks PUBLIC
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_compile_options(Dnks PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-ffast-math>
)

target_link_libraries(Dnks PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_formats
    juce::juce_audio_devices
    juce::juce_audio_basics
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_events
    juce::juce_data_structures
    juce::juce_core
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    PRIVATE
    juce::juce_recommended_warning_flags
)