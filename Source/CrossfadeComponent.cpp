#include "CrossfadeComponent.h"
#include "ThemeComponent.h"

// Constructor: Initializes the Crossfade component and configures the crossfade slider.
// - p1: Pointer to the first DJAudioPlayer instance (Deck 1).
// - p2: Pointer to the second DJAudioPlayer instance (Deck 2).
CrossfadeComponent::CrossfadeComponent(DJAudioPlayer* p1, DJAudioPlayer* p2)
    : player1(p1), player2(p2)
{
    // Configure the crossfade slider style.
    crossfadeSlider.setSliderStyle(juce::Slider::LinearVertical);
    crossfadeSlider.setRange(0.0, 1.0, 0.01); // ✅ Smoother transition between decks

    crossfadeSlider.setValue(0.5); // Start with an equal mix of both decks
    crossfadeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    crossfadeSlider.addListener(this); // ✅ Attach event listener for value changes

    // ✅ Customize the slider appearance
    crossfadeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::yellow);  // Knob color
    crossfadeSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkgrey); // Track color
    crossfadeSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::black); // Background

    // Make the slider visible in the UI.
    addAndMakeVisible(crossfadeSlider);
}

// Destructor: Cleans up the Crossfade component.
CrossfadeComponent::~CrossfadeComponent() {}

// Paint function: Renders the background and knob of the crossfade slider.
// - g: Reference to the JUCE graphics context used for drawing.
void CrossfadeComponent::paint(juce::Graphics& g)
{
    // ✅ Create a sleek vertical gradient background
    juce::ColourGradient gradient(juce::Colours::darkred, 0, 0,
        juce::Colours::darkblue, 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);

    // ✅ Create a glowing border effect around the component
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 2.0f);

    // 🎚 **Draw the knob handle that moves with the slider**
    float value = crossfadeSlider.getValue(); // Get the current slider position
    int minY = 20;  // Minimum Y position for the knob
    int maxY = getHeight() - 40; // Maximum Y position for the knob
    int knobY = minY + (1.0f - value) * (maxY - minY); // Calculate knob position
    int knobX = getWidth() / 2; // Center knob horizontally

    g.setColour(juce::Colours::cyan);
    g.fillEllipse(knobX - 15, knobY - 15, 30, 30); // ✅ Smooth round knob effect
}

// Resized function: Adjusts the layout when the window size changes.
void CrossfadeComponent::resized()
{
    int sliderWidth = getWidth() * 0.8; // Scale slider width dynamically
    crossfadeSlider.setBounds(getWidth() / 2 - sliderWidth / 2, 20, sliderWidth, getHeight() - 40);
}

// Handles slider value changes and adjusts deck volumes accordingly.
// - slider: Pointer to the slider that was moved.
void CrossfadeComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &crossfadeSlider)
    {
        float value = slider->getValue();
        player1->setGain(1.0 - value); // Reduce volume of Deck 1
        player2->setGain(value);       // Increase volume of Deck 2

        repaint();  // ✅ Repaint to update knob position visually
    }
}

// Applies the selected theme colors to the crossfade component.
void CrossfadeComponent::applyTheme()
{
    // Set the text color for the crossfade slider based on the theme.
    crossfadeSlider.setColour(juce::Slider::textBoxTextColourId, ThemeComponent::getTextColor());

    repaint(); // ✅ Ensure UI refreshes after theme change
}
