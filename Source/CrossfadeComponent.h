#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"

// CrossfadeComponent allows smooth transitions between two DJ decks
// by adjusting their volume levels dynamically using a vertical slider.
class CrossfadeComponent : public juce::Component, public juce::Slider::Listener
{
public:
    // Constructor: Initializes the crossfade component.
    // - player1: Pointer to the first DJAudioPlayer instance (Deck 1).
    // - player2: Pointer to the second DJAudioPlayer instance (Deck 2).
    CrossfadeComponent(DJAudioPlayer* player1, DJAudioPlayer* player2);

    // Destructor: Cleans up the CrossfadeComponent.
    ~CrossfadeComponent() override;

    // Paint function: Draws the background, gradient effect, and visual elements.
    // - g: Reference to the JUCE graphics context used for rendering.
    void paint(juce::Graphics& g) override;

    // Resized function: Adjusts the layout when the component size changes.
    void resized() override;

    // Handles slider movement and adjusts the volume of both decks accordingly.
    // - slider: Pointer to the slider that was moved.
    void sliderValueChanged(juce::Slider* slider) override;

    // Applies the selected theme colors to the crossfade component.
    void applyTheme();

private:
    DJAudioPlayer* player1; // Pointer to Deck 1's audio player.
    DJAudioPlayer* player2; // Pointer to Deck 2's audio player.

    juce::Slider crossfadeSlider; // Slider for manually adjusting crossfade level.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CrossfadeComponent)
};
