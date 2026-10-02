#pragma once
#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h> // ✅ Required for DSP (Digital Signal Processing) effects

// DJAudioPlayer handles audio playback, processing, and effects for a DJ deck.
// It provides features like track loading, playback controls, EQ adjustments, BPM detection, looping, and more.
class DJAudioPlayer : public juce::AudioSource
{
public:
    // Constructor: Initializes the audio player and links it to the format manager.
    // - formatManager: Reference to the JUCE audio format manager responsible for handling different audio formats.
    DJAudioPlayer(juce::AudioFormatManager& formatManager);

    // Destructor: Cleans up resources when DJAudioPlayer is destroyed.
    ~DJAudioPlayer();

    // Prepares the player for playback.
    // - samplesPerBlockExpected: Expected number of samples per audio processing block.
    // - sampleRate: The sample rate of the audio in Hz.
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    // Processes and fills the next audio buffer block.
    // - bufferToFill: Reference to the buffer that will receive the processed audio samples.
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Releases allocated resources when playback stops.
    void releaseResources() override;

    // ✅ Audio Playback Functions
    void loadURL(juce::URL audioURL); // Loads an audio file from a URL.
    void start();  // Starts playback.
    void stop();   // Stops playback and resets position.
    void pause();  // Pauses playback without resetting position.
    void toggleLoop(); // Toggles looping mode on/off.
    bool isLooping();  // Checks if looping is enabled.

    // ✅ Track Positioning
    void setPosition(double posInSecs);  // Sets playback position in seconds.
    void setPositionRelative(double pos); // Sets playback position as a fraction (0.0 to 1.0).
    double getPositionRelative(); // Gets the current playback position as a fraction.

    // ✅ Playback Speed & BPM Control
    void setSpeed(double ratio);  // Adjusts playback speed (e.g., for pitch control).
    double getSpeed();  // Returns the current playback speed.
    void setBPM(double newBPM); // Sets the BPM manually.
    double getBPM(); // Returns the detected BPM of the track.
    float detectBPM();  // ✅ Estimates track BPM using audio analysis.
    juce::String detectKey(); // ✅ Detects the musical key of the loaded track.

    // ✅ Beat Jumping
    void jumpByBeats(int beats); // Moves playback position forward/backward by a set number of beats.

    // ✅ Gain (Volume) Control
    void setGain(double gain); // Sets the playback volume.
    double getGain() const; // Returns the current gain level.

    // ✅ Real-Time Audio Analysis
    float getCurrentAudioLevel(); // Returns the peak audio level for visualization.

    // ✅ Track Metadata
    juce::URL getCurrentTrackURL(); // Returns the URL of the currently loaded track.

    // ✅ Equalizer (EQ) & Effects Control
    void setBass(float level);   // Adjusts the bass level.
    void setMid(float level);    // Adjusts the midrange level.
    void setTreble(float level); // Adjusts the treble level.
    void setReverb(float level); // Adjusts the reverb level.
    void setEcho(float level);   // Adjusts the echo level.
    void updateEQ(); // ✅ Applies EQ settings to the audio source.

    // ✅ Audio Processing
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages); // Processes audio for effects.

private:
    juce::AudioFormatManager& formatManager; // ✅ Reference to the audio format manager.
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource; // ✅ Stores the loaded audio file.
    juce::AudioTransportSource transportSource; // ✅ Handles playback.
    juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 }; // ✅ Handles speed adjustments.

    float currentLevel = 0.0f; // ✅ Stores real-time audio level for visual effects.
    bool looping = false; // ✅ Looping state (enabled/disabled).
    double bpm = 120.0;  // ✅ Default BPM if detection fails.
    double trackBPM = 120.0; // ✅ Detected BPM of the track.
    double playbackSpeed = 1.0; // ✅ Stores the current playback speed.
    juce::URL currentTrackURL; // ✅ Stores the currently loaded track's URL.
    double currentSampleRate = 44100.0; // ✅ Stores the current sample rate.

    // ✅ EQ Parameters
    float bassLevel = 0.0f;
    float midLevel = 0.0f;
    float trebleLevel = 0.0f;
    float reverbLevel = 0.0f;
    float echoLevel = 0.0f;

    // ✅ Digital Signal Processing (DSP) Chain
    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,  // ✅ Bass Filter
        juce::dsp::IIR::Filter<float>,  // ✅ Mid Filter
        juce::dsp::IIR::Filter<float>,  // ✅ Treble Filter
        juce::dsp::Reverb,              // ✅ Reverb Effect
        juce::dsp::DelayLine<float>     // ✅ Echo Effect
    > eqChain;

    juce::dsp::ProcessSpec spec; // ✅ DSP processing specifications.

    // ✅ Initializes audio playback by setting up the DSP chain.
    void prepareToPlay(double sampleRate, int samplesPerBlock);

    // ✅ Applies EQ settings to the audio buffer.
    void applyEQ(juce::AudioBuffer<float>& buffer);

    juce::AudioSourcePlayer audioSourcePlayer; // ✅ Handles audio playback.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DJAudioPlayer)
};
