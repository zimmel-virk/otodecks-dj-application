
#pragma once

#include <JuceHeader.h>

// BeatVisualizer is responsible for rendering an animated visualization
// of the audio levels, creating a dynamic bar animation effect.
class BeatVisualizer : public juce::Component, public juce::Timer
{
public:
    // Constructor: Initializes the BeatVisualizer component.
    // Starts a timer to refresh the animation at regular intervals.
    BeatVisualizer();

    // Destructor: Cleans up resources when the visualizer is destroyed.
    ~BeatVisualizer() override;

    // Paint function: Draws the visual representation of the beat animation.
    // - g: Reference to the JUCE graphics context used for rendering.
    void paint(juce::Graphics&) override;

    // Resized function: Called when the component size changes.
    // This function is currently unused but can be extended if needed.
    void resized() override;

    // Timer callback: Refreshes the visualization by triggering repaints.
    void timerCallback() override;

    // Updates the audio level to modify the animation.
    // - newLevel: The latest amplitude value detected from the audio signal.
    void updateLevel(float newLevel);

private:
    float level = 0.0f; // Stores the current amplitude level of the audio.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeatVisualizer)
};
