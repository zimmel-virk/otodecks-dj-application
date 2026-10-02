#include "AutoMixComponent.h"
#include "ThemeComponent.h"
#include "PlaylistComponent.h" 

// Constructor: Initializes the AutoMix component and sets up UI elements.
// Parameters:
// - _player1: Pointer to the first DJAudioPlayer instance (Deck 1).
// - _player2: Pointer to the second DJAudioPlayer instance (Deck 2).
// - _playlist: Pointer to the PlaylistComponent, used for track management.
// - _waveform1: Pointer to the WaveformDisplay for Deck 1.
// - _waveform2: Pointer to the WaveformDisplay for Deck 2.
AutoMixComponent::AutoMixComponent(DJAudioPlayer* _player1, DJAudioPlayer* _player2, PlaylistComponent* _playlist, WaveformDisplay* _waveform1, WaveformDisplay* _waveform2)
    : player1(_player1), player2(_player2), playlist(_playlist), waveform1(_waveform1), waveform2(_waveform2)
{
    // Add the Auto DJ button and assign it as a listener for click events.
    autoDJButton.addListener(this); // Listens for user interactions (button clicks).
    addAndMakeVisible(autoDJButton); // Ensures the button is visible in the UI.

    // Configure the crossfade slider to smoothly transition between decks.
    crossfadeSlider.setRange(0.0, 1.0, 0.01); // Sets slider range (0 = full Deck 1, 1 = full Deck 2).
    crossfadeSlider.setValue(0.5); // Default value set to the middle (equal mix of both decks).

    // Assign an event listener for slider value changes.
    crossfadeSlider.onValueChange = [this] {
        crossfadeValue = crossfadeSlider.getValue(); // Updates the crossfade position when slider moves.
        };

    addAndMakeVisible(crossfadeSlider); // Ensures the slider is displayed in the UI.
}

// Destructor: Cleans up the AutoMix component when it is no longer needed.
AutoMixComponent::~AutoMixComponent() {}


void AutoMixComponent::paint(juce::Graphics& g)
{
    // Fills the background of the component with a dark color (RGB: 20, 20, 20).
    g.fillAll(juce::Colour(20, 20, 20));
}

void AutoMixComponent::resized()
{
    // Sets the position and size of the Auto DJ button.
    autoDJButton.setBounds(10, 10, getWidth() - 20, 40);

    // Sets the position and size of the crossfade slider.
    crossfadeSlider.setBounds(10, 60, getWidth() - 20, 30);
}

void AutoMixComponent::buttonClicked(juce::Button* button)
{
    // Checks if the clicked button is the Auto DJ button.
    if (button == &autoDJButton)
    {
        // If Auto DJ is already active, stop it; otherwise, start it.
        if (autoDJActive)
        {
            stopAutoDJ();
        }
        else
        {
            startAutoDJ();
        }
    }
}

void AutoMixComponent::startAutoDJ()
{
    DBG("🟢 Auto DJ Button Clicked");

    // Check if the playlist exists before attempting to access tracks.
    if (playlist == nullptr)
    {
        DBG("❌ ERROR: playlist is NULL! Auto DJ cannot start.");
        return;
    }

    // Get the number of tracks in the playlist.
    int trackCount = playlist->getNumRows();
    DBG("📂 Tracks in Playlist: " + juce::String(trackCount));

    // Ensure there are at least two tracks to enable Auto DJ.
    if (trackCount < 2)
    {
        DBG("⚠️ ERROR: Not enough tracks in the playlist to start Auto DJ!");
        return;
    }

    // Retrieve the first two tracks from the playlist.
    juce::File firstTrack = playlist->getTrackFile(0);
    juce::File secondTrack = playlist->getTrackFile(1);

    // Validate that the files exist before attempting to load them.
    if (!firstTrack.existsAsFile() || !secondTrack.existsAsFile())
    {
        DBG("❌ ERROR: One or both tracks do not exist on disk.");
        return;
    }

    // Load the first track into Player 1 and update its waveform display.
    DBG("🎵 Loading First Track into Player 1...");
    player1->loadURL(juce::URL(firstTrack));
    waveform1->loadURL(juce::URL(firstTrack));

    // Load the second track into Player 2 and update its waveform display.
    DBG("🎵 Loading Second Track into Player 2...");
    player2->loadURL(juce::URL(secondTrack));
    waveform2->loadURL(juce::URL(secondTrack));

    // Start playback of the first track immediately.
    DBG("▶️ Starting Playback...");
    player1->start(); // ✅ Start playing Deck 1 immediately

    // Mark Auto DJ as active and update the button text.
    autoDJActive = true;
    autoDJButton.setButtonText("Stop Auto DJ");

    // Start a timer that checks track progress every second.
    startTimer(1000); // ✅ Auto DJ timer runs every 1 second.

    DBG("🎛 Auto DJ Started Successfully!");
}

void AutoMixComponent::stopAutoDJ()
{
    // Disable Auto DJ mode.
    autoDJActive = false;

    // Reset the button text back to "Auto DJ".
    autoDJButton.setButtonText("Auto DJ");

    // Stop the timer that checks track progress.
    stopTimer(); // ✅ Stops AutoMix timer when Auto DJ is disabled.

    DBG("🔴 Auto DJ Stopped.");
}

void AutoMixComponent::timerCallback()
{
    // Exit early if Auto DJ is not active.
    if (!autoDJActive) return;

    // Get the current playback position of the active deck.
    float position = (currentDeck == 1) ? player1->getPositionRelative() : player2->getPositionRelative();
    DBG("⏳ Current Track Position: " + juce::String(position));

    // If the track has reached 85% of its duration, trigger the transition.
    if (position >= 0.85)  // ✅ AutoMix triggers transition at 85% completion.
    {
        DBG("🔀 AutoMix Triggered: Switching to Next Track...");
        syncAndCrossfade();
    }
}



void AutoMixComponent::crossfade()
{
    // Duration of the crossfade effect in seconds.
    float fadeDuration = 3.0f;

    // Number of incremental steps for smooth volume transition.
    int fadeSteps = 30;

    // Get the next track from the playlist.
    juce::File nextTrack = getNextTrack();
    if (nextTrack.existsAsFile())
    {
        // Load the next track into Deck 2 and update its waveform display.
        player2->loadURL(juce::URL(nextTrack));
        waveform2->loadURL(juce::URL(nextTrack));
    }

    // Perform gradual crossfade by adjusting gains over multiple steps.
    for (int i = 0; i <= fadeSteps; ++i)
    {
        float gain = static_cast<float>(i) / static_cast<float>(fadeSteps);
        player1->setGain(1.0f - gain);  // Reduce Deck 1's volume.
        player2->setGain(gain);         // Increase Deck 2's volume.

        // Pause execution briefly to create a smooth transition effect.
        juce::Thread::sleep(fadeDuration * 1000 / fadeSteps);
    }

    // Swap players and waveform displays after transition completes.
    std::swap(player1, player2);
    std::swap(waveform1, waveform2);
}

juce::File AutoMixComponent::getNextTrack()
{
    // Ensure playlist exists and contains tracks before proceeding.
    if (playlist == nullptr || playlist->getNumRows() == 0)
    {
        DBG("⚠️ ERROR: Playlist is NULL or empty.");
        return juce::File();  // Return an empty file to indicate failure.
    }

    // Move to the next track in the playlist, cycling back to the start if needed.
    currentTrackIndex = (currentTrackIndex + 1) % playlist->getNumRows();
    juce::File nextTrack = playlist->getTrackFile(currentTrackIndex);

    // Validate that the track file exists before loading.
    if (!nextTrack.existsAsFile())
    {
        DBG("❌ ERROR: Track file does not exist: " + nextTrack.getFullPathName());
        return juce::File();
    }

    DBG("🎶 Next Track Selected: " + nextTrack.getFileName());
    return nextTrack;
}

void AutoMixComponent::applyTheme()
{
    // Retrieve the background color from the ThemeComponent.
    backgroundColour = ThemeComponent::getBackgroundColor();

    // Refresh the UI to apply the new theme.
    repaint();
}

void AutoMixComponent::syncAndCrossfade()
{
    // Ensure the playlist has tracks before attempting to sync and crossfade.
    if (playlist->getNumRows() == 0)
    {
        DBG("⚠️ No tracks available in the playlist.");
        return;
    }

    // Retrieve the next track for the transition.
    juce::File nextTrack = getNextTrack();
    if (!nextTrack.existsAsFile())
    {
        DBG("❌ ERROR: No valid next track found!");
        return;
    }

    // Get the BPM of the current playing deck.
    double targetBPM = (currentDeck == 1) ? player1->getBPM() : player2->getBPM();

    if (currentDeck == 1)
    {
        DBG("🔀 Loading next track into Deck 2...");

        // Load next track into Deck 2 and update waveform display.
        player2->loadURL(juce::URL(nextTrack));
        waveform2->loadURL(juce::URL(nextTrack));

        // Detect BPM of the next track and adjust speed if needed.
        double nextTrackBPM = player2->detectBPM();
        if (nextTrackBPM > 0)
        {
            player2->setBPM(targetBPM);
        }

        // Start playback of Deck 2 and perform the gradual crossfade transition.
        player2->start();
        gradualCrossfade(3.0f);
        currentDeck = 2;
    }
    else
    {
        DBG("🔀 Loading next track into Deck 1...");

        // Load next track into Deck 1 and update waveform display.
        player1->loadURL(juce::URL(nextTrack));
        waveform1->loadURL(juce::URL(nextTrack));

        // Detect BPM of the next track and adjust speed if needed.
        double nextTrackBPM = player1->detectBPM();
        if (nextTrackBPM > 0)
        {
            player1->setBPM(targetBPM);
        }

        // Start playback of Deck 1 and perform the gradual crossfade transition.
        player1->start();
        gradualCrossfade(3.0f);
        currentDeck = 1;
    }

    DBG("🎶 AutoMix Transition Complete!");
}

void AutoMixComponent::gradualCrossfade(float durationInSeconds)
{
    // Define the number of incremental steps for smooth volume transition.
    int fadeSteps = 30;

    // Calculate the delay between each step to match the duration.
    float stepDelay = (durationInSeconds * 1000) / fadeSteps;

    // Perform a gradual fade between decks.
    for (int i = 0; i <= fadeSteps; ++i)
    {
        float fadeLevel = static_cast<float>(i) / static_cast<float>(fadeSteps);

        // Adjust gains asynchronously to prevent blocking the main thread.
        juce::MessageManager::callAsync([this, fadeLevel]()
            {
                if (currentDeck == 1)
                {
                    player1->setGain(1.0f - fadeLevel);
                    player2->setGain(fadeLevel);
                }
                else
                {
                    player2->setGain(1.0f - fadeLevel);
                    player1->setGain(fadeLevel);
                }
            });

        // Pause briefly between fade steps for a smooth transition.
        juce::Thread::sleep(static_cast<int>(stepDelay));
    }

    // Ensure one deck is fully faded out and the other is fully active at the end.
    if (currentDeck == 1)
    {
        player1->setGain(0.0f);
        player2->setGain(1.0f);
    }
    else
    {
        player2->setGain(0.0f);
        player1->setGain(1.0f);
    }

    // Update the active deck after crossfade completes.
    currentDeck = (currentDeck == 1) ? 2 : 1;
}

