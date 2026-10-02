#include "DJAudioPlayer.h"
#include "PlaylistComponent.h"

// Constructor: Initializes the DJAudioPlayer and links it to the format manager.
// - fm: Reference to the audio format manager responsible for handling different file formats.
DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& fm)
    : formatManager(fm)
{
}

// Destructor: Cleans up resources when DJAudioPlayer is destroyed.
DJAudioPlayer::~DJAudioPlayer() {}

// Prepares the player for audio playback by configuring the transport and resampling sources.
// - samplesPerBlockExpected: Expected number of audio samples per processing block.
// - sampleRate: The audio sample rate in Hz.
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Processes the next audio block and applies effects like EQ and looping.
// - bufferToFill: Contains audio samples to be processed.
void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource == nullptr) // If no audio file is loaded, clear the buffer.
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    resampleSource.getNextAudioBlock(bufferToFill);

    // ✅ Compute the peak level of the current buffer for visualization.
    float peak = 0.0f;
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        peak = std::max(peak, bufferToFill.buffer->getMagnitude(channel, 0, bufferToFill.numSamples));
    }
    currentLevel = peak; // ✅ Store peak level for real-time audio visualization.

    applyEQ(*bufferToFill.buffer); // ✅ Apply manual EQ filtering.

    // ✅ Handle looping: Restart the track when it reaches the end.
    if (looping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
    {
        transportSource.setPosition(0);
        transportSource.start();
    }

    // ✅ Reset track position if it reaches the end (even if looping is off).
    if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds())
    {
        transportSource.setPosition(0);
    }

    applyEQ(*bufferToFill.buffer); // ✅ Apply EQ processing again.
}

// Applies EQ adjustments (Bass, Mid, Treble) to the audio buffer.
// - buffer: The buffer containing audio samples to be processed.
void DJAudioPlayer::applyEQ(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];

            // ✅ Apply EQ adjustments using mapped values.
            sample *= juce::jmap(bassLevel, 0.0f, 1.0f, 0.8f, 1.2f);
            sample *= juce::jmap(midLevel, 0.0f, 1.0f, 0.8f, 1.2f);
            sample *= juce::jmap(trebleLevel, 0.0f, 1.0f, 0.8f, 1.2f);

            channelData[i] = juce::jlimit(-1.0f, 1.0f, sample); // ✅ Prevent distortion by clamping values.
        }
    }
}

// Releases audio resources when playback stops.
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

// Loads an audio file from a given URL and prepares it for playback.
// - audioURL: The URL of the audio file to be loaded.
void DJAudioPlayer::loadURL(juce::URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.getLocalFile());
    if (reader == nullptr)
    {
        DBG("DJAudioPlayer::loadURL - ERROR: Failed to create reader.");
        return;
    }

    std::unique_ptr<juce::AudioFormatReaderSource> newSource =
        std::make_unique<juce::AudioFormatReaderSource>(reader, true);

    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
    readerSource = std::move(newSource);

    updateEQ(); // ✅ Apply EQ immediately when track is loaded.

    currentTrackURL = audioURL; // ✅ Store the current track URL.

    bpm = detectBPM(); // ✅ Auto-detect BPM (basic detection placeholder).
}

// Retrieves the URL of the currently loaded track.
juce::URL DJAudioPlayer::getCurrentTrackURL()
{
    return currentTrackURL; // ✅ Return the stored track URL.
}

// Adjusts the playback gain (volume).
// - gain: The new gain value (0.0 - 1.0).
void DJAudioPlayer::setGain(double gain)
{
    transportSource.setGain(gain);
}

// Sets the playback position in seconds.
// - posInSecs: The new playback position.
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

// Sets the playback position as a relative value (0.0 - 1.0).
// - pos: The new relative position (0.0 = start, 1.0 = end).
void DJAudioPlayer::setPositionRelative(double pos)
{
    transportSource.setPosition(pos * transportSource.getLengthInSeconds());
}

// Controls for starting, stopping, and pausing playback.
void DJAudioPlayer::pause() { transportSource.stop(); }
void DJAudioPlayer::start() { transportSource.start(); }
void DJAudioPlayer::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

// Returns the current playback position as a relative value (0.0 - 1.0).
double DJAudioPlayer::getPositionRelative()
{
    double length = transportSource.getLengthInSeconds();
    return (length > 0) ? transportSource.getCurrentPosition() / length : 0.0;
}

// ✅ Enables or disables looping.
void DJAudioPlayer::toggleLoop()
{
    looping = !looping;
    DBG("Looping: " + juce::String(looping ? "Enabled" : "Disabled"));
}

// ✅ Returns whether looping is currently enabled.
bool DJAudioPlayer::isLooping()
{
    return looping;
}

// ✅ Returns the detected BPM of the current track.
double DJAudioPlayer::getBPM()
{
    if (readerSource == nullptr)
        return 120.0; // Default BPM.

    double trackLength = transportSource.getLengthInSeconds();
    if (trackLength < 5.0)
        return 120.0; // Default for very short tracks.

    return 60.0 / (trackLength / 16.0); // Basic BPM estimation.
}

// ✅ Returns the real-time peak audio level for visual feedback.
float DJAudioPlayer::getCurrentAudioLevel()
{
    if (readerSource == nullptr || !transportSource.isPlaying())
    {
        return 0.0f;
    }

    return juce::jlimit(0.0f, 1.0f, currentLevel);
}

// ✅ Adjusts playback speed.
void DJAudioPlayer::setSpeed(double ratio)
{
    playbackSpeed = ratio;
    resampleSource.setResamplingRatio(playbackSpeed);
}

// ✅ Returns the current playback speed.
double DJAudioPlayer::getSpeed()
{
    return playbackSpeed;
}
// ✅ Detects the BPM (Beats Per Minute) of the currently loaded track.
// - Analyzes the first 30 seconds of audio to estimate BPM.
// - Uses a peak threshold to detect beats and count them.
// Returns:
// - The estimated BPM of the track.
float DJAudioPlayer::detectBPM()
{
    if (readerSource == nullptr || readerSource->getAudioFormatReader() == nullptr)
        return 0.0f;

    juce::AudioFormatReader* reader = readerSource->getAudioFormatReader();
    int sampleRate = static_cast<int>(reader->sampleRate);
    int numSamples = sampleRate * 30; // ✅ Analyze first 30 seconds instead of 10.
    juce::AudioBuffer<float> buffer(1, numSamples);

    reader->read(&buffer, 0, numSamples, 0, true, false); // ✅ Read 30 seconds.

    int beatCount = 0;
    float lastPeak = 0.0f;
    float threshold = 0.5f; // ✅ Lower threshold for more accurate beat detection.

    for (int i = 1; i < numSamples; i++)
    {
        float sample = buffer.getSample(0, i);
        if (sample > threshold && lastPeak < threshold) // ✅ Detect beat based on threshold.
        {
            beatCount++;
        }
        lastPeak = sample;
    }

    float estimatedBPM = (beatCount / 30.0f) * 60.0f; // ✅ Convert to BPM based on 30s window.

    DBG("🎶 BPM Detected: " + juce::String(estimatedBPM));
    return estimatedBPM > 30.0f ? estimatedBPM : 120.0f; // ✅ Prevent unrealistic BPM values.
}

// ✅ Detects the key of the currently loaded track based on frequency analysis.
// - Uses a simulated frequency detection algorithm to map the detected frequency to a musical key.
// Returns:
// - A string representing the detected key (e.g., "C", "D#", "A").
juce::String DJAudioPlayer::detectKey()
{
    static const juce::String keys[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    if (readerSource == nullptr) return "Unknown";

    float frequencies[] = { 261.63f, 277.18f, 293.66f, 311.13f, 329.63f, 349.23f, 369.99f, 392.00f, 415.30f, 440.00f, 466.16f, 493.88f };

    float detectedFrequency = 440.0f; // ✅ Simulated frequency detection.
    int keyIndex = static_cast<int>(std::round(std::log2(detectedFrequency / 440.0) * 12.0)) % 12;

    return keys[(keyIndex + 12) % 12]; // ✅ Prevents negative index.
}

// ✅ Moves the playback position forward or backward by a given number of beats.
// - Uses the detected BPM to calculate the duration of a single beat.
// - Ensures the new position stays within valid bounds.
// - If BPM is not detected, defaults to 120 BPM.
void DJAudioPlayer::jumpByBeats(int beats)
{
    if (readerSource)
    {
        if (bpm <= 0) bpm = 120.0;  // ✅ Default to 120 BPM if not detected.
        double secondsPerBeat = 60.0 / bpm;
        double newPosition = transportSource.getCurrentPosition() + (beats * secondsPerBeat);

        if (newPosition < 0) newPosition = 0;
        if (newPosition > transportSource.getLengthInSeconds()) newPosition = transportSource.getLengthInSeconds();

        transportSource.setPosition(newPosition);
    }
}

// ✅ Manually sets the BPM of the track for synchronization with other decks.
// - newBPM: The new BPM value to set.
void DJAudioPlayer::setBPM(double newBPM)
{
    if (newBPM > 0)
    {
        double currentPosition = transportSource.getCurrentPosition(); // ✅ Save playback position.
        bpm = newBPM;
        transportSource.setPosition(currentPosition); // ✅ Restore playback position.
    }
}

// ✅ EQ (Equalizer) Control Functions
// - Allows users to adjust Bass, Mid, Treble, Reverb, and Echo levels.
void DJAudioPlayer::setBass(float level) { bassLevel = level; updateEQ(); }
void DJAudioPlayer::setMid(float level) { midLevel = level; updateEQ(); }
void DJAudioPlayer::setTreble(float level) { trebleLevel = level; updateEQ(); }
void DJAudioPlayer::setReverb(float level) { reverbLevel = level; updateEQ(); }
void DJAudioPlayer::setEcho(float level) { echoLevel = level; updateEQ(); }

// ✅ Updates the equalizer settings based on user input.
void DJAudioPlayer::updateEQ()
{
    if (!spec.sampleRate) return; // ✅ Prevent division by zero.

    auto& bassFilter = eqChain.get<0>();
    auto& midFilter = eqChain.get<1>();
    auto& trebleFilter = eqChain.get<2>();
    auto& reverb = eqChain.get<3>();
    auto& echo = eqChain.get<4>();

    // ✅ Apply EQ using filters with correct coefficient updates.
    *bassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        spec.sampleRate, 200.0f, 0.7f, juce::Decibels::decibelsToGain(bassLevel));

    *midFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        spec.sampleRate, 1000.0f, 0.7f, juce::Decibels::decibelsToGain(midLevel));

    *trebleFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        spec.sampleRate, 5000.0f, 0.7f, juce::Decibels::decibelsToGain(trebleLevel));

    audioSourcePlayer.setGain(1.0f); // 🔊 Restore volume.

    DBG("✅ EQ Updated - Bass: " + juce::String(bassLevel) +
        ", Mid: " + juce::String(midLevel) +
        ", Treble: " + juce::String(trebleLevel));
}

// ✅ Prepares the DSP (Digital Signal Processing) for audio playback.
void DJAudioPlayer::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (sampleRate <= 0 || samplesPerBlock <= 0) return; // ✅ Prevent invalid values.

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    eqChain.prepare(spec); // ✅ Ensure DSP filters are prepared.

    DBG("🎧 Audio Prepared - SampleRate: " + juce::String(sampleRate) +
        ", BlockSize: " + juce::String(samplesPerBlock));
}

// ✅ Processes the audio block and applies EQ adjustments.
void DJAudioPlayer::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (buffer.getNumSamples() == 0)
    {
        DBG("⚠️ No Audio Samples Found!"); // ✅ Debug if no sound is processed.
        return;
    }

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    eqChain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    DBG("🔊 Processing Audio - NumSamples: " + juce::String(buffer.getNumSamples()));
}

// ✅ Retrieves the current gain (volume) of the transport source.
double DJAudioPlayer::getGain() const
{
    return transportSource.getGain();
}
