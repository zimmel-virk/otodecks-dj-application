#pragma once
#include <JuceHeader.h>

/**
 * ThemeComponent class allows users to change the application's color theme.
 * It provides a dropdown menu for selecting themes and updates UI elements accordingly.
 */
class ThemeComponent : public juce::Component, public juce::ComboBox::Listener
{
public:
    /**
     * Constructor: Initializes the theme selector and applies the default theme.
     */
    ThemeComponent();

    /**
     * Destructor: Cleans up any resources used by the component.
     */
    ~ThemeComponent() override;

    /**
     * Paints the component's background and label.
     * @param g Graphics context for drawing the component.
     */
    void paint(juce::Graphics& g) override;

    /**
     * Resizes the component and positions the theme selector and label.
     */
    void resized() override;

    /**
     * Handles changes to the theme dropdown menu.
     * @param comboBox Pointer to the ComboBox that triggered the event.
     */
    void comboBoxChanged(juce::ComboBox* comboBox) override;

    /**
     * Applies the selected theme to the specified component.
     * @param themeIndex The ID of the selected theme.
     * @param component The component to which the theme should be applied.
     */
    static void applyTheme(int themeIndex, juce::Component* component);

    /**
     * Returns the background color of the current theme.
     */
    static juce::Colour getBackgroundColor();

    /**
     * Returns the text color of the current theme.
     */
    static juce::Colour getTextColor();

    /**
     * Returns the button color of the current theme.
     */
    static juce::Colour getButtonColor();

    /**
     * Returns the waveform color of the current theme.
     */
    static juce::Colour getWaveformColor();

private:
    /**
     * Dropdown menu for selecting themes.
     */
    juce::ComboBox themeSelector;

    /**
     * Applies the selected theme without affecting other components.
     * @param themeIndex The ID of the selected theme.
     */
    void applyTheme(int themeIndex);

    /**
     * Label displaying the word "Theme" above the selector.
     */
    juce::Label themeLabel;

    /**
     * Static variables for storing theme colors.
     */
    static juce::Colour backgroundColour;
    static juce::Colour buttonColour;
    static juce::Colour textColour;
    static juce::Colour waveformColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThemeComponent)
};
