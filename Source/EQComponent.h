#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h" // ✅ Required to use DJAudioPlayer*

// EQComponent is responsible for controlling Equalizer (EQ) settings such as Bass, Mid, Treble, Reverb, and Echo.
// It provides UI sliders to adjust each parameter and buttons to save/load EQ presets.
class EQComponent : public juce::Component,
    public juce::Slider::Listener,  // ✅ Allows this class to handle slider events.
    public juce::Button::Listener   // ✅ Allows this class to handle button events.
{
public:
    // Constructor: Initializes EQ sliders, labels, and buttons.
    EQComponent();

    // Destructor: Cleans up the EQComponent resources.
    ~EQComponent() override;

    // 🎚 Equalizer Sliders (Controls Bass, Mid, Treble, Reverb, and Echo)
    juce::Slider bassSlider;   // ✅ Controls bass frequencies.
    juce::Slider midSlider;    // ✅ Controls mid frequencies.
    juce::Slider trebleSlider; // ✅ Controls treble frequencies.
    juce::Slider reverbSlider; // ✅ Controls reverb effect.
    juce::Slider echoSlider;   // ✅ Controls echo effect.

    // 🏷 Labels for the sliders
    juce::Label bassLabel;
    juce::Label midLabel;
    juce::Label trebleLabel;
    juce::Label reverbLabel;
    juce::Label echoLabel;

    // ✅ Paints the EQ UI background.
    void paint(juce::Graphics&) override;

    // ✅ Resizes and arranges all UI elements dynamically.
    void resized() override;

    // ✅ Handles changes in slider values and applies corresponding effects.
    void sliderValueChanged(juce::Slider* slider) override;

    // ✅ Handles button clicks for saving and loading EQ presets.
    void buttonClicked(juce::Button* button) override;

    // ✅ Saves the current EQ settings to a file.
    void saveEQPreset();

    // ✅ Loads EQ settings from a previously saved file.
    void loadEQPreset();

    // ✅ Links the EQComponent to a specific DJAudioPlayer instance.
    void setPlayer(DJAudioPlayer* newPlayer);

    // ✅ Applies theme colors to UI elements.
    void applyTheme();

private:
    // 🎛 Preset Buttons for saving and loading EQ settings.
    juce::TextButton savePresetButton{ "Save Preset" }; // ✅ Saves EQ settings.
    juce::TextButton loadPresetButton{ "Load Preset" }; // ✅ Loads previously saved EQ settings.

    DJAudioPlayer* player = nullptr; // ✅ Stores reference to the DJAudioPlayer.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQComponent)
};
