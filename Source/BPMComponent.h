#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"

// BPMComponent is responsible for synchronizing the BPM (Beats Per Minute)
// between two DJ decks to ensure smooth transitions.
class BPMComponent : public juce::Component,
    public juce::Button::Listener // Listens for button click events
{
public:
    // Constructor: Initializes the BPM sync component.
    // - player1: Pointer to the first DJAudioPlayer instance (Deck 1).
    // - player2: Pointer to the second DJAudioPlayer instance (Deck 2).
    BPMComponent(DJAudioPlayer* player1, DJAudioPlayer* player2);

    // Destructor: Cleans up the BPMComponent.
    ~BPMComponent() override;

    // Paint function: Draws the background of the component.
    // - g: Reference to the JUCE graphics context used for rendering.
    void paint(juce::Graphics&) override;

    // Resized function: Adjusts the button size and position when the window is resized.
    void resized() override;

    // Handles button click events for syncing BPM.
    // - button: Pointer to the button that was clicked.
    void buttonClicked(juce::Button* button) override;

    // Synchronizes the BPM between Deck 1 and Deck 2 by adjusting playback speed.
    void syncBPM();

    // Applies the selected theme to the Sync BPM button.
    void applyTheme();

private:
    juce::TextButton syncButton{ "Sync BPM" }; // Button to trigger BPM synchronization
    DJAudioPlayer* player1; // Pointer to Deck 1's audio player
    DJAudioPlayer* player2; // Pointer to Deck 2's audio player
    bool syncActive = false; // ✅ Tracks whether BPM sync is currently enabled

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BPMComponent)
};
