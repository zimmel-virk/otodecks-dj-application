#include "ThemeComponent.h"

// 🎨 **Static Theme Colors (Shared Across Components)**
juce::Colour ThemeComponent::backgroundColour = juce::Colours::black;  // ✅ Default: Dark Mode
juce::Colour ThemeComponent::buttonColour = juce::Colours::grey;
juce::Colour ThemeComponent::textColour = juce::Colours::white;
juce::Colour ThemeComponent::waveformColour = juce::Colours::blue;

/**
 * 🎛 **ThemeComponent Constructor**
 * - Initializes the theme selector and sets default theme to **Dark Mode**.
 */
ThemeComponent::ThemeComponent()
{
    // 🏷 **Create "Theme" Label**
    themeLabel.setText("Theme", juce::dontSendNotification);
    themeLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    themeLabel.setJustificationType(juce::Justification::centred);
    themeLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible(themeLabel);  // ✅ Make label visible

    // 🎨 **Theme Selector Dropdown**
    themeSelector.addItem("Dark Mode", 1);
    themeSelector.addItem("Light Mode", 2);
    themeSelector.addItem("Neon Mode", 3);
    themeSelector.addItem("Custom Mode", 4);
    themeSelector.addListener(this); // ✅ Detect theme changes
    addAndMakeVisible(themeSelector);

    applyTheme(1); // ✅ Default to Dark Mode
}

ThemeComponent::~ThemeComponent() {}

/**
 * 🎨 **Paint Function**
 * - Fills the background with the selected theme color.
 * - Displays the **Theme** label at the top.
 */
void ThemeComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    g.setColour(textColour);
    g.setFont(16.0f);
    g.drawText("Theme", getLocalBounds(), juce::Justification::centredTop);
}

/**
 * 🎛 **Resized Function**
 * - Positions the label and theme selector dropdown.
 */
void ThemeComponent::resized()
{
    themeLabel.setBounds(10, 5, getWidth() - 20, 20);  // ✅ Position label above theme selector
    themeSelector.setBounds(10, 30, getWidth() - 20, 30);  // ✅ Keep selector below label
}

/**
 * 🎚 **comboBoxChanged Function**
 * - Called when a new theme is selected.
 * - Applies the selected theme and repaints the UI.
 */
void ThemeComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &themeSelector)
    {
        applyTheme(comboBox->getSelectedId());
        repaint();
    }
}

/**
 * 🎨 **applyTheme Function (Main)**
 * - Updates the **background, button, text, and waveform colors** based on selected theme.
 * @param themeIndex The selected theme ID (1-4).
 * @param component The component to repaint after applying the theme.
 */
void ThemeComponent::applyTheme(int themeIndex, juce::Component* component)
{
    switch (themeIndex)
    {
    case 1: // 🌙 **Dark Mode**
        backgroundColour = juce::Colours::black;
        buttonColour = juce::Colours::grey;
        textColour = juce::Colours::white;
        waveformColour = juce::Colours::blue;
        break;

    case 2: // ☀️ **Light Mode**
        backgroundColour = juce::Colours::white;
        buttonColour = juce::Colours::lightgrey;
        textColour = juce::Colours::black;
        waveformColour = juce::Colours::darkgrey;
        break;

    case 3: // 🎆 **Neon Mode**
        backgroundColour = juce::Colours::darkmagenta;
        buttonColour = juce::Colours::cyan;
        textColour = juce::Colours::yellow;
        waveformColour = juce::Colours::lime;
        break;

    case 4: // 🎨 **Custom Mode**
        backgroundColour = juce::Colours::darkblue;
        buttonColour = juce::Colours::orange;
        textColour = juce::Colours::green;
        waveformColour = juce::Colours::red;
        break;
    }

    component->repaint(); // ✅ Refresh the UI
}

// 🎨 **Getter Functions for Theme Colors**
juce::Colour ThemeComponent::getBackgroundColor() { return backgroundColour; }
juce::Colour ThemeComponent::getTextColor() { return textColour; }
juce::Colour ThemeComponent::getButtonColor() { return buttonColour; }
juce::Colour ThemeComponent::getWaveformColor() { return waveformColour; }

/**
 * ⚠️ **applyTheme (Overloaded)**
 * - This function is **empty** and unused.
 */
void ThemeComponent::applyTheme(int themeIndex)
{
}
