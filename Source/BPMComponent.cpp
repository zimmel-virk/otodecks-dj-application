#include "BPMComponent.h"
#include "ThemeComponent.h"

// Constructor: Initializes the BPMComponent and sets up the Sync BPM button.
// - player1: Pointer to the first DJAudioPlayer instance (Deck 1).
// - player2: Pointer to the second DJAudioPlayer instance (Deck 2).
BPMComponent::BPMComponent(DJAudioPlayer* player1, DJAudioPlayer* player2)
    : player1(player1), player2(player2)
{
    // Set the button color to green for better visibility.
    syncButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

    // Add an event listener to handle button clicks.
    syncButton.addListener(this);

    // Make the Sync BPM button visible in the UI.
    addAndMakeVisible(syncButton);
}

// Destructor: Cleans up the BPMComponent when it is no longer needed.
BPMComponent::~BPMComponent() {}

// Paint function: Sets the background color of the BPM sync component.
// - g: Reference to the JUCE graphics context used for rendering.
void BPMComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(20, 20, 20)); // Dark gray background for contrast.
}

// Resized function: Adjusts the Sync BPM button’s size and position when the window is resized.
void BPMComponent::resized()
{
    syncButton.setBounds(10, 10, getWidth() - 20, getHeight() - 20);
}

// Handles button click events for the Sync BPM button.
// - button: Pointer to the button that was clicked.
void BPMComponent::buttonClicked(juce::Button* button)
{
    if (button == &syncButton) // Ensure that the Sync BPM button was clicked.
    {
        if (syncActive) // ✅ Check if already synced
        {
            // Reset both decks to their normal playback speed.
            player1->setSpeed(1.0);
            player2->setSpeed(1.0);

            // Update button text to indicate syncing is off.
            syncButton.setButtonText("Sync BPM");
            syncActive = false;
        }
        else
        {
            // Sync BPM of both players and update UI.
            syncBPM();
            syncButton.setButtonText("Synced!");
            syncActive = true;
        }
    }
}

// Synchronizes the BPM of both decks by adjusting playback speeds.
void BPMComponent::syncBPM()
{
    // Retrieve the BPM of both tracks.
    double bpm1 = player1->getBPM();
    double bpm2 = player2->getBPM();

    // Adjust the speed of the slower deck to match the BPM of the faster deck.
    if (bpm1 > bpm2)
    {
        player2->setSpeed(bpm1 / bpm2); // Speed up Deck 2 to match Deck 1's BPM.
    }
    else
    {
        player1->setSpeed(bpm2 / bpm1); // Speed up Deck 1 to match Deck 2's BPM.
    }

    // Debug message for tracking BPM synchronization.
    DBG("BPM Synced: Player1=" + juce::String(player1->getBPM()) +
        ", Player2=" + juce::String(player2->getBPM()));
}

// Applies the selected theme to the Sync BPM button.
void BPMComponent::applyTheme()
{
    syncButton.setColour(juce::TextButton::buttonColourId, ThemeComponent::getButtonColor());
    repaint(); // Refresh the UI after theme update.
}
