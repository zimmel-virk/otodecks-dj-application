#include "WaveformDisplay.h"
#include "ThemeComponent.h"

using namespace juce;

/**
 * Constructor: Initializes the waveform display with a given format manager and cache.
 * Sets up the zoom and scroll sliders.
 */
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse)
    : thumbnail(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0) // Corrected initialization order
{
    thumbnail.addChangeListener(this);

    // Zoom Slider: Allows users to zoom in/out of the waveform
    addAndMakeVisible(zoomSlider);
    zoomSlider.setRange(1.0, 5.0, 0.1); // Zoom levels from 1x to 5x
    zoomSlider.setValue(1.0); // Default zoom level
    zoomSlider.onValueChange = [this] { zoomLevel = zoomSlider.getValue(); repaint(); };

    // Scroll Slider: Allows users to scroll through the waveform
    addAndMakeVisible(scrollSlider);
    scrollSlider.setRange(0.0, 1.0, 0.01); // Scroll smoothly between 0% and 100%
    scrollSlider.setValue(0.0);
    scrollSlider.onValueChange = [this] { scrollPosition = scrollSlider.getValue(); repaint(); };
}

/**
 * Destructor: Cleans up any allocated resources.
 */
WaveformDisplay::~WaveformDisplay() {}

/**
 * Updates the playhead position relative to the track length.
 * @param pos The new position relative to the track (0.0 to 1.0).
 */
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
    if (!fileLoaded)
    {
        repaint();
    }
}

/**
 * Paints the waveform display.
 * @param g Graphics context used for drawing.
 */
void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(ThemeComponent::getBackgroundColor()); // Apply background theme
    g.setColour(ThemeComponent::getWaveformColor());

    // Apply a gradient effect to the waveform
    g.setGradientFill(juce::ColourGradient(
        ThemeComponent::getWaveformColor(),
        0, 0,
        juce::Colours::black,
        getWidth(), getHeight(),
        false));

    if (fileLoaded)
    {
        // Define the rectangle where the waveform will be drawn
        juce::Rectangle<int> waveformBounds(0, 0, getWidth(), getHeight());

        // Calculate visible waveform section based on zoom & scroll
        float start = scrollPosition * thumbnail.getTotalLength();
        float end = start + (thumbnail.getTotalLength() / zoomLevel);

        // Draw the waveform for the selected section
        thumbnail.drawChannel(g, waveformBounds, start, end, 0, 1.0f);

        // Draw the playhead as a thicker glowing line
        g.setColour(playheadColour);
        int playheadX = static_cast<int>(position * getWidth());
        g.fillRoundedRectangle(playheadX - 2, 0, 4, getHeight(), 2.0f);
    }
    else
    {
        // Display a message when no track is loaded
        g.setFont(20.0f);
        g.setColour(juce::Colours::grey);
        g.drawText("No track loaded", getLocalBounds(), juce::Justification::centred, true);
    }
}

/**
 * Positions the zoom and scroll sliders below the waveform.
 */
void WaveformDisplay::resized()
{
    zoomSlider.setBounds(10, getHeight() - 20, getWidth() / 2 - 15, 30);
    scrollSlider.setBounds(getWidth() / 2 + 5, getHeight() - 20, getWidth() / 2 - 15, 30);

    repaint();
}

/**
 * Loads an audio file into the waveform display.
 * @param audioURL The URL of the audio file.
 */
void WaveformDisplay::loadURL(juce::URL audioURL)
{
    juce::File audioFile = audioURL.getLocalFile();
    if (!audioFile.existsAsFile())
    {
        DBG("WaveformDisplay::loadURL - ERROR: File does not exist.");
        return;
    }

    if (thumbnail.setSource(new juce::FileInputSource(audioFile)))
    {
        fileLoaded = true;
        repaint(); // Update the display with the new waveform
    }
    else
    {
        DBG("WaveformDisplay::loadURL - ERROR: Failed to set source.");
    }
}

/**
 * Listens for changes in the waveform and triggers a repaint when updated.
 */
void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail) repaint();
}

/**
 * Applies the selected theme to the waveform display.
 */
void WaveformDisplay::applyTheme()
{
    repaint();
}
