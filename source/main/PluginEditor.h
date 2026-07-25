#pragma once

#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include <JuceHeader.h>

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor(PluginProcessor &);
    ~PluginEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
  PluginProcessor &audioProcessor;

  juce::WebSliderRelay gainRelay{"gain"};
  juce::WebSliderParameterAttachment gainWebAttachment{
      *audioProcessor.apvts.getParameter("gain") , gainRelay, nullptr};

//  juce::WebBrowserComponent::Options wvOptions;
  juce::WebBrowserComponent webView{
      juce::WebBrowserComponent::Options{}
          .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
          .withNativeIntegrationEnabled()
          .withOptionsFrom(gainRelay)};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};