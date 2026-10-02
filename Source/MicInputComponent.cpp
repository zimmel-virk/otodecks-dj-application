#include "MicInputComponent.h"

// ✅ Constructor: Initializes the microphone input component.
// - Sets up the UI with buttons for recording, stopping, effects, and opening files.
// - Configures the audio device to handle microphone input.
MicInputComponent::MicInputComponent()
{
    setSize(400, 200); // ✅ Set initial component size.

    // ✅ Make all buttons visible in the UI.
    addAndMakeVisible(recordButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(effectsButton);
    addAndMakeVisible(openFileButton);  // ✅ Add "Play Recording" button.

    // ✅ Assign event listeners to buttons.
    recordButton.addListener(this);
    stopButton.addListener(this);
    effectsButton.addListener(this);
    openFileButton.addListener(this);

    // ✅ Set button labels.
    recordButton.setButtonText("Start Recording");
    stopButton.setButtonText("Stop Recording");
    effectsButton.setButtonText("Apply Effect");

    juce::Logger::writeToLog("🎛 Buttons Created!");

    // ✅ Initialize audio device for 1 input (microphone) and 2 outputs (stereo).
    juce::String error = deviceManager.initialise(1, 2, nullptr, true);
    if (!error.isEmpty())
    {
        juce::Logger::writeToLog("❌ Audio Device Error: " + error);
    }

    // ✅ Set up audio channels for recording.
    setAudioChannels(1, 2);
}

// ✅ Destructor: Cleans up the audio system when the component is destroyed.
MicInputComponent::~MicInputComponent()
{
    shutdownAudio();
}

// ✅ prepareToPlay: Prepares the microphone buffer for recording.
// - samplesPerBlockExpected: Expected number of samples per processing block.
// - sampleRate: The audio sample rate (e.g., 44.1 kHz).
void MicInputComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    micBuffer.setSize(1, samplesPerBlockExpected);
}

// ✅ getNextAudioBlock: Captures microphone input and records it.
// - bufferToFill: The buffer where audio data should be stored.
void MicInputComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!isRecording)
    {
        bufferToFill.clearActiveBufferRegion(); // ✅ Stop live mic playback.
        return;
    }

    // ✅ Fetch the current audio device.
    auto* reader = deviceManager.getCurrentAudioDevice();
    if (reader == nullptr) return;

    // ✅ Capture the input audio buffer.
    auto* inputBuffer = bufferToFill.buffer->getReadPointer(0);

    // ✅ Write the recorded audio to a file.
    if (isRecording && outputStream)
    {
        outputStream->write(inputBuffer, bufferToFill.numSamples * sizeof(float));
    }

    bufferToFill.clearActiveBufferRegion(); // ✅ Prevent direct microphone playback.
}

// ✅ releaseResources: Frees up any allocated audio resources.
void MicInputComponent::releaseResources()
{
}

// ✅ paint: Draws the UI for the component.
// - g: The graphics context used for rendering.
void MicInputComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // ✅ Background color.
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("Mic Input & Recording", getLocalBounds(), juce::Justification::centredTop);
}

// ✅ resized: Positions UI components dynamically.
void MicInputComponent::resized()
{
    recordButton.setBounds(10, 40, 120, 40);
    stopButton.setBounds(140, 40, 120, 40);
    effectsButton.setBounds(270, 40, 120, 40);
    openFileButton.setBounds(170, 90, 150, 40);  // ✅ Position "Play Recording" button.
}

// ✅ buttonClicked: Handles button click events.
// - button: The button that was clicked.
void MicInputComponent::buttonClicked(juce::Button* button)
{
    juce::Logger::writeToLog("📢 Button Clicked!");

    if (button == &recordButton)
    {
        juce::Logger::writeToLog("🟢 Record Button Clicked!");
        startRecording();
    }
    else if (button == &stopButton)
    {
        juce::Logger::writeToLog("🔴 Stop Button Clicked!");
        stopRecording();
    }
    else if (button == &effectsButton)
    {
        selectedEffect = (selectedEffect + 1) % 4;
        juce::String effectNames[] = { "None", "Echo", "Pitch Shift", "Robot Voice" };
        effectsButton.setButtonText("Effect: " + effectNames[selectedEffect]);
    }
    else if (button == &openFileButton)  // ✅ Open file when clicked.
    {
        if (outputFile.existsAsFile())
        {
            DBG("📂 Opening Recording: " + outputFile.getFullPathName());
            outputFile.revealToUser();  // ✅ Open file in Explorer/Finder.
        }
        else
        {
            DBG("❌ ERROR: No recording file exists!");
        }
    }
}

// ✅ startRecording: Starts capturing microphone audio and saves it to a file.
void MicInputComponent::startRecording()
{
    DBG("📢 `startRecording()` function called!");

    // ✅ Save the file in a temporary directory instead of OneDrive/Documents.
    outputFile = juce::File::getSpecialLocation(juce::File::tempDirectory)
        .getChildFile("MicRecording.wav");

    DBG("📁 Attempting to create file: " + outputFile.getFullPathName());

    if (!outputFile.create())
    {
        DBG("❌ ERROR: Could not create the recording file!");
        return;
    }

    DBG("✅ File successfully created: " + outputFile.getFullPathName());

    // ✅ Open the file output stream for writing audio data.
    outputStream = std::make_unique<juce::FileOutputStream>(outputFile);
    if (!outputStream || !outputStream->openedOk())
    {
        DBG("❌ ERROR: FileOutputStream could not be opened! Check file permissions.");
        return;
    }

    isRecording = true;
    DBG("🎙️ Recording Started!");
}

// ✅ stopRecording: Stops capturing microphone input and saves the file.
void MicInputComponent::stopRecording()
{
    if (isRecording)
    {
        isRecording = false;
        outputStream.reset(); // ✅ Close the file safely.

        if (outputFile.existsAsFile())
        {
            DBG("✅ Recording saved: " + outputFile.getFullPathName());

            // ✅ Open the file location in Explorer/Finder.
            outputFile.revealToUser();
        }
        else
        {
            DBG("❌ ERROR: Recording file does not exist!");
        }
    }
}

// ✅ applyEffect: Applies audio effects to the recorded buffer.
// - buffer: The audio buffer containing the microphone input.
void MicInputComponent::applyEffect(juce::AudioBuffer<float>& buffer)
{
    if (selectedEffect == 1) // ✅ Echo Effect
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            for (int sample = buffer.getNumSamples() - 1; sample > 0; --sample)
            {
                buffer.getWritePointer(channel)[sample] += 0.6f * buffer.getReadPointer(channel)[sample - 1];
            }
        }
    }
    else if (selectedEffect == 2) // ✅ Pitch Shift Effect (Simplified)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                buffer.getWritePointer(channel)[sample] *= 1.2f; // Increase pitch.
            }
        }
    }
    else if (selectedEffect == 3) // ✅ Robot Voice Effect
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            for (int sample = 0; sample < buffer.getNumSamples(); sample += 2)
            {
                buffer.getWritePointer(channel)[sample] = -buffer.getReadPointer(channel)[sample];
            }
        }
    }
}
