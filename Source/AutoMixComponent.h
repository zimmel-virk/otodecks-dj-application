#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "PlaylistComponent.h"
#include "WaveformDisplay.h"

// AutoMixComponent handles automatic mixing between two DJ decks.
// It enables smooth transitions, BPM syncing, and automated playback.
class AutoMixComponent : public juce::Component,
    public juce::Button::Listener,  // Listens for button clicks
    public juce::Timer  // Handles time-based events
{
public:
    // Constructor: Initializes the AutoMix component and links it to the required elements.
    // Parameters:
    // - player1: Pointer to the first DJAudioPlayer (Deck 1).
    // - player2: Pointer to the second DJAudioPlayer (Deck 2).
    // - playlist: Pointer to the PlaylistComponent for track management.
    // - waveform1: Pointer to the WaveformDisplay for Deck 1.
    // - waveform2: Pointer to the WaveformDisplay for Deck 2.
    AutoMixComponent(DJAudioPlayer* player1, DJAudioPlayer* player2, PlaylistComponent* playlist, WaveformDisplay* waveform1, WaveformDisplay* waveform2);

    // Destructor: Cleans up the AutoMix component.
    ~AutoMixComponent() override;

    // Paint function: Draws the background and UI elements.
    // - g: Reference to the JUCE graphics context used for rendering.
    void paint(juce::Graphics&) override;

    // Resized function: Adjusts the layout when the component size changes.
    void resized() override;

    // Handles button click events (e.g., toggling Auto DJ on/off).
    // - button: Pointer to the button that was clicked.
    void buttonClicked(juce::Button* button) override;

    // Timer callback: Periodically checks track progress to trigger transitions.
    void timerCallback() override;

    // Starts the Auto DJ mode, loads tracks, and begins playback.
    void startAutoDJ();

    // Stops Auto DJ mode and resets playback control to manual.
    void stopAutoDJ();

    // Performs a crossfade transition between Deck 1 and Deck 2.
    void crossfade();

    // Retrieves the next track from the playlist.
    // Returns:
    // - A juce::File object representing the next track to be loaded.
    juce::File getNextTrack();

    // Applies the selected theme color to the component.
    void applyTheme();

    // Syncs BPM between decks and smoothly transitions between tracks.
    void syncAndCrossfade();

    // Performs a gradual crossfade effect over a given duration.
    // Parameters:
    // - durationInSeconds: The length of the fade transition in seconds.
    void gradualCrossfade(float durationInSeconds);


private:
    PlaylistComponent* playlistComponent; // Reference to the playlist manager
    DJAudioPlayer* player1; // Pointer to Deck 1 player
    DJAudioPlayer* player2; // Pointer to Deck 2 player
    PlaylistComponent* playlist; // Pointer to the playlist
    WaveformDisplay* waveform1; // Pointer to waveform visualization for Deck 1
    WaveformDisplay* waveform2; // Pointer to waveform visualization for Deck 2

    int currentDeck = 1; // Indicates which deck is currently active

    juce::TextButton autoDJButton{ "Auto DJ" }; // Button to toggle Auto DJ mode
    juce::Slider crossfadeSlider; // Slider for manual crossfade control
    juce::Colour backgroundColour = juce::Colours::black; // Default background color

    bool autoDJActive = false; // Tracks if Auto DJ mode is enabled
    float crossfadeValue = 0.5f; // Stores the current crossfade position
    int currentTrackIndex = 0; // Keeps track of the playlist index

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoMixComponent)
};
