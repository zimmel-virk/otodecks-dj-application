#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"

// JogWheelComponent simulates a DJ jog wheel, allowing users to scratch and manipulate playback speed
// using mouse gestures. It visually rotates and applies speed adjustments to a linked DJAudioPlayer.
class JogWheelComponent : public juce::Component,
    public juce::MouseListener // ✅ Allows the component to handle mouse events.
{
public:
    // ✅ Constructor: Initializes the jog wheel and links it to a DJAudioPlayer.
    // - player: Pointer to the DJAudioPlayer instance that will be controlled.
    JogWheelComponent(DJAudioPlayer* player);

    // ✅ Destructor: Cleans up any allocated resources.
    ~JogWheelComponent() override;

    // ✅ Paints the jog wheel UI, including its rotating disk and speed indicators.
    void paint(juce::Graphics&) override;

    // ✅ Handles component resizing (not needed for fixed-size components but required in JUCE).
    void resized() override;

    // ✅ Handles mouse drag events to adjust playback speed and rotate the jog wheel.
    void mouseDrag(const juce::MouseEvent& event) override;

    // ✅ Handles mouse click events to start scratching.
    void mouseDown(const juce::MouseEvent& event) override;

    // ✅ Handles mouse release events to stop scratching and reset speed.
    void mouseUp(const juce::MouseEvent& event) override;

    // ✅ Resets the jog wheel's rotation and speed to normal.
    void resetJogWheel();

private:
    DJAudioPlayer* player; // ✅ Pointer to the audio player being controlled.

    float lastMouseX = 0.0f; // ✅ Stores the last X position of the mouse.
    float lastMouseY = 0.0f; // ✅ Stores the last Y position of the mouse (for vertical gestures).
    bool isScratching = false; // ✅ Flag to indicate if scratching is active.
    float rotationAngle = 0.0f; // ✅ Stores the visual rotation of the jog wheel.

    // ✅ Colors for the jog wheel.
    juce::Colour wheelColour = juce::Colour(50, 50, 50);  // Default jog wheel color.
    juce::Colour activeColour = juce::Colour(0, 255, 0);  // Color when actively scratching.
    juce::Colour normalColour = juce::Colour(0, 180, 255); // Default color when idle.

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JogWheelComponent) // ✅ Prevents copying.
};
