#include "ApplicationSettings.h"
#include "CustomLookAndFeel.h"
#include "DevicePanelFooter.h"
#include "DevicePanelHeader.h"
#include <juce_gui_basics/juce_gui_basics.h>

DevicePanelFooter::DevicePanelFooter(Notifications& notificationsPopup_, ximu3::Connection& connection_) : notificationsPopup(notificationsPopup_),
                                                                                                           connection(connection_)
{
    addAndMakeVisible(notificationsButton);
    addAndMakeVisible(errorsButton);
    addAndMakeVisible(numberOfNotificationsLabel);
    addAndMakeVisible(numberOfErrorsLabel);

    const auto initialiseColours = [](auto& label)
    {
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::textColourId, UIColours::textEditor);
    };
    initialiseColours(numberOfErrorsLabel);
    initialiseColours(numberOfNotificationsLabel);

    notificationsButton.onClick = errorsButton.onClick = [&]
    {
        notificationsPopup.show(true);
    };

    notificationCallback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            notificationsPopup.getFeed().add(message.timestamp, juce::String(message.char_array), juce::Colours::white);
                                            if (ApplicationSettings::getSingleton().showNotificationAndErrorMessages)
                                            {
                                                notificationsPopup.show(true);
                                            }

                                            const auto numberOfNotifications = numberOfNotificationsLabel.getText().getIntValue();
                                            if (numberOfNotifications < 999)
                                            {
                                                setNumberOfNotifications(numberOfNotifications + 1);
                                            }
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

                                            notificationsPopup.getFeed().add(message.timestamp, juce::String(message.char_array), UIColours::warning);
                                            if (ApplicationSettings::getSingleton().showNotificationAndErrorMessages)
                                            {
                                                notificationsPopup.show(true);
                                            }

                                            const auto numberOfErrors = numberOfErrorsLabel.getText().getIntValue();
                                            if (numberOfErrors < 999)
                                            {
                                                setNumberOfErrors(numberOfErrors + 1);
                                            }
                                        });
    };
    errorCallbackID = connection.addErrorCallback(errorCallback);

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

    notificationsPopup.getFeed().onClear = [&]
    {
        setNumberOfNotifications(0);
        setNumberOfErrors(0);
    };

    addAndMakeVisible(statisticsLabel);
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
    flexBox.items.add(juce::FlexItem(notificationsButton).withMinWidth((float) iconWidth).withMaxWidth((float) iconWidth).withMargin({ (float) iconMargin }));
    flexBox.items.add(juce::FlexItem(numberOfErrorsLabel).withFlex(1.0f).withMaxWidth((float) maxTextWidth));
    flexBox.items.add(juce::FlexItem(errorsButton).withMinWidth((float) iconWidth).withMaxWidth((float) iconWidth).withMargin({ (float) iconMargin }));

    const auto minWidth = 2 * iconWidth + 4 * iconMargin;
    const auto maxWidth = minWidth + 2 * maxTextWidth;
    const auto width = bounds.getWidth() - (int) std::ceil(statisticsLabel.getTextWidth()) - DevicePanelHeader::margin;
    flexBox.performLayout(bounds.removeFromRight(juce::jlimit(minWidth, maxWidth, width)));

    statisticsLabel.setBounds(bounds.withTrimmedRight(DevicePanelHeader::margin));
}

void DevicePanelFooter::setNumberOfNotifications(const int number)
{
    numberOfNotificationsLabel.setText(juce::String(number));
    notificationsButton.setToggleState(number > 0, juce::dontSendNotification);
}

void DevicePanelFooter::setNumberOfErrors(const int number)
{
    numberOfErrorsLabel.setText(juce::String(number));
    errorsButton.setToggleState(number > 0, juce::dontSendNotification);
}
