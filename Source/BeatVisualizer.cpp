#include "BeatVisualizer.h"

// Constructor: Initializes the BeatVisualizer and starts the animation timer.
BeatVisualizer::BeatVisualizer()
{
    startTimer(30); // Refresh every 30ms for smooth animation
}

// Destructor: Stops the timer when the visualizer is destroyed to free resources.
BeatVisualizer::~BeatVisualizer()
{
    stopTimer();
}

// Paint function: Renders the animated beat visualization on the screen.
// - g: Reference to the JUCE graphics context used for drawing.
void BeatVisualizer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(10, 10, 10)); // Fill background with a dark color.

    if (level == 0.0f) return; // Do nothing if no audio is playing (silence).

    int numBars = 12; // Number of bars in the visualizer.
    float barWidth = getWidth() / numBars; // Calculate the width of each bar.
    float maxHeight = getHeight(); // Maximum possible height of a bar.

    // Map the audio level (0 to 1) to a scaled height range (5px to full height).
    float animatedLevel = juce::jmap(level, 0.0f, 1.0f, 5.0f, maxHeight);

    for (int i = 0; i < numBars; ++i)
    {
        // Generate a slightly randomized height for each bar to create a wave effect.
        float barHeight = animatedLevel * ((float)(rand() % 10) / 10.0f + 0.5f);

        // Create a color gradient that changes hue for each bar.
        juce::ColourGradient gradient(
            juce::Colour::fromHSV((float)i / numBars, 1.0f, 1.0f, 1.0f), // Varying color
            0, maxHeight - barHeight, // Gradient start position
            juce::Colours::black, // Gradient end color (black)
            0, maxHeight,
            false);
        g.setGradientFill(gradient);

        // Draw the rectangle for the current bar.
        g.fillRect(i * barWidth, maxHeight - barHeight, barWidth - 2, barHeight);
    }
}

// Resized function: Called when the component size changes.
// Currently, no special resizing behavior is needed.
void BeatVisualizer::resized()
{
    // No special resizing needed
}

// Timer callback: Periodically refreshes the visualizer by repainting it.
void BeatVisualizer::timerCallback()
{
    repaint(); // Request a redraw of the visualizer to update animation.
}

// Updates the level of the visualizer based on audio intensity.
// - newLevel: The latest amplitude level detected from the audio.
void BeatVisualizer::updateLevel(float newLevel)
{
    if (newLevel < 0.05f) newLevel = 0.0f; // Ignore very small fluctuations (noise reduction).

    // Apply a smoothing algorithm to make the transition between frames less abrupt.
    level = level * 0.7f + newLevel * 0.3f;

    repaint(); // Request a redraw with the updated level.
}
