#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "BeatVisualizer.h"
#include "JogWheelComponent.h"
#include "BPMComponent.h"

// DeckGUI provides the graphical interface for controlling a single DJ deck.
// It allows playback control, looping, BPM synchronization, visual feedback,
// and interactive elements like waveform display and jog wheel.
class DeckGUI : public juce::Component,
    public juce::Button::Listener,  // Handles button interactions
    public juce::Slider::Listener,  // Handles slider adjustments
    public juce::FileDragAndDropTarget, // Enables drag-and-drop track loading
    public juce::Timer, // Updates UI elements periodically
    public juce::KeyListener // Adds keyboard shortcuts for playback control
{
public:
    // Constructor: Initializes DeckGUI and links it to essential components.
    // - player: Pointer to the DJAudioPlayer instance associated with this deck.
    // - bpm: Pointer to the BPMComponent for beat synchronization.
    // - formatManager: Reference to the audio format manager (handles audio decoding).
    // - cache: Reference to the thumbnail cache for efficient waveform rendering.
    DeckGUI(DJAudioPlayer* player, BPMComponent* bpm, juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache);

    // Destructor: Cleans up resources when DeckGUI is destroyed.
    ~DeckGUI();

    // Timer callback: Periodically updates the waveform display and beat visualizer.
    void timerCallback() override;

    // Paint function: Renders the background and theme colors.
    // - g: Reference to the JUCE graphics context used for rendering.
    void paint(juce::Graphics&) override;

    // Applies the selected theme to buttons and UI elements.
    void applyTheme();

    // Resized function: Adjusts the layout dynamically when the window size changes.
    void resized() override;

    // Handles button clicks (Play, Pause, Stop, Load, Loop, BPM Sync).
    // - button: Pointer to the button that was clicked.
    void buttonClicked(juce::Button*) override;

    // Handles slider adjustments (Volume, Speed, Track Position).
    // - slider: Pointer to the slider that was adjusted.
    void sliderValueChanged(juce::Slider* slider) override;

    // Checks if the dragged file type is supported before allowing it to be dropped.
    // - files: List of files dragged into the deck.
    // Returns:
    // - true if the file is an MP3, WAV, or OGG file, false otherwise.
    bool isInterestedInFileDrag(const juce::StringArray& files) override;

    // Handles files dropped onto the deck, loading the selected track.
    // - files: List of files dropped onto the deck.
    // - x, y: Coordinates where the files were dropped.
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // Handles keyboard shortcuts for playback control.
    // - key: The key that was pressed.
    // - originatingComponent: The component that received the key event.
    // Returns:
    // - true if the key event was handled, false otherwise.
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;

    // Loads a new track into the waveform display.
    // - audioURL: The URL of the track to load.
    void updateWaveform(juce::URL audioURL);

private:
    JogWheelComponent jogWheel; // Interactive jog wheel for manual scrubbing

    // Playback control buttons
    juce::TextButton playButton{ " Play" };
    juce::TextButton pauseButton{ " Pause" };
    juce::TextButton stopButton{ " Stop" };

    // Additional control buttons
    juce::TextButton loadButton{ "LOAD" }; // Opens file picker for track selection
    juce::TextButton loopButton{ "LOOP" }; // Toggles looping mode
    juce::TextButton bpmSyncButton{ "SYNC BPM" }; // Synchronizes BPM with the other deck

    // Sliders for adjusting volume, speed, and position
    juce::Slider volSlider{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
    juce::Slider speedSlider{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
    juce::Slider posSlider{ juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight };

    juce::FileChooser fChooser{ "Select a file..." }; // File chooser for loading tracks

    DJAudioPlayer* player; // Pointer to the associated DJ audio player
    WaveformDisplay waveformDisplay; // Visual representation of the loaded track
    BeatVisualizer beatVisualizer; // Displays dynamic beat levels in real-time

    // Labels for sliders
    juce::Label volLabel;
    juce::Label speedLabel;
    juce::Label posLabel;

    // Configures playback buttons (Play, Pause, Stop)
    void configureButtons();

    // Configures sliders (Volume, Speed, Position)
    void configureSliders();

    // Theme colors for UI elements
    juce::Colour backgroundColour = juce::Colour(30, 30, 30);
    juce::Colour buttonColour = juce::Colour(50, 50, 50);
    juce::Colour textColour = juce::Colour(200, 200, 200);
    juce::Colour highlightColour = juce::Colour(0, 150, 250);

    // Beat jump buttons for quick navigation within the track
    juce::TextButton jumpForwardButton{ ">> Beat" };
    juce::TextButton jumpBackwardButton{ "<< Beat" };

    BPMComponent* bpmComponent;  // ✅ Pointer to the BPM component for tempo synchronization

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
