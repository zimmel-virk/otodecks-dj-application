#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "EQComponent.h"
#include "BPMComponent.h"
#include "AutoMixComponent.h"
#include "CrossfadeComponent.h"
#include "ThemeComponent.h"
#include "MicInputComponent.h"

// ✅ MainComponent: The central UI controller of the DJ application.
// - Manages all audio players, decks, effects, and UI components.
// - Inherits from `AudioAppComponent` to handle audio processing.
class MainComponent : public juce::AudioAppComponent
{
public:
    // ✅ Constructor: Initializes all components and UI elements.
    MainComponent();

    // ✅ Destructor: Cleans up resources when the component is deleted.
    ~MainComponent();

    // ✅ prepareToPlay: Prepares the audio system before playback starts.
    // - samplesPerBlockExpected: The number of audio samples per processing block.
    // - sampleRate: The audio sample rate (e.g., 44.1 kHz).
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // ✅ getNextAudioBlock: Fetches the next block of audio to be played.
    // - bufferToFill: The buffer where audio data should be written.
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // ✅ releaseResources: Releases audio resources when playback stops.
    void releaseResources() override;

    // ✅ paint: Draws the background and any custom UI elements.
    // - g: The graphics context used for rendering.
    void paint(juce::Graphics& g) override;

    // ✅ resized: Dynamically positions and sizes all UI components.
    void resized() override;

    // ✅ applyTheme: Changes the UI theme based on user selection.
    void applyTheme();

private:
    // ✅ Audio Format Manager: Handles audio file decoding.
    juce::AudioFormatManager formatManager;

    // ✅ Audio Thumbnail Cache: Stores waveforms for faster UI updates.
    juce::AudioThumbnailCache audioThumbnailCache{ 10 };

    // ✅ Crossfade Component: Manages smooth transitions between decks.
    CrossfadeComponent crossfadeComponent{ &player1, &player2 };

    // ✅ Theme Selector Dropdown: Allows users to switch UI themes.
    juce::ComboBox themeSelector;

    // ✅ Audio Players: Two separate players for Deck 1 and Deck 2.
    DJAudioPlayer player1{ formatManager };
    DJAudioPlayer player2{ formatManager };

    // ✅ Deck GUIs: User interfaces for controlling each deck.
    DeckGUI deckGUI1{ &player1, &bpmComponent, formatManager, audioThumbnailCache };
    DeckGUI deckGUI2{ &player2, &bpmComponent, formatManager, audioThumbnailCache };

    // ✅ Playlist Manager: Handles track loading and organization.
    PlaylistComponent playlistComponent{ &player1, &player2, &deckGUI1, &deckGUI2 };

    // ✅ Equalizer Component: Allows real-time adjustment of audio frequencies.
    EQComponent eqComponent;

    // ✅ BPM Sync Component: Matches beats per minute (BPM) between decks.
    BPMComponent bpmComponent{ &player1, &player2 };

    // ✅ Buttons for common controls
    juce::TextButton loadButton{ "Load Tracks" }; // ✅ Load new tracks.
    juce::TextButton syncBPMButton{ "Sync BPM" }; // ✅ Sync BPM between decks.
    juce::TextButton loopButton{ "Loop" }; // ✅ Enable/disable looping.

    // ✅ Audio Mixer: Combines both decks' audio into a single output.
    juce::MixerAudioSource mixerSource;

    // ✅ UI Colors: Customize the application's appearance.
    juce::Colour backgroundColour = juce::Colour(10, 10, 10); // ✅ Dark background.
    juce::Colour accentColour = juce::Colour(0, 180, 255); // ✅ Light blue accent.

    // ✅ Waveform Displays: Show visual representations of audio tracks.
    WaveformDisplay waveformDisplay1, waveformDisplay2;

    // ✅ AutoMix Component: Automatically transitions between tracks.
    AutoMixComponent autoMix;

    // ✅ Theme Manager: Handles UI styling.
    ThemeComponent themeComponent;

    // ✅ Microphone Input Component: Handles live audio input.
    MicInputComponent micInput;

    // ✅ Prevents accidental copying of the component.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
