#include "MainComponent.h"
#include "ThemeComponent.h"

// ✅ Constructor: Initializes all components and sets up the UI layout.
MainComponent::MainComponent()
    : player1(formatManager), player2(formatManager),
    deckGUI1(&player1, &bpmComponent, formatManager, audioThumbnailCache), // ✅ Deck 1 UI
    deckGUI2(&player2, &bpmComponent, formatManager, audioThumbnailCache), // ✅ Deck 2 UI
    playlistComponent(&player1, &player2, &deckGUI1, &deckGUI2), // ✅ Playlist Manager
    waveformDisplay1(formatManager, audioThumbnailCache), // ✅ Waveform for Deck 1
    waveformDisplay2(formatManager, audioThumbnailCache), // ✅ Waveform for Deck 2
    autoMix(&player1, &player2, &playlistComponent, &waveformDisplay1, &waveformDisplay2) // ✅ Auto DJ System
{
    setSize(1000, 700); // ✅ Set default window size

    // ✅ Check and request audio recording permissions
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { if (granted) setAudioChannels(2, 2); });
    }
    else
    {
        setAudioChannels(2, 2); // ✅ Initialize with 2 input & 2 output channels
    }

    formatManager.registerBasicFormats(); // ✅ Register standard audio formats (MP3, WAV, etc.)

    // ✅ Button Actions (Click Handlers)
    loadButton.onClick = [this] { playlistComponent.loadTracksFromFileBrowser(); };
    syncBPMButton.onClick = [this] { bpmComponent.syncBPM(); };
    loopButton.onClick = [this] { player1.toggleLoop(); player2.toggleLoop(); };

    // ✅ Style Buttons
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(50, 50, 50));
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    syncBPMButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orange);

    // ✅ Add UI Components to the Main Window
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);
    addAndMakeVisible(eqComponent);
    addAndMakeVisible(bpmComponent);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(syncBPMButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(autoMix);
    addAndMakeVisible(crossfadeComponent);

    // ✅ Theme Selector (Dropdown to change UI appearance)
    themeSelector.addItem("Dark Mode", 1);
    themeSelector.addItem("Light Mode", 2);
    themeSelector.addItem("Neon Mode", 3);
    themeSelector.addItem("Custom Mode", 4);
    themeSelector.onChange = [this] { applyTheme(); };
    addAndMakeVisible(themeSelector);

    addAndMakeVisible(micInput); // ✅ Microphone Input Component
}

// ✅ Destructor: Cleans up resources when the component is deleted.
MainComponent::~MainComponent()
{
    shutdownAudio(); // ✅ Shut down audio processing before closing.
}

// ✅ prepareToPlay: Prepares audio players before playback starts.
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

// ✅ getNextAudioBlock: Fetches the next block of audio to be played.
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

// ✅ releaseResources: Releases audio resources when playback stops.
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

// ✅ paint: Draws the background of the main UI.
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour); // ✅ Set background color.
}

// ✅ resized: Positions and resizes all UI components dynamically.
void MainComponent::resized()
{
    int padding = 10;
    int buttonHeight = 40;

    int leftColumnWidth = getWidth() * 0.60 - padding * 2;
    int rightColumnWidth = getWidth() * 0.35 - padding * 2;

    int deckHeight = getHeight() * 0.30;  // ✅ Increased deck height.
    int bottomHeight = getHeight() * 0.20;  // ✅ Ensure EQ fits properly.

    // 🎛 **Left Column: Two Decks and Waveforms**
    deckGUI1.setBounds(padding, padding, leftColumnWidth, deckHeight);
    waveformDisplay1.setBounds(padding, deckHeight - 60, leftColumnWidth, 50);

    deckGUI2.setBounds(padding, deckHeight + padding * 2, leftColumnWidth, deckHeight);
    waveformDisplay2.setBounds(padding, deckHeight * 2 - 60, leftColumnWidth, 50);

    // 🎛 **Right Column: AutoMix, EQ, and Playlist**
    int rightColumnX = getWidth() - rightColumnWidth - padding;  // ✅ Align to top right

    autoMix.setBounds(rightColumnX, padding, rightColumnWidth, 90);
    waveformDisplay2.setBounds(rightColumnX, padding + 100, rightColumnWidth, 120);
    eqComponent.setBounds(rightColumnX, padding + 100, rightColumnWidth, 270);

    // 📂 **Playlist Positioned on Right**
    playlistComponent.setBounds(getWidth() - rightColumnWidth - padding - 50, padding, rightColumnWidth + 50, getHeight() - 2 * padding - 20);

    // 🎛 **Crossfade Slider (Between Decks)**
    int crossfadeWidth = 60;
    int crossfadeHeight = deckGUI1.getHeight() * 2;
    crossfadeComponent.setBounds(deckGUI1.getRight() + 10, deckGUI1.getY(), crossfadeWidth, crossfadeHeight);

    // 🎛 **Theme Selector (Dropdown at the Top)**
    themeSelector.setBounds(getWidth() - rightColumnWidth - padding - 20, getHeight() - bottomHeight - 210, rightColumnWidth - 400, 30);

    // ✅ Microphone Input Section
    int recordingSectionX = eqComponent.getX();
    int recordingSectionY = crossfadeComponent.getBottom() + 10;
    micInput.setBounds(recordingSectionX + 100, recordingSectionY - 100, rightColumnWidth, 150);

    int eqPresetBottom = crossfadeComponent.getBottom() + padding; // ✅ Place below EQ Preset buttons

    // ✅ Move Playlist to Bottom
    playlistComponent.setBounds(10, getHeight() - 250, getWidth() - 20, 240);
}

// ✅ applyTheme: Changes the UI theme based on user selection.
void MainComponent::applyTheme()
{
    int selectedTheme = themeSelector.getSelectedId();
    ThemeComponent::applyTheme(selectedTheme, this);

    // ✅ Apply theme to all subcomponents
    deckGUI1.applyTheme();
    deckGUI2.applyTheme();
    playlistComponent.applyTheme();
    waveformDisplay1.applyTheme();
    waveformDisplay2.applyTheme();
    eqComponent.applyTheme();
    bpmComponent.applyTheme();
    autoMix.applyTheme();
    crossfadeComponent.applyTheme();
}
