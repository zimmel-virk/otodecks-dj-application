#include "EQComponent.h" 
#include "ThemeComponent.h"

// Constructor: Initializes the EQ sliders and labels.
EQComponent::EQComponent()
{
    // 🎚 Configure EQ Sliders (Rotary Style)
    for (auto* slider : { &bassSlider, &midSlider, &trebleSlider, &reverbSlider, &echoSlider })
    {
        slider->addListener(this);  // ✅ Ensure the slider triggers `sliderValueChanged()`
        slider->setSliderStyle(juce::Slider::Rotary);  // ✅ Use round rotary-style sliders.
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20); // ✅ Display text box below each slider.
        addAndMakeVisible(*slider);
    }

    // 🎚 Configure Bass Slider & Label
    bassLabel.setText("Bass", juce::dontSendNotification);
    bassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bassLabel);
    bassSlider.setRange(0.0, 1.0);
    addAndMakeVisible(bassSlider);

    // 🎚 Configure Mid Slider & Label
    midLabel.setText("Mid", juce::dontSendNotification);
    midLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(midLabel);
    midSlider.setRange(0.0, 1.0);
    addAndMakeVisible(midSlider);

    // 🎚 Configure Treble Slider & Label
    trebleLabel.setText("Treble", juce::dontSendNotification);
    trebleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trebleLabel);
    trebleSlider.setRange(0.0, 1.0);
    addAndMakeVisible(trebleSlider);

    // 🎚 Configure Reverb Slider & Label
    reverbLabel.setText("Reverb", juce::dontSendNotification);
    reverbLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbLabel);
    reverbSlider.setRange(0.0, 1.0);
    addAndMakeVisible(reverbSlider);

    // 🎚 Configure Echo Slider & Label
    echoLabel.setText("Echo", juce::dontSendNotification);
    echoLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(echoLabel);
    echoSlider.setRange(0.0, 1.0);
    addAndMakeVisible(echoSlider);

    // ✅ Configure Save & Load Preset Buttons
    savePresetButton.addListener(this);
    loadPresetButton.addListener(this);
    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(loadPresetButton);
}

// Destructor: Cleans up EQComponent resources.
EQComponent::~EQComponent() {}

// Paints the background of the EQ component.
// - g: JUCE graphics context for drawing UI elements.
void EQComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(15, 15, 15)); // ✅ Dark background color for better contrast.
}

// Resizes the component and positions all sliders, labels, and buttons dynamically.
void EQComponent::resized()
{
    int padding = 10;
    int sliderWidth = (getWidth() - 4 * padding) / 3; // ✅ 3 sliders per row.
    int sliderHeight = 80;
    int labelHeight = 20; // ✅ Label height.
    int buttonHeight = 30;
    int centerX = getWidth() / 2; // ✅ Define `centerX` properly.

    // 🎛 **Position Sliders**
    bassSlider.setBounds(padding, 10, sliderWidth, sliderHeight);
    midSlider.setBounds(padding + sliderWidth + padding, 10, sliderWidth, sliderHeight);
    trebleSlider.setBounds(padding + 2 * (sliderWidth + padding), 10, sliderWidth, sliderHeight);
    reverbSlider.setBounds(padding, 100, sliderWidth, sliderHeight);
    echoSlider.setBounds(padding + sliderWidth + padding, 100, sliderWidth, sliderHeight);

    // 🎛 **Position Labels Below Sliders**
    bassLabel.setBounds(padding, sliderHeight + 5, sliderWidth, labelHeight);
    midLabel.setBounds(padding + sliderWidth + padding, sliderHeight + 5, sliderWidth, labelHeight);
    trebleLabel.setBounds(padding + 2 * (sliderWidth + padding), sliderHeight + 5, sliderWidth, labelHeight);
    reverbLabel.setBounds(padding, 100 + sliderHeight + 5, sliderWidth, labelHeight);
    echoLabel.setBounds(padding + sliderWidth + padding, 100 + sliderHeight + 5, sliderWidth, labelHeight);

    // 📂 **Preset Buttons (Below Sliders)**
    int buttonWidth = getWidth() / 3; // ✅ Adjust button width for better spacing.
    savePresetButton.setBounds(centerX - buttonWidth - 5, getHeight() - 50, buttonWidth - 10, buttonHeight);
    loadPresetButton.setBounds(centerX + 5, getHeight() - 50, buttonWidth - 10, buttonHeight);
}

// Handles changes in slider values and applies corresponding effects.
void EQComponent::sliderValueChanged(juce::Slider* slider)
{
    float value = slider->getValue();

    if (player) // ✅ Ensure player exists before applying effects.
    {
        if (slider == &bassSlider)
        {
            DBG("🎚 Bass Adjusted: " + juce::String(value));
            player->setBass(value); // ✅ Apply bass change.
        }
        else if (slider == &midSlider)
        {
            DBG("🎚 Mid Adjusted: " + juce::String(value));
            player->setMid(value); // ✅ Apply mid change.
        }
        else if (slider == &trebleSlider)
        {
            DBG("🎚 Treble Adjusted: " + juce::String(value));
            player->setTreble(value); // ✅ Apply treble change.
        }
        else if (slider == &reverbSlider)
        {
            DBG("🎛 Reverb Adjusted: " + juce::String(value));
            player->setReverb(value); // ✅ Apply reverb change.
        }
        else if (slider == &echoSlider)
        {
            DBG("🎤 Echo Adjusted: " + juce::String(value));
            player->setEcho(value); // ✅ Apply echo change.
        }
        player->updateEQ(); // ✅ Ensure changes are applied immediately.
    }
}

// Handles button clicks for saving and loading EQ presets.
void EQComponent::buttonClicked(juce::Button* button)
{
    if (button == &savePresetButton)
    {
        saveEQPreset();
    }
    else if (button == &loadPresetButton)
    {
        loadEQPreset();
    }
}

// Saves the current EQ settings to a JSON file.
void EQComponent::saveEQPreset()
{
    juce::File presetFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("EQPreset.json");

    juce::DynamicObject* jsonObject = new juce::DynamicObject();
    jsonObject->setProperty("bass", bassSlider.getValue());
    jsonObject->setProperty("mid", midSlider.getValue());
    jsonObject->setProperty("treble", trebleSlider.getValue());
    jsonObject->setProperty("reverb", reverbSlider.getValue());
    jsonObject->setProperty("echo", echoSlider.getValue());

    juce::var jsonVar(jsonObject);
    presetFile.replaceWithText(juce::JSON::toString(jsonVar)); // ✅ Save JSON data to file.

    DBG("Preset Saved: " + presetFile.getFullPathName());
}

// Loads previously saved EQ settings from a JSON file.
void EQComponent::loadEQPreset()
{
    juce::File presetFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("EQPreset.json");

    if (!presetFile.existsAsFile())
    {
        DBG("❌ Preset file does not exist.");
        return;
    }

    juce::var json = juce::JSON::parse(presetFile.loadFileAsString());

    if (!json.isObject())
    {
        DBG("❌ Error: Invalid JSON format.");
        return;
    }

    bassSlider.setValue(json["bass"], juce::sendNotification);
    midSlider.setValue(json["mid"], juce::sendNotification);
    trebleSlider.setValue(json["treble"], juce::sendNotification);
    reverbSlider.setValue(json["reverb"], juce::sendNotification);
    echoSlider.setValue(json["echo"], juce::sendNotification);

    DBG("✅ EQ Preset Loaded!");
}

// Applies the theme colors to the component.

void EQComponent::applyTheme()
{
    bassSlider.setColour(juce::Slider::textBoxTextColourId, ThemeComponent::getTextColor());
    midSlider.setColour(juce::Slider::textBoxTextColourId, ThemeComponent::getTextColor());
    trebleSlider.setColour(juce::Slider::textBoxTextColourId, ThemeComponent::getTextColor());
    repaint();
}

// Sets the reference to the DJAudioPlayer.

void EQComponent::setPlayer(DJAudioPlayer* newPlayer)
{
    player = newPlayer; // ✅ Store the reference to DJAudioPlayer
}
