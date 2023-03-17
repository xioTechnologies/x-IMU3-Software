#include "ApplicationSettings.h"
#include "CustomLookAndFeel.h"
#include "DevicePanelFooter.h"
#include "DevicePanelHeader.h"

DevicePanelFooter::DevicePanelFooter(ximu3::Connection& connection_) : connection(connection_)
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

                                            std::ostringstream text;
                                            text << " " << message.message_rate << " messages/s";
                                            text << " (" << std::fixed << std::setprecision(2) << message.data_rate / 1000.0f << " kB/s) ";
                                            statisticsLabel.setText(text.str());
                                            resized();
                                        });
    };
    statisticsCallbackID = connection.addStatisticsCallback(statisticsCallback);

    addAndMakeVisible(latestMessageLabel);

    addAndMakeVisible(notificationsButton);
    addAndMakeVisible(errorsButton);
    addAndMakeVisible(numberOfNotificationsLabel);
    addAndMakeVisible(numberOfErrorsLabel);

    numberOfErrorsLabel.setColour(juce::Label::textColourId, UIColours::textEditor);
    numberOfNotificationsLabel.setColour(juce::Label::textColourId, UIColours::textEditor);

    notificationsButton.onClick = errorsButton.onClick = [&]
    {
        DialogLauncher::launchDialog(std::make_unique<NotificationAndErrorMessagesDialog>(messages, [&]
        {
            messagesChanged();
        }), [this]
                                     {
                                         for (auto& notificationMessage : messages)
                                         {
                                             notificationMessage.isUnread = false;
                                         }
                                         messagesChanged();
                                         return true;
                                     });
    };

    notificationCallback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            messages.push_back({ false, message.timestamp, message.char_array });

                                            messagesChanged();
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

                                            messages.push_back({ true, message.timestamp, message.char_array });

                                            messagesChanged();
                                        });
    };
    errorCallbackID = connection.addErrorCallback(errorCallback);
}

DevicePanelFooter::~DevicePanelFooter()
{
    connection.removeCallback(statisticsCallbackID);
    connection.removeCallback(notificationCallbackID);
    connection.removeCallback(errorCallbackID);
}

void DevicePanelFooter::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundDarkest);
}

void DevicePanelFooter::resized()
{
    auto bounds = getLocalBounds().reduced(UILayout::panelMargin, 0);

    const auto iconWidth = bounds.getHeight();
    static constexpr int iconMargin = 2;
    static const int maxTextWidth = UIFonts::getSmallFont().getStringWidth("000");

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
    latestMessageLabel.setBounds(bounds);
}

void DevicePanelFooter::messagesChanged()
{
    const auto getCountText = [&](const auto isError)
    {
        int count = 0;
        for (const auto& message : messages)
        {
            if ((message.isError == isError) && message.isUnread && (++count >= 100))
            {
                return juce::String("99+");
            }
        }
        return juce::String(count);
    };

    numberOfNotificationsLabel.setText(getCountText(false));
    notificationsButton.setToggleState(numberOfNotificationsLabel.getText() != "0", juce::dontSendNotification);

    numberOfErrorsLabel.setText(getCountText(true));
    errorsButton.setToggleState(numberOfErrorsLabel.getText() != "0", juce::dontSendNotification);

    if (messages.empty() == false && messages.back().isUnread)
    {
        latestMessageLabel.setText(messages.back().message);
        latestMessageLabel.setColour(juce::Label::textColourId, messages.back().isError ? UIColours::warning : juce::Colours::white);
    }
    else
    {
        latestMessageLabel.setText("");
    }

    if (auto* const dialog = dynamic_cast<NotificationAndErrorMessagesDialog*>(DialogLauncher::getLaunchedDialog()))
    {
        dialog->messagesChanged();
    }
}
