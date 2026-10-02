#include "PlaylistComponent.h"
#include "ThemeComponent.h"
#include <algorithm> // ✅ Required for std::sort

// 📌 **BPM Cache**: Stores detected BPM values to avoid redundant calculations.
std::map<juce::File, double> bpmCache;

// 🎵 **PlaylistComponent: Manages playlist functionality** 
// - Displays track metadata (title, length, BPM, key).
// - Loads, saves, and auto-sorts tracks.
// - Handles drag-and-drop loading and deck assignment.
PlaylistComponent::PlaylistComponent(DJAudioPlayer* player1, DJAudioPlayer* player2, DeckGUI* deck1, DeckGUI* deck2)
    : player1(player1), player2(player2), deck1(deck1), deck2(deck2)
{
    formatManager.registerBasicFormats(); // ✅ Ensure format manager supports basic file types.

    // ✅ Make table and buttons visible.
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loadplaylistButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(autoQueueButton);

    // 🎛 **Set Up Table Columns**
    tableComponent.getHeader().addColumn("Track Title", 1, 250);
    tableComponent.getHeader().addColumn("Length", 2, 100);  // ✅ Track duration.
    tableComponent.getHeader().addColumn("BPM", 3, 80);  // ✅ Beats per minute.
    tableComponent.getHeader().addColumn("Key", 4, 80);  // ✅ Musical key.
    tableComponent.getHeader().addColumn("Deck 1", 5, 100);
    tableComponent.getHeader().addColumn("Deck 2", 6, 100);
    tableComponent.getHeader().addColumn("Remove", 7, 100);

    tableComponent.setModel(this);
    tableComponent.setColour(juce::TableListBox::backgroundColourId, backgroundColour);
    tableComponent.setColour(juce::TableListBox::textColourId, textColour);

    // 🎛 **Configure Button Actions**
    loadButton.onClick = [this] { loadTracksFromFileBrowser(); };
    loadplaylistButton.onClick = [this] { loadPlaylist(); };
    saveButton.onClick = [this] { savePlaylist(); };
    autoQueueButton.onClick = [this] { autoQueueTracks(); };

    // 🎨 **Set Button Colors**
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    loadplaylistButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    autoQueueButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
}

// ✅ Destructor: Cleanup.
PlaylistComponent::~PlaylistComponent() {}

// ✅ Returns the number of tracks in the playlist.
int PlaylistComponent::getNumRows() { return trackFiles.size(); }

// 🎨 **Paints the background of each row.**
void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(rowHighlightColour);
    else
        g.fillAll(rowNumber % 2 == 0 ? juce::Colour(30, 30, 30) : juce::Colour(40, 40, 40));
}

// 🎨 **Paints the content inside each cell (track title, BPM, etc.).**
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(textColour);

    if (columnId == 1)  // Track Title
        g.drawText(trackFiles[rowNumber].getFileName(), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    else if (columnId == 2)  // Length
        g.drawText(juce::String(detectTrackLength(trackFiles[rowNumber])) + " sec", 2, 0, width - 4, height, juce::Justification::centred, true);
    else if (columnId == 3)  // BPM
        g.drawText(juce::String(detectBPM(trackFiles[rowNumber])), 2, 0, width - 4, height, juce::Justification::centred, true);
    else if (columnId == 4)  // Key
        g.drawText(detectKey(trackFiles[rowNumber]), 2, 0, width - 4, height, juce::Justification::centred, true);
}

// ✅ **Creates interactive buttons for loading tracks into decks or removing them.**
juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (columnId == 5 || columnId == 6 || columnId == 7)  // ✅ Buttons only for Deck 1, Deck 2, Remove
    {
        auto* btn = dynamic_cast<juce::TextButton*>(existingComponentToUpdate);

        if (!btn)
        {
            btn = new juce::TextButton();
            btn->setComponentID(std::to_string(rowNumber) + "-" + std::to_string(columnId));
            btn->addListener(this);
        }

        if (columnId == 5) btn->setButtonText("Deck 1");
        else if (columnId == 6) btn->setButtonText("Deck 2");
        else if (columnId == 7) btn->setButtonText("Remove");

        return btn;
    }

    return nullptr;
}

// 🎯 **Handles button clicks for loading tracks into decks or removing them.**
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    juce::String componentID = button->getComponentID();
    int dashPos = componentID.indexOfChar('-');
    int id = componentID.substring(0, dashPos).getIntValue();
    int columnId = componentID.substring(dashPos + 1).getIntValue();

    if (id >= 0 && id < trackFiles.size())
    {
        juce::URL fileURL = juce::URL(trackFiles[id]);

        if (columnId == 5)  // ✅ Load into Deck 1
        {
            player1->loadURL(fileURL);
            player1->start();
            deck1->updateWaveform(fileURL);
        }
        else if (columnId == 6)  // ✅ Load into Deck 2
        {
            player2->loadURL(fileURL);
            player2->start();
            deck2->updateWaveform(fileURL);
        }
        else if (columnId == 7)  // ✅ Remove Track
        {
            trackFiles.erase(trackFiles.begin() + id);
            tableComponent.updateContent();
            repaint();
        }
    }
    DBG("📢 Button Clicked: " + button->getButtonText());

    if (button == &loadplaylistButton)
    {
        DBG("📂 Loading playlist...");
        loadPlaylist();
    }
    else if (button == &saveButton)
    {
        DBG("💾 Saving playlist...");
        savePlaylist();
    }
    else if (button == &autoQueueButton)
    {
        DBG("🔀 Auto-Queueing tracks...");
        autoQueueTracks();
    }
}

//  **Double-Click Track to Load It into Deck 1**
void PlaylistComponent::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
    if (rowNumber >= 0 && rowNumber < trackFiles.size())
    {
        juce::URL fileURL = juce::URL(trackFiles[rowNumber]);
        player1->loadURL(fileURL);
        player1->start();
        deck1->updateWaveform(fileURL);
    }
}

// Drag & Drop Sorting
void PlaylistComponent::moveTrack(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < trackFiles.size() && toIndex >= 0 && toIndex < trackFiles.size())
    {
        juce::File temp = trackFiles[fromIndex];
        trackFiles.erase(trackFiles.begin() + fromIndex);
        trackFiles.insert(trackFiles.begin() + toIndex, temp);
        tableComponent.updateContent();
        repaint();
    }
}
void PlaylistComponent::loadTracksFromFileBrowser()
{
    // 📂 **File Chooser to Load Multiple Audio Tracks**
    static std::unique_ptr<juce::FileChooser> chooser;
    chooser = std::make_unique<juce::FileChooser>("Select Audio Files", juce::File(), "*.wav;*.mp3;*.ogg");

    chooser->launchAsync(juce::FileBrowserComponent::canSelectMultipleItems,
        [this](const juce::FileChooser& fc)
        {
            auto files = fc.getResults();

            if (files.isEmpty()) return; // ✅ Do nothing if no files were selected.

            for (const auto& file : files)
            {
                trackFiles.push_back(file);
            }

            tableComponent.updateContent(); // ✅ Refresh table with new tracks.
            repaint();
        });
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour); // 🎨 Apply background color.
}

void PlaylistComponent::resized()
{
    // 📌 **Layout Constants**
    int padding = 10;
    int buttonHeight = 40;
    int tableHeight = getHeight() - buttonHeight - (3 * padding);
    int buttonWidth = (getWidth() - (5 * padding)) / 4;  // ✅ Equal width for 4 buttons.

    // 🎛 **Position Playlist Table**
    tableComponent.setBounds(padding, padding, getWidth() - (2 * padding), tableHeight);

    // 🎛 **Arrange Buttons in a Row Below Table**
    loadButton.setBounds(padding, tableHeight + (2 * padding), buttonWidth, buttonHeight);
    loadplaylistButton.setBounds(padding + buttonWidth + padding, tableHeight + (2 * padding), buttonWidth, buttonHeight);
    saveButton.setBounds(padding + 2 * (buttonWidth + padding), tableHeight + (2 * padding), buttonWidth, buttonHeight);
    autoQueueButton.setBounds(padding + 3 * (buttonWidth + padding), tableHeight + (2 * padding), buttonWidth, buttonHeight);
}

juce::File PlaylistComponent::getTrackFile(int index)
{
    if (index < 0 || index >= trackFiles.size())
    {
        DBG("❌ Error: getTrackFile() - Invalid index: " + juce::String(index));
        return juce::File(); // ✅ Return an empty file to prevent crashes.
    }

    return trackFiles[index];
}

void PlaylistComponent::applyTheme()
{
    // 🎨 **Apply Theme Colors to Playlist**
    tableComponent.setColour(juce::TableListBox::backgroundColourId, ThemeComponent::getBackgroundColor());
    tableComponent.setColour(juce::TableListBox::textColourId, ThemeComponent::getTextColor());
    repaint();
}

void PlaylistComponent::savePlaylist()
{
    // 💾 **Save Playlist to a JSON File**
    juce::File playlistFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("DJ_Playlist.json");

    juce::Array<juce::var> jsonArray;

    for (const auto& track : trackFiles)
    {
        jsonArray.add(track.getFullPathName()); // ✅ Store full file path.
    }

    playlistFile.replaceWithText(juce::JSON::toString(jsonArray));
    DBG("✅ Playlist saved!");
}

void PlaylistComponent::loadPlaylist()
{
    // 📂 **Load Playlist from JSON File**
    juce::File playlistFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("DJ_Playlist.json");

    if (!playlistFile.existsAsFile())
    {
        DBG("❌ No saved playlist found.");
        return;
    }

    juce::var jsonData = juce::JSON::parse(playlistFile.loadFileAsString());
    if (!jsonData.isArray()) return; // ✅ Ensure valid JSON format.

    trackFiles.clear();
    for (const auto& item : *jsonData.getArray())
    {
        trackFiles.push_back(juce::File(item.toString())); // ✅ Convert JSON string back to File.
    }

    tableComponent.updateContent(); // ✅ Refresh UI.
    DBG("✅ Playlist loaded!");
}

void PlaylistComponent::autoQueueTracks()
{
    if (trackFiles.size() < 2)
    {
        DBG("⚠️ Not enough tracks to auto-sort!");
        return;
    }

    // 🔀 **Sort tracks by BPM (ascending), fallback to alphabetical order**
    std::sort(trackFiles.begin(), trackFiles.end(), [this](const juce::File& a, const juce::File& b)
        {
            double bpmA = detectBPM(a);
            double bpmB = detectBPM(b);

            if (bpmA == bpmB)
            {
                return a.getFileName().toLowerCase() < b.getFileName().toLowerCase(); // Alphabetical sorting.
            }

            return bpmA < bpmB; // ✅ Sort by increasing BPM.
        });

    tableComponent.updateContent(); // ✅ Refresh UI to reflect new order.
    repaint();

    // ✅ Update track titles in the UI.
    trackTitles.clear();
    for (const auto& file : trackFiles)
    {
        trackTitles.add(file.getFileNameWithoutExtension());
    }

    DBG("✅ Tracks auto-sorted by BPM and playlist updated!");
}

double PlaylistComponent::detectBPM(const juce::File& file)
{
    if (bpmCache.find(file) != bpmCache.end()) // ✅ If BPM already calculated, use cached value.
    {
        return bpmCache[file];
    }

    // 🎵 **Create a temporary DJAudioPlayer to analyze BPM**
    DJAudioPlayer tempPlayer(formatManager);
    tempPlayer.loadURL(juce::URL(file));

    double bpm = tempPlayer.detectBPM();
    bpmCache[file] = bpm; // ✅ Store BPM for future use.
    return bpm;
}

void PlaylistComponent::addTrack(const juce::File& file)
{
    // 📌 **[To be implemented]**
}

double PlaylistComponent::detectTrackLength(const juce::File& file)
{
    // ⏱ **Detects the length of an audio track**
    auto* reader = formatManager.createReaderFor(file);
    if (reader)
    {
        double lengthInSeconds = reader->lengthInSamples / reader->sampleRate; // ✅ Convert to seconds.
        delete reader;
        return lengthInSeconds;
    }
    return 0.0;
}

juce::String PlaylistComponent::detectKey(const juce::File& file)
{
    return "C Major"; // 🚨 Placeholder: Replace with real key detection algorithm.
}
