#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PluginAudioProcessor;
class ParameterInstance;
class Parameters
{
public:
    Parameters(PluginAudioProcessor &p);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    PluginAudioProcessor &audioProcessor;

public:
    juce::AudioProcessorValueTreeState apvts;

 //   std::unique_ptr<ParameterInstance> testParam;
};

class ParameterInstance : public juce::AudioProcessorParameter::Listener, juce::AsyncUpdater
{
public:
    ParameterInstance(PluginAudioProcessor &p, Parameters &pm, juce::String paramID);

    //==============================================================================
    void parameterValueChanged(int /*maybe unused*/, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}
    void handleAsyncUpdate() override;
    void triggerUpdate();

    //==============================================================================
    float get() const noexcept;
    float getSafe() const noexcept;
    juce::RangedAudioParameter *getRangedAudioParameter() const noexcept;

private:
    float valueSafe;
    std::atomic<float> value;
    std::atomic<float> cachedValue;

    juce::String paramID;
    juce::RangedAudioParameter *rangedParam = nullptr;

    PluginAudioProcessor &audioProcessor;
    Parameters &param;
};
