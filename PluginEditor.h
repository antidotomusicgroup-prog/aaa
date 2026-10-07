#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SimpleDelayAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Slider::Listener,
                                       private juce::Timer
{
public:
    SimpleDelayAudioProcessorEditor(SimpleDelayAudioProcessor&);
    ~SimpleDelayAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

private:
    void drawKnob(juce::Graphics& g, juce::Rectangle<int> bounds, float value, juce::Colour colour, const juce::String& label, const juce::String& valueText);

    SimpleDelayAudioProcessor& audioProcessor;

    juce::Slider delaySlider;
    juce::Slider feedbackSlider;
    juce::Slider mixSlider;

    // Para animaciones suaves
    float animatedDelay = 0.0f;
    float animatedFeedback = 0.0f;
    float animatedMix = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDelayAudioProcessorEditor)
};