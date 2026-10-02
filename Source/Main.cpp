#include <JuceHeader.h>
#include "MainComponent.h"

// ✅ OtoDecksApplication: The main entry point for the DJ application.
// - Manages the app lifecycle, including startup, shutdown, and multiple instances.
class OtoDecksApplication : public juce::JUCEApplication
{
public:
    // ✅ Constructor: Initializes the application instance.
    OtoDecksApplication() {}

    // ✅ Returns the name of the application.
    const juce::String getApplicationName() override { return "OtoDecks"; }

    // ✅ Returns the application version.
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    // ✅ Allows multiple instances of the application to run simultaneously.
    bool moreThanOneInstanceAllowed() override { return true; }

    // ✅ Called when the application starts.
    // - Creates and initializes the main application window.
    void initialise(const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    // ✅ Called when the application is shutting down.
    // - Cleans up any allocated resources.
    void shutdown() override
    {
        mainWindow = nullptr; // ✅ Deletes the main window safely.
    }

    // ✅ Handles system requests to quit the application.
    void systemRequestedQuit() override
    {
        quit(); // ✅ Gracefully exits the application.
    }

    // ✅ Called when another instance of the application starts.
    // - Logs the command line input for debugging purposes.
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        juce::Logger::writeToLog("Another instance started: " + commandLine);
    }

    // ✅ MainWindow Class: Manages the main application window.
    class MainWindow : public juce::DocumentWindow
    {
    public:
        // ✅ Constructor: Creates the main application window.
        // - name: The title of the window.
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::black,  // ✅ Set window background to black.
                juce::DocumentWindow::allButtons) // ✅ Allow all window buttons (close, minimize, maximize).
        {
            setUsingNativeTitleBar(true);   // ✅ Use native window styling.
            setContentOwned(new MainComponent(), true); // ✅ Attach the main UI component.
            setResizable(true, true);       // ✅ Enable window resizing.
            setResizeLimits(800, 600, 1920, 1080); // ✅ Set min and max window sizes.
            //centreWithSize(900, 650);       // ✅ Center window with default size.

            // ✅ Set the window to full screen
            setFullScreen(true);
            setVisible(true);               // ✅ Show the window.
        }

        // ✅ Called when the close button is pressed.
        // - Requests application shutdown.
        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow; // ✅ Smart pointer to manage the main window.
};

// ✅ Starts the JUCE application by creating an instance of OtoDecksApplication.
START_JUCE_APPLICATION(OtoDecksApplication)
