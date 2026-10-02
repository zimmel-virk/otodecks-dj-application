#include "DeckGUI.h"
#include "ThemeComponent.h"

// Constructor: Initializes the DeckGUI component and configures UI elements.
// - _player: Pointer to the DJAudioPlayer associated with this deck.
// - bpm: Pointer to the BPMComponent for BPM synchronization.
// - formatManager: Reference to the audio format manager for waveform processing.
// - cache: Reference to the thumbnail cache to optimize waveform rendering.
DeckGUI::DeckGUI(DJAudioPlayer* _player, BPMComponent* bpm, juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache)
    : player(_player), bpmComponent(bpm), waveformDisplay(formatManager, cache), jogWheel(_player)
{
    configureButtons();  // ✅ Set up Play, Pause, Stop, and Loop buttons
    configureSliders();  // ✅ Set up volume, speed, and position sliders

    // ✅ Add visual components to the interface
    addAndMakeVisible(waveformDisplay); // Displays waveform visualization of the track
    addAndMakeVisible(beatVisualizer);  // Visualizes beats dynamically
    addAndMakeVisible(jogWheel);        // Enables manual scrubbing and scratching

    startTimer(200); // ✅ UI updates every 200ms

    // ✅ Add control buttons to the UI
    addAndMakeVisible(loadButton);       // Load track button
    addAndMakeVisible(loopButton);       // Loop toggle button
    addAndMakeVisible(bpmSyncButton);    // BPM Sync button
    addAndMakeVisible(playButton);       // Play button
    addAndMakeVisible(pauseButton);      // Pause button
    addAndMakeVisible(stopButton);       // Stop button

    // ✅ Add sliders for volume, speed, and track position control
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);

    // ✅ Customize button appearance and add event listeners
    loadButton.setColour(juce::TextButton::buttonColourId, buttonColour);
    loadButton.setColour(juce::TextButton::textColourOffId, textColour);
    loadButton.addListener(this);

    loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
    loopButton.addListener(this);

    bpmSyncButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    bpmSyncButton.addListener(this);

    // ✅ Set up labels for sliders
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, true);
    addAndMakeVisible(volLabel);

    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, true);
    addAndMakeVisible(speedLabel);

    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.attachToComponent(&posSlider, true);
    addAndMakeVisible(posLabel);

    startTimer(200); // ✅ Ensures smooth UI updates

    // ✅ Enable keyboard input handling
    addKeyListener(this);
    setWantsKeyboardFocus(true);

    // ✅ Add beat jump buttons (for quick navigation)
    addAndMakeVisible(jumpForwardButton);
    addAndMakeVisible(jumpBackwardButton);

    // Configure beat jump buttons to move by 1 beat
    jumpForwardButton.onClick = [this] { player->jumpByBeats(1); };
    jumpBackwardButton.onClick = [this] { player->jumpByBeats(-1); };
}

// Destructor: Cleans up DeckGUI and stops the timer to prevent unnecessary updates.
DeckGUI::~DeckGUI()
{
    stopTimer();
}

// Paint function: Fills the component's background with the selected theme color.
// - g: Reference to the JUCE graphics context used for rendering.
void DeckGUI::paint(juce::Graphics& g)
{
    g.fillAll(ThemeComponent::getBackgroundColor()); // ✅ Apply the current theme's background color
}

// Applies the selected theme to UI components like buttons and text colors.
void DeckGUI::applyTheme()
{
    loadButton.setColour(juce::TextButton::buttonColourId, ThemeComponent::getButtonColor());
    loadButton.setColour(juce::TextButton::textColourOffId, ThemeComponent::getTextColor());
    repaint(); // ✅ Ensure UI updates immediately with the new theme
}

// Resized function: Adjusts the layout of all buttons, sliders, and visual components
// when the window size changes dynamically.
void DeckGUI::resized()
{
    int rowH = getHeight() / 8; // ✅ Calculate row height based on total height
    int colW = getWidth() / 4;  // ✅ Divide width into 4 equal columns for layout

    // 🎛 **Playback Controls (Top Row)**
    playButton.setBounds(0, 0, colW, rowH);
    pauseButton.setBounds(colW, 0, colW, rowH);
    stopButton.setBounds(2 * colW, 0, colW, rowH);
    loadButton.setBounds(0, rowH, getWidth(), rowH); // ✅ Load button spans full width

    // 🔄 **Loop, BPM Sync, & Beat Jump Controls (Second Row)**
    loopButton.setBounds(0, rowH * 2, colW, rowH);
    bpmSyncButton.setBounds(colW, rowH * 2, colW, rowH);

    // ✅ Position beat jump buttons next to BPM Sync
    jumpBackwardButton.setBounds(2 * colW, rowH * 2, colW / 2, rowH);
    jumpForwardButton.setBounds(2 * colW + colW / 2, rowH * 2, colW / 2, rowH);

    // 🎵 **Waveform & Beat Visualizer**
    waveformDisplay.setBounds(0, rowH * 3, getWidth(), rowH * 2); // ✅ Full width for better visibility
    beatVisualizer.setBounds(0, rowH * 5, getWidth(), rowH); // ✅ Positioned below waveform display

    // 🎚 **Sliders Section**
    int leftPadding = 40;    // ✅ Leave space on the extreme left
    int rightPadding = 40;   // ✅ Leave space on the extreme right
    int sliderSpacing = 20;  // ✅ Reduce space between sliders for compact design

    // ✅ Calculate available width for sliders
    int sliderWidth = (getWidth() - leftPadding - rightPadding - 2 * sliderSpacing) / 3;
    int sliderHeight = rowH * 2;
    int textBoxWidth = 70;  // ✅ Increase text box size for better readability
    int sliderY = rowH * 6; // ✅ Align sliders in the lower section

    // Position sliders for volume, speed, and track position
    volSlider.setBounds(leftPadding, sliderY, sliderWidth, sliderHeight);
    speedSlider.setBounds(volSlider.getRight() + sliderSpacing, sliderY, sliderWidth, sliderHeight);
    posSlider.setBounds(speedSlider.getRight() + sliderSpacing, sliderY, sliderWidth, sliderHeight);

    // 🎚 **Move Labels to the Right of Each Slider for better readability**
    int labelYOffset = 5; // ✅ Offset to align labels properly
    volLabel.setBounds(volSlider.getRight() - textBoxWidth / 2 - 100, sliderY - labelYOffset, textBoxWidth, 30);
    speedLabel.setBounds(speedSlider.getRight() - textBoxWidth / 2 - 100, sliderY - labelYOffset, textBoxWidth, 30);
    posLabel.setBounds(posSlider.getRight() - textBoxWidth / 2 - 100, sliderY - labelYOffset, textBoxWidth, 30);

    // 🎛 **Jog Wheel & Beat Jump Controls**
    jogWheel.setBounds(leftPadding - 30, getHeight() - 80, 80, 80); // ✅ Positioned at the bottom-left

    int buttonWidth = 80;

}



// Handles button click events for playback, looping, BPM sync, and track loading.
// - button: Pointer to the button that was clicked.
void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        player->start(); // ▶ Start playback
    }
    else if (button == &pauseButton)
    {
        player->pause(); // ⏸ Pause playback
    }
    else if (button == &stopButton)
    {
        player->stop(); // ⏹ Stop playback
    }
    else if (button == &loopButton) // 🔄 Toggle Loop Mode
    {
        player->toggleLoop(); // Enable or disable looping
        loopButton.setButtonText(player->isLooping() ? "LOOP ON" : "LOOP OFF"); // Update button text
    }
    else if (button == &bpmSyncButton) // 🕺 BPM Sync Feature
    {
        double bpm = player->getBPM();
        player->setSpeed(120.0 / bpm); // Adjust playback speed to sync BPM to 120
        bpmSyncButton.setButtonText("SYNCED"); // ✅ Update button text
    }
    else if (button == &loadButton) // 📂 Load a new track
    {
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles; // Allow file selection
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                auto chosenFile = chooser.getResult();
                if (chosenFile.existsAsFile()) // Ensure the selected file exists
                {
                    player->loadURL(juce::URL{ chosenFile }); // Load track into the player
                    waveformDisplay.loadURL(juce::URL{ chosenFile }); // Update waveform display

                    // ✅ Reset all controls when loading a new track
                    volSlider.setValue(1.0); // Reset volume to default (100%)
                    speedSlider.setValue(1.0); // Reset speed to normal
                    posSlider.setValue(0.0); // Reset position slider to track start
                    jogWheel.resetJogWheel(); // ✅ Reset jog wheel rotation
                    bpmSyncButton.setButtonText("Sync BPM"); // ✅ Reset BPM Sync button text
                    bpmSyncButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green); // ✅ Reset color

                    repaint(); // ✅ Ensure UI updates immediately after loading a new track
                }
            });
    }
}

// Handles slider value changes to update volume, speed, and track position.
// - slider: Pointer to the slider that was adjusted.
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue()); // 🔊 Adjust volume level
        volSlider.repaint(); // ✅ Force UI repaint for smooth slider updates
    }
    else if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue()); // 🎵 Adjust playback speed
        speedSlider.repaint(); // ✅ Force UI repaint for smooth updates
    }
    else if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue()); // ⏩ Seek to the selected track position
    }
}


// Determines if the dragged file is an audio file that can be loaded into the deck.
// - files: A list of files being dragged into the application.
// Returns:
// - true if the file is an MP3, WAV, or OGG file, false otherwise.
bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files)
{
    return files[0].endsWith(".mp3") || files[0].endsWith(".wav") || files[0].endsWith(".ogg");
}

// Handles files dropped onto the deck, loading the selected track into the player and updating UI components.
// - files: List of files dropped onto the deck.
// - x, y: Coordinates where the files were dropped.
void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y)
{
    if (files.size() > 0)
    {
        juce::File file{ files[0] };
        if (file.existsAsFile()) // Ensure the dropped file exists before loading.
        {
            player->loadURL(juce::URL{ file }); // Load track into the player.
            waveformDisplay.loadURL(juce::URL{ file }); // Update waveform display.
            beatVisualizer.updateLevel(1.0f);  // Ensure the visualizer reacts to the new track.
        }
    }
}

// Periodically updates the waveform display and beat visualizer based on the track’s playback position.
void DeckGUI::timerCallback()
{
    if (player)
    {
        waveformDisplay.setPositionRelative(player->getPositionRelative()); // Sync waveform position with track playback.

        // ✅ Update BeatVisualizer with current audio level.
        float realTimeAudioLevel = player->getCurrentAudioLevel(); // ✅ Get audio level from DJAudioPlayer.
        beatVisualizer.updateLevel(realTimeAudioLevel);
    }
}

// Configures playback control buttons (Play, Pause, Stop) and adds them to the UI.
void DeckGUI::configureButtons()
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);

    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
}

// Configures sliders (Volume, Speed, and Position) and applies custom styles.
void DeckGUI::configureSliders()
{
    for (auto* slider : { &volSlider, &speedSlider, &posSlider })
    {
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);  // ✅ Large rotary sliders for better usability.
        slider->setRange(0.0, 1.0); // Default range (adjusted later per slider).

        volSlider.setRange(0.0, 2.0); // ✅ Allow volume boost up to 200%.
        speedSlider.setRange(0.5, 2.0); // ✅ Allow playback speed from half-speed to double-speed.

        // ✅ Custom styling for sliders.
        slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
        slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
        slider->setColour(juce::Slider::backgroundColourId, juce::Colours::black);

        addAndMakeVisible(*slider);
        slider->addListener(this);

        // ✅ Set Number Box Style (Move it to the right for better layout).
        slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 25);
    }

    volSlider.setInterceptsMouseClicks(true, true); // ✅ Allow mouse interaction with volume slider.
    speedSlider.setInterceptsMouseClicks(true, true); // ✅ Allow mouse interaction with speed slider.

    // ✅ Set slider labels for better readability.
    volSlider.setTextValueSuffix(" Volume");
    speedSlider.setTextValueSuffix(" Speed");
    posSlider.setTextValueSuffix(" Position");
}

// Updates the waveform display when a new track is loaded.
// - audioURL: The URL of the track to load into the waveform display.
void DeckGUI::updateWaveform(juce::URL audioURL)
{
    waveformDisplay.loadURL(audioURL);
}

// ✅ Adds keyboard shortcuts for Play, Pause, and Stop functionality.
// - key: The key pressed by the user.
// - component: The component receiving the key event (not used here).
// Returns:
// - true if the key event was handled, false otherwise.
bool DeckGUI::keyPressed(const juce::KeyPress& key, juce::Component*)
{
    if (key.getTextCharacter() == 'p') // 🎵 Press 'P' to start playback.
    {
        player->start();
        return true;
    }
    else if (key.getTextCharacter() == ' ') // ⏸ Press 'Spacebar' to pause playback.
    {
        player->pause();
        return true;
    }
    else if (key.getTextCharacter() == 's') // ⏹ Press 'S' to stop playback.
    {
        player->stop();
        return true;
    }
    return false;
}
