#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"

/**
 * 🎵 PlaylistComponent - Handles track management for the DJ application.
 *
 * - Displays a **table** of loaded tracks.
 * - Allows users to **load, save, and auto-sort** playlists.
 * - Supports **drag-and-drop** sorting and double-clicking to load tracks into decks.
 * - Auto-detects **BPM, Key, and Duration** for each track.
 */
class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel, // ✅ Implements table model for displaying tracks
    public juce::Button::Listener,  // ✅ Handles button interactions
    public juce::DragAndDropContainer // ✅ Enables drag-and-drop sorting
{
public:
    /**
     * 🎧 Constructor - Initializes the Playlist Component.
     * @param player1 Pointer to DJAudioPlayer for Deck 1.
     * @param player2 Pointer to DJAudioPlayer for Deck 2.
     * @param deck1 Pointer to DeckGUI for Deck 1.
     * @param deck2 Pointer to DeckGUI for Deck 2.
     */
    PlaylistComponent(DJAudioPlayer* player1, DJAudioPlayer* player2, DeckGUI* deck1, DeckGUI* deck2);
    ~PlaylistComponent() override;

    // 🎨 JUCE Component Overrides
    void paint(juce::Graphics&) override;
    void resized() override;

    // 📋 JUCE TableListBoxModel Overrides
    int getNumRows() override;
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    // 🎛 Button and Table Event Handling
    void buttonClicked(juce::Button* button) override;
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;

    void loadTracksFromFileBrowser();
    void moveTrack(int fromIndex, int toIndex);
    juce::File getTrackFile(int index); // ✅ NEW: Allows AutoMix to access tracks

    void applyTheme();
    void loadPlaylist();   // ✅ Load playlist from file
    void savePlaylist();   // ✅ Save playlist to file
    void autoQueueTracks(); // ✅ Automatically sort tracks based on BPM & Key Matching

    double detectBPM(const juce::File& file); // ✅ Declare detectBPM() in the header file


private:
    // 🎛 UI Components
    juce::TableListBox tableComponent; // ✅ Table for displaying track list
    juce::TextButton loadButton{ "Load Tracks" };  // ✅ Load tracks manually
    juce::TextButton loadplaylistButton{ "Load Playlist" }; // ✅ Load saved playlist
    juce::TextButton saveButton{ "Save Playlist" }; // ✅ Save current playlist
    juce::TextButton autoQueueButton{ "Auto-Queue" }; // ✅ Sort tracks by BPM

    juce::AudioFormatManager formatManager; // ✅ Manages audio file formats

    /**
     * 🎵 **TrackInfo** - Stores metadata for each track.
     */
    struct TrackInfo
    {
        juce::String title; // ✅ Track title
        double bpm; // ✅ Beats per minute (BPM)
        juce::String key; // ✅ Musical key
        juce::File file; // ✅ File path
    };

    juce::Array<TrackInfo> trackList; // ✅ Stores track metadata
    std::vector<juce::File> trackFiles; // ✅ Stores raw file paths

    void addTrack(const juce::File& file);// ✅ Adds a track to the playlist

  
    // 🎛 DJ Components
    DJAudioPlayer* player1; // ✅ Reference to Deck 1's player
    DJAudioPlayer* player2; // ✅ Reference to Deck 2's player
    DeckGUI* deck1; // ✅ Reference to Deck 1 UI
    DeckGUI* deck2; // ✅ Reference to Deck 2 UI

    juce::StringArray trackTitles; // ✅ Stores track names for UI

    double detectTrackLength(const juce::File& file);
   
    juce::String detectKey(const juce::File& file);

    

    // 🎨 UI Colors
    juce::Colour backgroundColour = juce::Colour(20, 20, 20); // ✅ Dark theme background
    juce::Colour rowHighlightColour = juce::Colour(0, 150, 250); // ✅ Highlight selected row
    juce::Colour textColour = juce::Colour(220, 220, 220); // ✅ Text color

    std::map<juce::File, double> bpmCache; // ✅ Cache BPM values to prevent recalculations
   
   

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
