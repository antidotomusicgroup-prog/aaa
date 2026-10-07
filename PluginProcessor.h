#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SimpleDelayAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Slider::Listener
{
public:
    SimpleDelayAudioProcessorEditor(SimpleDelayAudioProcessor&);
    ~SimpleDelayAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    SimpleDelayAudioProcessor& audioProcessor;

    juce::Slider delaySlider;
    juce::Slider feedbackSlider;
    juce::Slider mixSlider;

    juce::Label delayLabel;
    juce::Label feedbackLabel;
    juce::Label mixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDelayAudioProcessorEditor)
};