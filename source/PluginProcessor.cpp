#include "PluginEditor.h"

SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor(SimpleDelayAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Configurar sliders
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& name, int paramIndex)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        slider.setRange(0.0, 1.0, 0.01); // Normalizado, luego mapeamos
        slider.addListener(this);
        slider.setUserData(reinterpret_cast<void*>(static_cast<intptr_t>(paramIndex)));
        addAndMakeVisible(slider);

        label.setText(name, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    };

    setupSlider(delaySlider, delayLabel, "Time (ms)", 0);
    setupSlider(feedbackSlider, feedbackLabel, "Feedback", 1);
    setupSlider(mixSlider, mixLabel, "Mix", 2);

    // Valores iniciales
    delaySlider.setValue(250.0 / 2000.0, juce::dontSendNotification); // 250ms de 2000 max
    feedbackSlider.setValue(0.3, juce::dontSendNotification);
    mixSlider.setValue(0.5, juce::dontSendNotification);

    setSize(400, 250);
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
    delaySlider.removeListener(this);
    feedbackSlider.removeListener(this);
    mixSlider.removeListener(this);
}

void SimpleDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    int paramIndex = static_cast<int>(reinterpret_cast<intptr_t>(slider->getUserData()));
    float value = static_cast<float>(slider->getValue());

    // Mapear valores según el parámetro
    float realValue = value;
    if (paramIndex == 0) // Delay time: 0-1 mapea a 10-2000ms
        realValue = 10.0f + (value * 1990.0f);
    else if (paramIndex == 1) // Feedback: 0-0.95
        realValue = value * 0.95f;
    else if (paramIndex == 2) // Mix: 0-1
        realValue = value;

    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.getParameters()[paramIndex]))
    {
        param->setValueNotifyingHost(realValue / param->range.end);
    }
}

void SimpleDelayAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Simple Delay VST3", getLocalBounds().removeFromTop(40), juce::Justification::centred);
}

void SimpleDelayAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);

    auto sliderArea = area.removeFromTop(150);
    int sliderWidth = sliderArea.getWidth() / 3;

    auto placeSlider = [&](juce::Slider& slider, juce::Label& label)
    {
        auto bounds = sliderArea.removeFromLeft(sliderWidth).reduced(10);
        label.setBounds(bounds.removeFromBottom(20));
        slider.setBounds(bounds);
    };

    placeSlider(delaySlider, delayLabel);
    placeSlider(feedbackSlider, feedbackLabel);
    placeSlider(mixSlider, mixLabel);
}
