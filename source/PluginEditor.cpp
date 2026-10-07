#include "PluginEditor.h"

SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor(SimpleDelayAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Configuración de sliders invisibles (los dibujaremos nosotros)
    auto setupSlider = [this](juce::Slider& slider, int paramIndex)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setRange(0.0, 1.0, 0.001);
        slider.addListener(this);
        slider.setUserData(reinterpret_cast<void*>(static_cast<intptr_t>(paramIndex)));
        slider.setAlpha(0.0f); // Invisible, dibujamos encima
        addAndMakeVisible(slider);
    };

    setupSlider(delaySlider, 0);
    setupSlider(feedbackSlider, 1);
    setupSlider(mixSlider, 2);

    // Valores iniciales
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.getParameters()[0]))
        delaySlider.setValue(param->get() / param->range.end, juce::dontSendNotification);
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.getParameters()[1]))
        feedbackSlider.setValue(param->get() / param->range.end, juce::dontSendNotification);
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.getParameters()[2]))
        mixSlider.setValue(param->get() / param->range.end, juce::dontSendNotification);

    // Iniciar timer para animaciones
    startTimerHz(60);

    setSize(480, 320);
    setResizable(true, false);
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
    stopTimer();
    delaySlider.removeListener(this);
    feedbackSlider.removeListener(this);
    mixSlider.removeListener(this);
}

void SimpleDelayAudioProcessorEditor::timerCallback()
{
    // Animación suave de los valores
    animatedDelay += (static_cast<float>(delaySlider.getValue()) - animatedDelay) * 0.15f;
    animatedFeedback += (static_cast<float>(feedbackSlider.getValue()) - animatedFeedback) * 0.15f;
    animatedMix += (static_cast<float>(mixSlider.getValue()) - animatedMix) * 0.15f;

    repaint();
}

void SimpleDelayAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    int paramIndex = static_cast<int>(reinterpret_cast<intptr_t>(slider->getUserData()));
    float value = static_cast<float>(slider->getValue());

    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(audioProcessor.getParameters()[paramIndex]))
    {
        // Mapear valores
        float realValue = value;
        if (paramIndex == 0) // Delay: 10-2000ms
            realValue = 10.0f + (value * 1990.0f);
        else if (paramIndex == 1) // Feedback: 0-0.95
            realValue = value * 0.95f;
        // Mix ya es 0-1

        param->setValueNotifyingHost(realValue / param->range.end);
    }
}

void SimpleDelayAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // === FONDO CON GRADIENTE ===
    juce::ColourGradient gradient(
        juce::Colour(0xff1a1a2e), 0, 0,           // Azul oscuro arriba
        juce::Colour(0xff16213e), 0, bounds.getHeight(), // Azul más oscuro abajo
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    // === BORDE EXTERIOR BRILLANTE ===
    g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
    g.drawRect(bounds.reduced(1), 2.0f);

    // === HEADER: antidoto music ===
    auto headerArea = bounds.removeFromTop(70);

    // Línea decorativa superior
    g.setColour(juce::Colour(0xff00d4ff));
    g.fillRect(headerArea.removeFromTop(2).reduced(20, 0));

    // Logo "antidoto music"
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::Font::getSystemUIFontName(), 28.0f, juce::Font::bold));
    g.drawText("antidoto music", headerArea.removeFromTop(35).reduced(20, 0), juce::Justification::left);

    // Subtítulo "DELAY"
    g.setColour(juce::Colour(0xff00d4ff));
    g.setFont(juce::Font(juce::Font::getSystemUIFontName(), 12.0f, juce::Font::bold));
    g.drawText("D E L A Y", headerArea.reduced(20, 0), juce::Justification::left);

    // === LÍNEA SEPARADORA ===
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.fillRect(bounds.removeFromTop(20).reduced(20, 10));

    // === ÁREA DE KNOBS ===
    auto knobArea = bounds.reduced(30, 20);
    int knobWidth = knobArea.getWidth() / 3;

    // Dibujar cada knob
    auto delayBounds = knobArea.removeFromLeft(knobWidth).reduced(10);
    auto feedbackBounds = knobArea.removeFromLeft(knobWidth).reduced(10);
    auto mixBounds = knobArea.reduced(10);

    // Obtener valores reales para mostrar
    float delayVal = animatedDelay;
    float feedbackVal = animatedFeedback;
    float mixVal = animatedMix;

    // Calcular texto de valores
    int delayMs = static_cast<int>(10.0f + (delayVal * 1990.0f));
    float feedbackPct = feedbackVal * 100.0f;
    float mixPct = mixVal * 100.0f;

    // Dibujar KNOB 1: TIME (Cyan)
    drawKnob(g, delayBounds, delayVal, juce::Colour(0xff00d4ff), "TIME", juce::String(delayMs) + " ms");

    // Dibujar KNOB 2: FEEDBACK (Magenta)
    drawKnob(g, feedbackBounds, feedbackVal, juce::Colour(0xffff006e), "FEEDBACK", juce::String(feedbackPct, 0) + " %");

    // Dibujar KNOB 3: MIX (Verde)
    drawKnob(g, mixBounds, mixVal, juce::Colour(0xff39ff14), "MIX", juce::String(mixPct, 0) + " %");

    // === FOOTER ===
    auto footer = bounds.removeFromBottom(25);
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.setFont(9.0f);
    g.drawText("VST3 • 64-bit • FL Studio Ready", footer.reduced(20, 0), juce::Justification::right);
    g.drawText("© 2024", footer.reduced(20, 0), juce::Justification::left);
}

void SimpleDelayAudioProcessorEditor::drawKnob(juce::Graphics& g, juce::Rectangle<int> bounds, float value, juce::Colour colour, const juce::String& label, const juce::String& valueText)
{
    // Área del knob (círculo)
    auto knobArea = bounds.removeFromTop(bounds.getHeight() * 0.6f);
    int size = juce::jmin(knobArea.getWidth(), knobArea.getHeight()) - 20;
    auto circleBounds = knobArea.withSizeKeepingCentre(size, size);

    float radius = size / 2.0f;
    float centreX = circleBounds.getCentreX();
    float centreY = circleBounds.getCentreY();

    // === SOMBRA EXTERNA (Glow effect) ===
    g.setColour(colour.withAlpha(0.1f));
    g.fillEllipse(circleBounds.expanded(8));

    // === FONDO DEL KNOB ===
    g.setColour(juce::Colour(0xff0f3460));
    g.fillEllipse(circleBounds);

    // === BORDE DEL KNOB ===
    g.setColour(colour.withAlpha(0.5f));
    g.drawEllipse(circleBounds.reduced(2), 2.0f);

    // === ARCO DE PROGRESO ===
    float startAngle = juce::MathConstants<float>::pi * 0.75f;  // 135 grados
    float endAngle = juce::MathConstants<float>::pi * 2.25f;    // 405 grados (270° de recorrido)

    juce::Path arcPath;
    arcPath.addCentredArc(centreX, centreY, radius - 8, radius - 8, 0.0f, startAngle, startAngle + (endAngle - startAngle) * value, true);

    // Track de fondo (gris)
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.strokePath(arcPath, juce::PathStrokeType(4.0f));

    // Track de progreso (color)
    g.setColour(colour);
    g.strokePath(arcPath, juce::PathStrokeType(4.0f));

    // === PUNTERO/INDICADOR ===
    float angle = startAngle + (endAngle - startAngle) * value;
    float pointerLength = radius - 15;
    float pointerX = centreX + std::cos(angle - juce::MathConstants<float>::halfPi) * pointerLength;
    float pointerY = centreY + std::sin(angle - juce::MathConstants<float>::halfPi) * pointerLength;

    // Línea del puntero
    g.setColour(juce::Colours::white);
    g.drawLine(centreX, centreY, pointerX, pointerY, 3.0f);

    // Punto central
    g.setColour(colour);
    g.fillEllipse(centreX - 6, centreY - 6, 12, 12);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(centreX - 3, centreY - 3, 6, 6);

    // === TEXTO DEBAJO DEL KNOB ===
    auto textArea = bounds;

    // Valor numérico grande
    g.setColour(colour);
    g.setFont(juce::Font(juce::Font::getSystemUIFontName(), 20.0f, juce::Font::bold));
    g.drawText(valueText, textArea.removeFromTop(25), juce::Justification::centred);

    // Label (TIME, FEEDBACK, etc)
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::Font(juce::Font::getSystemUIFontName(), 11.0f, juce::Font::bold));
    g.drawText(label, textArea, juce::Justification::centred);
}

void SimpleDelayAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Header
    area.removeFromTop(70);

    // Separador
    area.removeFromTop(20);

    // Footer
    auto footer = area.removeFromBottom(25);

    // Área de knobs
    auto knobArea = area.reduced(30, 20);
    int knobWidth = knobArea.getWidth() / 3;

    // Posicionar sliders invisibles sobre los knobs dibujados
    auto delayBounds = knobArea.removeFromLeft(knobWidth).reduced(10);
    delaySlider.setBounds(delayBounds.removeFromTop(delayBounds.getHeight() * 0.6f).withSizeKeepingCentre(120, 120));

    auto feedbackBounds = knobArea.removeFromLeft(knobWidth).reduced(10);
    feedbackSlider.setBounds(feedbackBounds.removeFromTop(feedbackBounds.getHeight() * 0.6f).withSizeKeepingCentre(120, 120));

    auto mixBounds = knobArea.reduced(10);
    mixSlider.setBounds(mixBounds.removeFromTop(mixBounds.getHeight() * 0.6f).withSizeKeepingCentre(120, 120));
}
