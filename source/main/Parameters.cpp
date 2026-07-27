#include "Parameters.h"

Parameters::Parameters(PluginAudioProcessor &p) : audioProcessor(p),
                                                  apvts(audioProcessor, nullptr, "Parameters", createParameterLayout())
{

  //  reverbLevel = std::make_unique<ParameterInstance>(audioProcessor, *this, "reverbLevel");
}

juce::AudioProcessorValueTreeState::ParameterLayout
Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    /*
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"reverbLevel", 1},
                                                           "Reverb Level",
                                                           juce::NormalisableRange<float>{0.0f, 1.0f, 0.01}, 1.0f));

    return layout;
    */
}

ParameterInstance::ParameterInstance(PluginAudioProcessor &p, Parameters &pm, juce::String paramID) : audioProcessor(p), param(pm)
{
    this->paramID = paramID;

    float initValue = param.apvts.getRawParameterValue(paramID)->load();
    value.store(initValue);
    valueSafe = initValue;
    cachedValue.store(initValue);

    if (auto *parameter = dynamic_cast<juce::AudioProcessorParameterWithID *>(param.apvts.getParameter(paramID)))
    {
        if (auto *ranged = dynamic_cast<juce::RangedAudioParameter *>(parameter))
        {
            rangedParam = ranged;
            rangedParam->addListener(this);
        }
    }
}

void ParameterInstance::parameterValueChanged(int /*maybe unused*/, float newValue)
{
    // load atomics for thread safe reading
    cachedValue.store(newValue);
    triggerUpdate();
    triggerAsyncUpdate();
}

void ParameterInstance::handleAsyncUpdate()
{
    if (rangedParam)
    {
        float newValue = cachedValue.load(std::memory_order_relaxed);

        if (auto *parameter = dynamic_cast<juce::AudioProcessorParameterWithID *>(param.apvts.getParameter(paramID)))
        {
            if (auto *rangedParam = dynamic_cast<juce::RangedAudioParameter *>(parameter))
            {
                valueSafe = rangedParam->convertFrom0to1(newValue);
            }
        }
    }
}

void ParameterInstance::triggerUpdate()
{
    if (rangedParam)
    {
        float newValue = cachedValue.load(std::memory_order_relaxed);
        if (auto *parameter = dynamic_cast<juce::AudioProcessorParameterWithID *>(param.apvts.getParameter(paramID)))
        {
            if (auto *rangedParam = dynamic_cast<juce::RangedAudioParameter *>(parameter))
            {
                value.store(rangedParam->convertFrom0to1(newValue));
            }
        }
    }
}

float ParameterInstance::get() const noexcept
{
    return value.load(std::memory_order_relaxed);
}

float ParameterInstance::getSafe() const noexcept
{
    return valueSafe;
}

juce::RangedAudioParameter *ParameterInstance::getRangedAudioParameter() const noexcept
{
    return rangedParam;
}
