#pragma once
#include <JuceHeader.h>

// 🎤 **MicInputComponent: Handles microphone input and recording**
// - Captures live microphone audio.
// - Saves recordings to a file.
// - Applies real-time audio effects (Echo, Pitch Shift, Robot Voice).
class MicInputComponent : public juce::AudioAppComponent, public juce::Button::Listener
{
public:
    // ✅ Constructor: Initializes microphone input, UI buttons, and audio settings.
    MicInputComponent();

    // ✅ Destructor: Cleans up allocated resources.
    ~MicInputComponent() override;

    // 🎛 **Audio Processing Methods**

    // ✅ prepareToPlay: Prepares the audio device for recording.
    // - samplesPerBlockExpected: The expected number of samples per block.
    // - sampleRate: The audio sample rate (e.g., 44.1 kHz).
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // ✅ getNextAudioBlock: Captures the next block of microphone input.
    // - bufferToFill: Stores the recorded audio data.
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // ✅ releaseResources: Frees up any allocated audio resources.
    void releaseResources() override;

    // 🎨 **UI Methods**

    // ✅ paint: Draws the component UI.
    // - g: The JUCE graphics object used for rendering.
    void paint(juce::Graphics& g) override;

    // ✅ resized: Adjusts UI elements dynamically when the window is resized.
    void resized() override;

    // ✅ buttonClicked: Handles button press events.
    // - button: The button that was clicked.
    void buttonClicked(juce::Button* button) override;

private:
    // 🎙 **Audio Device & Buffer**
    juce::AudioDeviceManager deviceManager;  // ✅ Manages audio input/output.
    juce::AudioBuffer<float> micBuffer;      // ✅ Stores recorded microphone audio.

    // 🎛 **UI Buttons**
    juce::TextButton recordButton{ "Start Recording" };  // ✅ Start recording audio.
    juce::TextButton stopButton{ "Stop Recording" };     // ✅ Stop recording.
    juce::TextButton effectsButton{ "Apply Effect" };    // ✅ Apply an audio effect.
    juce::TextButton openFileButton{ "Open Recording" }; // ✅ Open saved recording.

    // 💾 **Recording File & Stream**
    juce::File outputFile;  // ✅ File to store the recorded audio.
    std::unique_ptr<juce::FileOutputStream> outputStream; // ✅ Stream to write audio data.

    // 🎵 **Recording State & Effects**
    bool isRecording = false;  // ✅ Tracks whether recording is active.
    int selectedEffect = 0;    // ✅ Selected audio effect (0=None, 1=Echo, 2=Pitch Shift, 3=Robot Voice).

    // 🎛 **Internal Methods**

    // ✅ startRecording: Begins capturing microphone input.
    void startRecording();

    // ✅ stopRecording: Stops recording and saves the audio file.
    void stopRecording();

    // ✅ applyEffect: Applies an audio effect (Echo, Pitch Shift, or Robot Voice).
    // - buffer: The audio buffer containing the microphone input.
    void applyEffect(juce::AudioBuffer<float>& buffer);

    // 🚨 Prevents copying and memory leaks.
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MicInputComponent)
};
