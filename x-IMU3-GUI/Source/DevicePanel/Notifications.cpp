#include "../ApplicationSettings.h"
#include "Notifications.h"

Notifications::Notifications()
{
    addAndMakeVisible(feed);
    addAndMakeVisible(showNotificationsButton);
    addAndMakeVisible(closeButton);
    addAndMakeVisible(title);

    showNotificationsButton.setClickingTogglesState(true);
    showNotificationsButton.setToggleState(ApplicationSettings::getSingleton().showNotificationAndErrorMessages, juce::dontSendNotification);
    showNotificationsButton.onClick = [this]
    {
        ApplicationSettings::getSingleton().showNotificationAndErrorMessages = showNotificationsButton.getToggleState();
        ApplicationSettings::getSingleton().save();
    };

    closeButton.onClick = [this]
    {
        show(false);
    };

    ApplicationSettings::getSingleton().showNotificationAndErrorMessages.getValueTree().addListener(this);
}

Notifications::~Notifications()
{
    ApplicationSettings::getSingleton().showNotificationAndErrorMessages.getValueTree().removeListener(this);
}

void Notifications::paint(juce::Graphics& g)
{
    g.setColour(UIColours::menuStrip);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("Notifications", getLocalBounds(), juce::Justification::centred, true);
}

void Notifications::resized()
{
    auto bounds = getLocalBounds();
    auto headerButtonsBounds = bounds.removeFromTop(24);
    feed.setBounds(bounds);
    title.setBounds(headerButtonsBounds.withTrimmedLeft(3));
    closeButton.setBounds(headerButtonsBounds.removeFromRight(headerButtonsBounds.getHeight()).reduced(2));
    showNotificationsButton.setBounds(headerButtonsBounds.removeFromRight(headerButtonsBounds.getHeight()).reduced(2));
}

void Notifications::show(const bool showOrHide)
{
    auto& animator = juce::Desktop::getInstance().getAnimator();
    if (showOrHide)
    {
        if (isTimerRunning() == false)
        {
            animator.fadeIn(this, fadeDuration);
        }

        startTimer(showDuration);
    }
    else
    {
        stopTimer();
        animator.fadeOut(this, fadeDuration);
    }
}

TerminalFeed& Notifications::getFeed()
{
    return feed;
}

void Notifications::timerCallback()
{
    if (isMouseOver(true))
    {
        startTimer(500);
    }
    else
    {
        show(false);
    }
}

void Notifications::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    ApplicationSettings::getSingleton().showNotificationAndErrorMessages.forceUpdateOfCachedValue();
    showNotificationsButton.setToggleState(ApplicationSettings::getSingleton().showNotificationAndErrorMessages, juce::dontSendNotification);
}
