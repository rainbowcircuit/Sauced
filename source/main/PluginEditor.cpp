#include "PluginEditor.h"
#include "juce_core/juce_core.h"
#include "juce_gui_extra/juce_gui_extra.h"

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    juce::ignoreUnused(audioProcessor);
    setSize(800, 800);

    addAndMakeVisible(webView);
    

    webView.goToURL("http://localhost:8000");
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics &g)
{
    webView.setBounds(getLocalBounds());

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto area = getLocalBounds();
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    auto helloWorld = juce::String("Hello World ");
    g.drawText(helloWorld, area.removeFromTop(150), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
}