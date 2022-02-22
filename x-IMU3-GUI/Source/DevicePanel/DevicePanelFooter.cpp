#include "ApplicationSettings.h"
#include "CustomLookAndFeel.h"
#include "DevicePanelFooter.h"
#include "DevicePanelHeader.h"

DevicePanelFooter::DevicePanelFooter(Notifications& notificationsPopup_, ximu3::Connection& connection_) : notificationsPopup(notificationsPopup_),
                                                                                                           connection(connection_)
{
    addAndMakeVisible(statisticsLabel);

    statisticsCallback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            std::ostringstream statisticsLabelText;
                                            statisticsLabelText << " " << message.message_rate << " messages/s";
                                            statisticsLabelText << " (" << std::fixed << std::setprecision(2) << message.data_rate / 1000.0f << " kB/s) ";
                                            statisticsLabel.setText(statisticsLabelText.str());
                                            resized();
                                        });
    };
    statisticsCallbackID = connection.addStatisticsCallback(statisticsCallback);

    addAndMakeVisible(latestNotificationMessage);

    addAndMakeVisible(notificationsButton);
    addAndMakeVisible(errorsButton);
    addAndMakeVisible(numberOfNotificationsLabel);
    addAndMakeVisible(numberOfErrorsLabel);

    numberOfErrorsLabel.setColour(juce::Label::textColourId, UIColours::textEditor);
    numberOfNotificationsLabel.setColour(juce::Label::textColourId, UIColours::textEditor);

    notificationsButton.onClick = errorsButton.onClick = [&]
    {
        DialogLauncher::launchDialog(std::make_unique<NotificationsDialog>(notificationMessages), [this]
        {
            for (auto& notificationMessage : notificationMessages)
            {
                notificationMessage.isUnread = false;
            }
            notificationMessagesChanged(false);
        });

        static_cast<NotificationsDialog*>(DialogLauncher::getLaunchedDialog())->onClear = [&]
        {
            notificationMessagesChanged(false);
        };
    };

    notificationCallback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            NotificationsDialog::NotificationMessage notificationMessage;
                                            notificationMessage.isError = false;
                                            notificationMessage.timestamp = message.timestamp;
                                            notificationMessage.message = message.char_array;
                                            notificationMessages.push_back(notificationMessage);

                                            notificationMessagesChanged(true);
                                        });
    };
    notificationCallbackID = connection.addNotificationCallback(notificationCallback);

    errorCallback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            NotificationsDialog::NotificationMessage notificationMessage;
                                            notificationMessage.isError = true;
                                            notificationMessage.timestamp = message.timestamp;
                                            notificationMessage.message = message.char_array;
                                            notificationMessages.push_back(notificationMessage);

                                            notificationMessagesChanged(true);
                                        });
    };
    errorCallbackID = connection.addErrorCallback(errorCallback);
}

DevicePanelFooter::~DevicePanelFooter()
{
    connection.removeCallback(notificationCallbackID);
    connection.removeCallback(errorCallbackID);
    connection.removeCallback(statisticsCallbackID);
}

void DevicePanelFooter::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::footer);
}

void DevicePanelFooter::resized()
{
    auto bounds = getLocalBounds().reduced(UILayout::panelMargin, 0);

    const auto iconWidth = bounds.getHeight();
    static constexpr int iconMargin = 2;
    static const int maxTextWidth = UIFonts::smallFont.getStringWidth("999");

    juce::FlexBox flexBox;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    flexBox.items.add(juce::FlexItem(numberOfNotificationsLabel).withFlex(1.0f).withMaxWidth((float) maxTextWidth));
    flexBox.items.add(juce::FlexItem(notificationsButton).withMinWidth((float) iconWidth).withMargin({ (float) iconMargin }));
    flexBox.items.add(juce::FlexItem(numberOfErrorsLabel).withFlex(1.0f).withMaxWidth((float) maxTextWidth));
    flexBox.items.add(juce::FlexItem(errorsButton).withMinWidth((float) iconWidth).withMargin({ (float) iconMargin }));

    const auto minWidth = 2 * iconWidth + 4 * iconMargin;
    const auto maxWidth = minWidth + 2 * maxTextWidth;
    const auto width = bounds.getWidth() - (int) std::ceil(statisticsLabel.getTextWidth());
    flexBox.performLayout(bounds.removeFromRight(juce::jlimit(minWidth, maxWidth, width)));

    statisticsLabel.setBounds(bounds.removeFromLeft((int) std::ceil(statisticsLabel.getTextWidth())));
    latestNotificationMessage.setBounds(bounds);
}

void DevicePanelFooter::notificationMessagesChanged(const bool showLatest)
{
    const auto getNumberOfUnreadMessages = [&](const auto isError)
    {
        int count = 0;
        for (const auto& notificationMessage : notificationMessages)
        {
            if ((notificationMessage.isError == isError) && notificationMessage.isUnread && (count < 999))
            {
                count++;
            }
        }
        return count;
    };

    const auto numberOfNotifications = getNumberOfUnreadMessages(false);
    numberOfNotificationsLabel.setText(juce::String(numberOfNotifications));
    notificationsButton.setToggleState(numberOfNotifications > 0, juce::dontSendNotification);

    const auto numberOfErrors = getNumberOfUnreadMessages(true);
    numberOfErrorsLabel.setText(juce::String(getNumberOfUnreadMessages(true)));
    errorsButton.setToggleState(numberOfErrors > 0, juce::dontSendNotification);

    if (showLatest)
    {
        latestNotificationMessage.setText(notificationMessages.back().message);
        latestNotificationMessage.setColour(juce::Label::textColourId, notificationMessages.back().isError ? UIColours::warning : juce::Colours::white);
        startTimer(5000);
    }

    if (auto* dialog = dynamic_cast<NotificationsDialog*>(DialogLauncher::getLaunchedDialog()))
    {
        dialog->notificationMessagesChanged();
    }
}

void DevicePanelFooter::timerCallback()
{
    latestNotificationMessage.setText("");
    stopTimer();
}
