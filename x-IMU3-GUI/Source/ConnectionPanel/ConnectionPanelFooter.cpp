#include "ApplicationSettings.h"
#include "ConnectionPanel.h"
#include "ConnectionPanelFooter.h"
#include "CustomLookAndFeel.h"

ConnectionPanelFooter::ConnectionPanelFooter(ConnectionPanel& connectionPanel_) : connectionPanel(connectionPanel_)
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
    statisticsCallbackID = connectionPanel.getConnection()->addStatisticsCallback(statisticsCallback);

    addAndMakeVisible(latestMessageLabel);

    addAndMakeVisible(notificationsButton);
    addAndMakeVisible(errorsButton);
    addAndMakeVisible(numberOfNotificationsLabel);
    addAndMakeVisible(numberOfErrorsLabel);

    notificationsButton.onClick = errorsButton.onClick = [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<NotificationsAndErrorsDialog>(messages, [&]
        {
            messagesChanged();
        }, connectionPanel), [this]
                                              {
                                                  for (auto& notificationMessage : messages)
                                                  {
                                                      notificationMessage.unread = false;
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

                                            messages.push_back({ NotificationsAndErrorsDialog::Message::Type::notification, message.timestamp, juce::String::createStringFromData(message.char_array, (int) message.number_of_bytes) });

                                            messagesChanged();
                                        });
    };
    notificationCallbackID = connectionPanel.getConnection()->addNotificationCallback(notificationCallback);

    errorCallback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            messages.push_back({ NotificationsAndErrorsDialog::Message::Type::error, message.timestamp, juce::String::createStringFromData(message.char_array, (int) message.number_of_bytes) });

                                            messagesChanged();
                                        });
    };
    errorCallbackID = connectionPanel.getConnection()->addErrorCallback(errorCallback);
}

ConnectionPanelFooter::~ConnectionPanelFooter()
{
    connectionPanel.getConnection()->removeCallback(statisticsCallbackID);
    connectionPanel.getConnection()->removeCallback(notificationCallbackID);
    connectionPanel.getConnection()->removeCallback(errorCallbackID);
}

void ConnectionPanelFooter::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundDarkest);
}

void ConnectionPanelFooter::resized()
{
    auto bounds = getLocalBounds().reduced(UILayout::panelMargin, 0);

    const auto iconWidth = bounds.getHeight();
    static constexpr int iconMargin = 2;

    const auto numberOfNotificationsMaxWidth = 5 + numberOfNotificationsLabel.getText().length() * UIFonts::getSmallFont().getStringWidth("0");
    const auto numberOfErrorsMaxWidth = 5 + numberOfErrorsLabel.getText().length() * UIFonts::getSmallFont().getStringWidth("0");

    juce::FlexBox flexBox;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    flexBox.items.add(juce::FlexItem(numberOfNotificationsLabel).withFlex(1.0f).withMaxWidth((float) numberOfNotificationsMaxWidth));
    flexBox.items.add(juce::FlexItem(notificationsButton).withMinWidth((float) iconWidth).withMargin({ (float) iconMargin }));
    flexBox.items.add(juce::FlexItem(numberOfErrorsLabel).withFlex(1.0f).withMaxWidth((float) numberOfErrorsMaxWidth));
    flexBox.items.add(juce::FlexItem(errorsButton).withMinWidth((float) iconWidth).withMargin({ (float) iconMargin }));

    const auto minWidth = 2 * iconWidth + 4 * iconMargin;
    const auto maxWidth = minWidth + numberOfNotificationsMaxWidth + numberOfErrorsMaxWidth;
    const auto width = bounds.getWidth() - (int) std::ceil(statisticsLabel.getTextWidth());
    flexBox.performLayout(bounds.removeFromRight(juce::jlimit(minWidth, maxWidth, width)));

    statisticsLabel.setBounds(bounds.removeFromLeft((int) std::ceil(statisticsLabel.getTextWidth())));
    latestMessageLabel.setBounds(bounds);
}

void ConnectionPanelFooter::messagesChanged()
{
    const auto getCountText = [&](const auto type)
    {
        int count = 0;
        for (const auto& message : messages)
        {
            if ((message.type == type) && message.unread && (++count >= 100))
            {
                return juce::String("99+");
            }
        }
        return juce::String(count);
    };

    numberOfNotificationsLabel.setText(getCountText(NotificationsAndErrorsDialog::Message::Type::notification));
    notificationsButton.setToggleState(numberOfNotificationsLabel.getText() != "0", juce::dontSendNotification);

    numberOfErrorsLabel.setText(getCountText(NotificationsAndErrorsDialog::Message::Type::error));
    errorsButton.setToggleState(numberOfErrorsLabel.getText() != "0", juce::dontSendNotification);

    if (messages.empty() == false && messages.back().unread)
    {
        latestMessageLabel.setText(messages.back().message);
        latestMessageLabel.setColour(juce::Label::textColourId, messages.back().getColour());
        startTimer(5000);
    }
    else
    {
        timerCallback();
    }

    if (auto* const dialog = dynamic_cast<NotificationsAndErrorsDialog*>(DialogQueue::getSingleton().getActive()))
    {
        dialog->messagesChanged();
    }

    resized();
}

void ConnectionPanelFooter::timerCallback()
{
    latestMessageLabel.setText("");
    stopTimer();
}
