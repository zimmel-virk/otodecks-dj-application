#include "JogWheelComponent.h"

// ✅ Constructor: Initializes the Jog Wheel component.
// - _player: Pointer to the DJAudioPlayer that will be controlled.
JogWheelComponent::JogWheelComponent(DJAudioPlayer* _player)
    : player(_player)
{
    addMouseListener(this, true); // ✅ Enable mouse interaction.
}

// ✅ Destructor: Cleans up any allocated resources.
JogWheelComponent::~JogWheelComponent() {}

// ✅ Draws the jog wheel UI, including the rotating disk, radial notches, and speed display.
void JogWheelComponent::paint(juce::Graphics& g)
{
    // 🟢 **Transparent Background (No Square)**
    g.fillAll(juce::Colours::transparentBlack);

    // 🎛 **Define Smaller Circular Jog Wheel Area**
    int wheelSize = juce::jmin(getWidth(), getHeight()) * 0.7f;  // ✅ Make it 70% of original size.
    int centerX = getWidth() / 2;
    int centerY = getHeight() / 2 - 10;  // ✅ Move it UP slightly.

    // 🎨 **Glowing Outer Ring**
    g.setColour(isScratching ? activeColour.brighter() : normalColour.darker());
    g.drawEllipse(centerX - wheelSize / 2, centerY - wheelSize / 2, wheelSize, wheelSize, 4.0f);

    // 🎛 **Jog Wheel Disk with Rotation**
    g.setColour(isScratching ? activeColour : normalColour);
    juce::Path wheelPath;
    wheelPath.addEllipse(centerX - wheelSize / 2 + 5, centerY - wheelSize / 2 + 5, wheelSize - 10, wheelSize - 10);
    g.fillPath(wheelPath, juce::AffineTransform::rotation(rotationAngle, centerX, centerY));

    // ⚙️ **Radial Notches for Realism**
    g.setColour(juce::Colours::black);
    for (int i = 0; i < 12; i++)
    {
        float angle = juce::MathConstants<float>::twoPi * (i / 12.0f);
        float x1 = centerX + (wheelSize / 2 - 12) * std::cos(angle);
        float y1 = centerY + (wheelSize / 2 - 12) * std::sin(angle);
        float x2 = centerX + (wheelSize / 2 - 5) * std::cos(angle);
        float y2 = centerY + (wheelSize / 2 - 5) * std::sin(angle);
        g.drawLine(x1, y1, x2, y2, 1.5f);
    }

    // 🔵 **Smaller Center Grip**
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.fillEllipse(centerX - 8, centerY - 8, 16, 16);

    // 📌 **Speed Change Display (Below the Jog Wheel)**
    g.setFont(12.0f);
    g.setColour(juce::Colours::white);
    g.drawText("Speed: " + juce::String(player->getSpeed()), 0, centerY + wheelSize / 2 + 5, getWidth(), 20, juce::Justification::centred);
}

// ✅ Handles component resizing (currently not needed but required for JUCE components).
void JogWheelComponent::resized() {}

// ✅ Called when the user clicks on the jog wheel.
// - Stores the initial mouse position to calculate movement.
void JogWheelComponent::mouseDown(const juce::MouseEvent& event)
{
    isScratching = true;
    lastMouseX = event.position.x;
    lastMouseY = event.position.y; // ✅ Track initial Y position.
    repaint();
}

// ✅ Called when the user drags the mouse while holding the jog wheel.
// - Adjusts playback speed based on horizontal movement.
// - Adjusts the visual rotation of the jog wheel.
void JogWheelComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isScratching)
    {
        float deltaX = event.position.x - lastMouseX;
        float deltaY = event.position.y - lastMouseY; // ✅ Detect vertical movement.
        float speedAdjustment = deltaX * 0.01f;

        float newSpeed = player->getSpeed() + speedAdjustment;

        // ✅ If dragging UP, gradually return to normal speed (1.0x).
        if (deltaY < -5) { // Moving up on screen (negative Y).
            newSpeed = newSpeed * 0.8f + 1.0f * 0.2f; // Smooth transition back to normal.
        }

        // ✅ Allow Reverse (-2x) to Fast Forward (5x).
        if (newSpeed < -2.0f) newSpeed = -2.0f;
        if (newSpeed > 5.0f) newSpeed = 5.0f;

        player->setSpeed(newSpeed);

        // ✅ Rotate jog wheel visually.
        rotationAngle += deltaX * 0.01f;

        lastMouseX = event.position.x;
        lastMouseY = event.position.y; // ✅ Update last Y position.
        repaint();
    }
}

// ✅ Called when the user releases the mouse button after scratching.
// - Resets speed to normal (1.0x).
void JogWheelComponent::mouseUp(const juce::MouseEvent& event)
{
    isScratching = false;
    player->setSpeed(1.0f); // ✅ Force full reset to normal speed.
    repaint();
}

// ✅ Resets the jog wheel's rotation and speed to normal.
void JogWheelComponent::resetJogWheel()
{
    rotationAngle = 0.0f; // ✅ Reset visual rotation.
    player->setSpeed(1.0); // ✅ Reset speed to normal.
    repaint();
}
