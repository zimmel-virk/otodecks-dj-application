# OtoDecks — C++ DJ Application

OtoDecks is a desktop DJ application developed in **C++ with the JUCE framework**. It provides two independent audio decks together with waveform visualisation, playlist management, playback-speed control, looping, BPM-related controls, crossfading, equalisation, microphone recording, themes and an automatic mixing workflow.

The repository preserves the original JUCE project and source files unchanged.

## Main Features

### Dual-deck audio playback

The application creates two `DJAudioPlayer` instances and two corresponding `DeckGUI` interfaces.

Each deck provides:

- play;
- pause;
- stop;
- track loading;
- looping;
- volume control;
- playback-speed control;
- track-position control;
- one-beat forward/backward jumps;
- BPM-related controls;
- waveform display.

Supported audio loading is handled through JUCE's `AudioFormatManager`.

The file browser in the project accepts:

```text
WAV
MP3
OGG
```

## Waveform Display

Each deck includes a waveform component based on JUCE's `AudioThumbnail`.

The waveform interface supports:

- waveform rendering;
- current-position indication;
- zoom control;
- scrolling through the waveform;
- theme-dependent waveform colours.

The waveform is updated when a new track is loaded into a deck.

## Playlist Management

`PlaylistComponent` provides a table-based track library.

The playlist displays:

```text
Track Title
Length
BPM
Key
Deck 1
Deck 2
Remove
```

Tracks can be loaded through a multi-file browser and assigned directly to either deck.

The component also includes functionality for:

- removing tracks;
- double-click loading;
- moving tracks within the playlist;
- saving a playlist;
- loading a playlist;
- automatic queue ordering;
- BPM caching.

The project stores saved playlist information as JSON in the user's documents directory.

## BPM Handling

The project contains several BPM-related functions.

`DJAudioPlayer` includes a basic BPM-estimation routine that analyses the first section of a loaded track and counts threshold crossings.

`BPMComponent` provides synchronization controls between the two players by adjusting playback speed.

Deck-level controls also allow playback-speed adjustment relative to a target BPM.

The implementation is intentionally preserved as written in the original project.

## Beat Navigation

Each deck includes:

```text
<< Beat
>> Beat
```

controls.

The `jumpByBeats()` method calculates the approximate duration of a beat from the stored BPM value and moves the current playback position backward or forward while keeping it inside the track boundaries.

## Crossfading

A dedicated `CrossfadeComponent` provides a crossfade slider between the two decks.

The automatic-mixing component also performs gradual gain transitions between the active player and the next track.

## Auto DJ / Automatic Mixing

`AutoMixComponent` implements an automatic mixing workflow.

The component:

1. checks that the playlist contains enough tracks;
2. loads tracks into the two players;
3. starts playback;
4. monitors playback progress using a timer;
5. begins a transition near the end of the active track;
6. obtains the next playlist track;
7. gradually changes player gain during the transition;
8. swaps the active deck/waveform references for the next cycle.

The source also contains logic for BPM-aware transition handling.

## Equaliser and Effects Controls

`EQComponent` provides rotary controls for:

```text
Bass
Mid
Treble
Reverb
Echo
```

The audio-player class contains a JUCE DSP processor chain consisting of:

- low-shelf IIR filter;
- midrange peak filter;
- high-shelf IIR filter;
- JUCE reverb processor;
- JUCE delay line.

Bass, midrange and treble coefficient updates are defined in `DJAudioPlayer::updateEQ()`.

EQ settings can also be saved and loaded using a JSON preset file in the user's documents directory.

## Jog Wheel

`JogWheelComponent` implements a graphical jog-wheel interface.

Mouse interaction changes playback speed while dragging, allowing temporary track-speed manipulation. When the interaction finishes, playback speed returns to normal.

The component also renders the wheel's visual rotation and current speed.

## Microphone Recording

`MicInputComponent` adds microphone-input functionality.

The component includes controls to:

- start recording;
- stop recording;
- select/apply effects;
- open the recorded file.

The application requests audio-recording permission when necessary and configures audio input/output channels through JUCE.

The microphone component contains simple effect modes including:

- echo;
- simplified pitch modification;
- robot-voice processing.

## Beat Visualisation

`BeatVisualizer` provides a timer-driven real-time visual display based on audio-level information.

`DJAudioPlayer` calculates the peak magnitude of active audio buffers so that current signal level can be used for visual feedback.

## Themes

`ThemeComponent` defines multiple visual modes:

```text
Dark Mode
Light Mode
Neon Mode
Custom Mode
```

Theme state affects shared colours such as:

- application background;
- buttons;
- text;
- waveforms.

Individual interface components expose theme-refresh functions so the visual appearance can be updated across the application.

## Application Structure

The source is divided into focused JUCE components:

```text
Source/
├── Main.cpp
├── MainComponent.cpp / .h
├── DJAudioPlayer.cpp / .h
├── DeckGUI.cpp / .h
├── WaveformDisplay.cpp / .h
├── PlaylistComponent.cpp / .h
├── BPMComponent.cpp / .h
├── CrossfadeComponent.cpp / .h
├── AutoMixComponent.cpp / .h
├── EQComponent.cpp / .h
├── JogWheelComponent.cpp / .h
├── MicInputComponent.cpp / .h
├── BeatVisualizer.cpp / .h
└── ThemeComponent.cpp / .h
```

The JUCE project definition is:

```text
NewProject.jucer
```

The application name defined in `Main.cpp` is:

```text
OtoDecks
```

## JUCE Modules

The original `.jucer` file references the following JUCE modules:

```text
juce_audio_basics
juce_audio_devices
juce_audio_formats
juce_audio_processors
juce_audio_utils
juce_core
juce_data_structures
juce_dsp
juce_events
juce_graphics
juce_gui_basics
juce_gui_extra
```

## Building the Project

The supplied `NewProject.jucer` file contains Visual Studio 2019 and Visual Studio 2022 exporters and references the JUCE module locations used on the original development machine.

To build on another system:

1. install JUCE;
2. open `NewProject.jucer` in Projucer;
3. update the JUCE module paths if necessary;
4. select or add an exporter for the local development environment;
5. save the project;
6. open the generated IDE project and build it.

The local JUCE paths inside the original `.jucer` file are preserved unchanged.

## Technologies

- C++
- JUCE
- JUCE DSP
- Object-Oriented Programming
- real-time audio processing
- desktop GUI development
- JSON persistence
- audio waveform rendering

## Original-File Preservation

All original `.cpp`, `.h` and `.jucer` files were copied without modifying their contents.

Only the following repository-support files were added:

```text
README.md
.gitignore
ORIGINAL_FILES_VERIFIED.txt
```

Operating-system metadata such as `.DS_Store` is excluded.

## Author

**Zimmel Javed Virk**  
BSc Computer Science — University of London
