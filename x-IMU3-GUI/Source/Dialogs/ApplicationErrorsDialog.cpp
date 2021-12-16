#include "../ApplicationSettings.h"
#include "ApplicationErrorsDialog.h"
#include "Widgets/TerminalFeed.h"

static TerminalFeed* getTerminalFeed()
{
    class DestroyedAtShutdown : private juce::DeletedAtShutdown, public TerminalFeed
    {
    public:
        DestroyedAtShutdown()
        {
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DestroyedAtShutdown)
    };
    static juce::Component::SafePointer<TerminalFeed> terminalFeed = new DestroyedAtShutdown();
    return terminalFeed;
}

ApplicationErrorsDialog::ApplicationErrorsDialog() : Dialog(BinaryData::warning_white_svg, "Application Errors", "Close", "", &showErrorsButton, std::numeric_limits<int>::max(), true)
{
    addAndMakeVisible(*getTerminalFeed());
    addAndMakeVisible(showErrorsButton);

    getTerminalFeed()->onClear = []
    {
        numberOfUnreadErrors = 0;
    };

    showErrorsButton.onClick = [this]
    {
        ApplicationSettings::getSingleton().showApplicationErrors = showErrorsButton.getToggleState();
        ApplicationSettings::getSingleton().save();
    };

    showErrorsButton.setToggleState(ApplicationSettings::getSingleton().showApplicationErrors, juce::dontSendNotification);

    setSize(800, 480);
}

ApplicationErrorsDialog::~ApplicationErrorsDialog()
{
    // This condition fails if the application is quit while this dialog is shown and the terminal feed is deleted
    if (getTerminalFeed() != nullptr)
    {
        getTerminalFeed()->applyColourToAll(UIColours::grey);
        getTerminalFeed()->setBounds({});
    }

    numberOfUnreadErrors = 0;
}

void ApplicationErrorsDialog::resized()
{
    Dialog::resized();
    getTerminalFeed()->setBounds(getContentBounds(true));
}

void ApplicationErrorsDialog::addError(const juce::String& message)
{
    if (juce::MessageManager::getInstance()->isThisTheMessageThread() == false)
    {
        juce::MessageManager::callAsync([message]
                                        {
                                            addError(message);
                                        });
        return;
    }

    numberOfUnreadErrors = juce::jmin(TerminalFeed::maxNumberOfMessages, (int) numberOfUnreadErrors.getValue() + 1);

    getTerminalFeed()->add(juce::Time::getCurrentTime(), message, juce::Colours::white);

    if (ApplicationSettings::getSingleton().showApplicationErrors && DialogLauncher::getLaunchedDialog() == nullptr)
    {
        DialogLauncher::launchDialog(std::make_unique<ApplicationErrorsDialog>());
    }
}

juce::Value ApplicationErrorsDialog::numberOfUnreadErrors;
