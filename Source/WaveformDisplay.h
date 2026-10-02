#pragma once
#include <JuceHeader.h>

/**
 * WaveformDisplay Component
 *
 * This class provides a visual representation of an audio waveform.
 * It supports zooming, scrolling, and tracking the current playhead position.
 */
class WaveformDisplay : public juce::Component, public juce::ChangeListener
{
public:
    /**
     * Constructor: Initializes the waveform display with the given format manager and cache.
     * @param formatManager Manages different audio file formats.
     * @param cache A cache for quickly loading audio thumbnails.
     */
    WaveformDisplay(juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache);

    /**
     * Destructor: Cleans up any allocated resources.
     */
    ~WaveformDisplay();

    /**
     * Paints the waveform display.
     * @param g Graphics context used for drawing.
     */
    void paint(juce::Graphics&) override;

    /**
     * Positions and resizes UI elements like zoom and scroll sliders.
     */
    void resized() override;

    /**
     * Listens for waveform changes and triggers a repaint when the waveform updates.
     * @param source The ChangeBroadcaster that triggered the update.
     */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    /**
     * Loads an audio file into the waveform display.
     * @param audioURL The URL of the audio file.
     */
    void loadURL(juce::URL audioURL);

    /**
     * Updates the playhead position relative to the track length.
     * @param pos The new position relative to the track (0.0 to 1.0).
     */
    void setPositionRelative(double pos);

    /**
     * Applies the selected theme to the waveform display.
     */
    void applyTheme();

private:
    juce::AudioThumbnail thumbnail; // Stores and manages the waveform visualization
    bool fileLoaded = false;        // Indicates whether an audio file has been loaded
    double position = 0.0;          // Current playback position relative to the track

    // Styling elements for waveform display
    juce::Colour waveformColour = juce::Colour(0, 200, 255); // Neon Blue
    juce::Colour playheadColour = juce::Colours::red.withAlpha(0.8f); // Slightly transparent red
    juce::Colour backgroundColour = juce::Colour(20, 20, 20); // Dark theme

    // Zoom and Scroll functionality
    double zoomLevel = 1.0;  // Default zoom level
    double scrollPosition = 0.0;  // Default scroll position
    juce::Slider zoomSlider, scrollSlider; // UI sliders for zoom and scroll

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
