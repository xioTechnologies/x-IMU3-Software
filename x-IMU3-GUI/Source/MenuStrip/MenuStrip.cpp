#include "ApplicationSettings.h"
#include "CustomLayouts.h"
#include "CustomLookAndFeel.h"
#include "DefaultLayout.h"
#include "DevicePanelContainer.h"
#include "Dialogs/AboutDialog.h"
#include "Dialogs/ApplicationErrorsDialog.h"
#include "Dialogs/ApplicationSettingsDialog.h"
#include "Dialogs/AreYouSureDialog.h"
#include "Dialogs/ErrorDialog.h"
#include "Dialogs/FileConverterDialog.h"
#include "Dialogs/FileConverterProgressDialog.h"
#include "Dialogs/NewConnectionDialog.h"
#include "Dialogs/SaveWindowLayoutDialog.h"
#include "Dialogs/SearchForConnectionsDialog.h"
#include "Dialogs/SendCommandDialog.h"
#include "MenuStrip.h"
#include "RecentConnections.h"
#include "Widgets/PopupMenuHeader.h"
#include "Windows/WindowIDs.h"

MenuStrip::MenuStrip(juce::ValueTree& windowLayout_, DevicePanelContainer& devicePanelsContainer_) : windowLayout(windowLayout_),
                                                                                                     devicePanelsContainer(devicePanelsContainer_)
{
    setWindowLayout({});

    for (auto& buttonGroup : buttonGroups)
    {
        addAndMakeVisible(buttonGroup.label);
        for (auto& button : buttonGroup.buttons)
        {
            addAndMakeVisible(button.get());
        }
    }

    searchButton.onClick = [this]
    {
        std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections;

        for (auto& devicePanel : devicePanelsContainer.getDevicePanels())
        {
            existingConnections.push_back(devicePanel->getConnection().getInfo());
        }

        DialogLauncher::launchDialog(std::make_unique<SearchForConnectionsDialog>(std::move(existingConnections)), [this]
        {
            if (auto* dialog = dynamic_cast<SearchForConnectionsDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                for (const auto& connectionInfo : dialog->getConnectionInfos())
                {
                    devicePanelsContainer.connectToDevice(*connectionInfo);
                }
            }
        });
    };

    if (ApplicationSettings::getSingleton().searchForConnectionsOnStartup)
    {
        searchButton.triggerClick();
    }

    shutdownButton.onClick = [this]
    {
        DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("Are you sure you want to shutdown all devices?"), [this]
        {
            devicePanelsContainer.sendCommands({{ "shutdown", {}}});
        });
    };

    sendCommandButton.onClick = [this]
    {
        DialogLauncher::launchDialog(std::make_unique<SendCommandDialog>("Send Command To All Devices"), [this]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                devicePanelsContainer.sendCommands({ CommandMessage(dialog->getCommand()) });
            }
        });
    };

    juce::File(dataLoggerSettings.directory).createDirectory();

    dataLoggerStartStopButton.onClick = [&]
    {
        if (isTimerRunning())
        {
            dataLogger.reset();
            stopTimer();
            dataLoggerStartStopButton.setToggleState(false, juce::dontSendNotification);
            return;
        }

        DialogLauncher::launchDialog(std::make_unique<DataLoggerSettingsDialog>(dataLoggerSettings), [this]
        {
            if (auto* dialog = dynamic_cast<DataLoggerSettingsDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                dataLoggerSettings = dialog->getSettings();

                std::vector<ximu3::Connection*> connections;
                for (auto& devicePanel : devicePanelsContainer.getDevicePanels())
                {
                    connections.push_back(&devicePanel->getConnection());
                }

                bool failed = false;

                dataLogger = std::make_unique<ximu3::DataLogger>(dataLoggerSettings.directory.toStdString(),
                                                                 dataLoggerSettings.name.toStdString(),
                                                                 connections,
                                                                 [&](ximu3::XIMU3_Result result)
                                                                 {
                                                                     if (result == ximu3::XIMU3_ResultOk)
                                                                     {
                                                                         juce::File(dataLoggerSettings.directory).getChildFile(dataLoggerSettings.name).revealToUser();
                                                                     }
                                                                     else
                                                                     {
                                                                         failed = true; // callback will be on same thread if data logger fails
                                                                     }
                                                                 });
                if (failed == false)
                {
                    dataLoggerStartTime = juce::Time::getCurrentTime();
                    startTimerHz(25);
                    dataLoggerStartStopButton.setToggleState(true, juce::dontSendNotification);
                }
            }
        });
    };

    applicationErrorsButton.onClick = []
    {
        DialogLauncher::launchDialog(std::make_unique<ApplicationErrorsDialog>());
    };

    versionButton.onClick = []
    {
        DialogLauncher::launchDialog(std::make_unique<AboutDialog>());
    };

    mainSettingsButton.onClick = []
    {
        DialogLauncher::launchDialog(std::make_unique<ApplicationSettingsDialog>());
    };

    devicePanelsContainer.addComponentListener(this);
    componentChildrenChanged(devicePanelsContainer);

    ApplicationErrorsDialog::numberOfUnreadErrors.addListener(this);
}

MenuStrip::~MenuStrip()
{
    devicePanelsContainer.removeComponentListener(this);
    ApplicationErrorsDialog::numberOfUnreadErrors.removeListener(this);
}

void MenuStrip::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::menuStrip);
}

void MenuStrip::resized()
{
    static constexpr int buttonHeight = 24;
    static constexpr int buttonWidth = 45;
    static constexpr int buttonMargin = 3;
    static constexpr int buttonGroupMargin = 10;

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.alignContent = juce::FlexBox::AlignContent::stretch;
    flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

    for (auto& buttonGroup : buttonGroups)
    {
        int groupWidth = 0;

        auto& groupBox = buttonGroup.groupBox;
        auto& buttonBox = buttonGroup.buttonBox;

        groupBox = {};
        buttonBox = {};

        buttonBox.flexDirection = juce::FlexBox::Direction::row;
        buttonBox.alignContent = juce::FlexBox::AlignContent::stretch;
        buttonBox.alignItems = juce::FlexBox::AlignItems::stretch;
        buttonBox.justifyContent = juce::FlexBox::JustifyContent::center;

        groupBox.flexDirection = juce::FlexBox::Direction::column;
        groupBox.alignContent = juce::FlexBox::AlignContent::stretch;
        groupBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (auto& button : buttonGroup.buttons)
        {
            const auto width = buttonWidth * (&button.get() == &dataLoggerTime ? 2.489f : 1.0f);
            buttonBox.items.add(juce::FlexItem(button)
                                        .withHeight(buttonHeight)
                                        .withFlex(0, 0, width)
                                        .withMargin(buttonMargin));

            groupWidth += juce::roundToInt(width) + buttonMargin;
        }

        groupBox.items.add(juce::FlexItem(buttonBox)
                                   .withHeight(buttonHeight)
                                   .withFlex(0.5));
        groupBox.items.add(juce::FlexItem(buttonGroup.label)
                                   .withFlex(10));

        flexBox.items.add(juce::FlexItem(groupBox)
                                  .withMargin(buttonGroupMargin)
                                  .withWidth((float) groupWidth)
                                  .withFlex(1, 0));
    }

    flexBox.performLayout(getLocalBounds().toFloat());
}

juce::PopupMenu MenuStrip::getManualConnectMenu()
{
    juce::PopupMenu menu;

    const auto connectCallback = [this]
    {
        if (auto* dialog = dynamic_cast<NewConnectionDialog*>(DialogLauncher::getLaunchedDialog()))
        {
            auto connectionInfo = dialog->getConnectionInfo();
            devicePanelsContainer.connectToDevice(*connectionInfo);
            RecentConnections().update(*connectionInfo);
        }
    };
    menu.addItem("New USB Connection", [connectCallback]
    {
        DialogLauncher::launchDialog(std::make_unique<UsbConnectionDialog>(), connectCallback);
    });
    menu.addItem("New Serial Connection", [connectCallback]
    {
        DialogLauncher::launchDialog(std::make_unique<SerialConnectionDialog>(), connectCallback);
    });
    menu.addItem("New TCP Connection", [connectCallback]
    {
        DialogLauncher::launchDialog(std::make_unique<TcpConnectionDialog>(), connectCallback);
    });
    menu.addItem("New UDP Connection", [connectCallback]
    {
        DialogLauncher::launchDialog(std::make_unique<UdpConnectionDialog>(), connectCallback);
    });
    menu.addItem("New Bluetooth Connection", [connectCallback]
    {
        DialogLauncher::launchDialog(std::make_unique<BluetoothConnectionDialog>(), connectCallback);
    });
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("RECENT CONNECTIONS"), nullptr);

    for (auto& connectionInfo : RecentConnections().get())
    {
        const auto connectionInfoString = connectionInfo->toString();
        menu.addItem(connectionInfoString, [this, connectionInfo = std::shared_ptr<ximu3::ConnectionInfo>(connectionInfo.release())]
        {
            devicePanelsContainer.connectToDevice(*connectionInfo);
        });
    }

    return menu;
}

juce::PopupMenu MenuStrip::getDisconnectMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Disconnect All", [this]
    {
        dataLoggerTime.setTime(juce::RelativeTime());
        dataLoggerStartStopButton.setToggleState(false, juce::sendNotificationSync);
        devicePanelsContainer.removeAllPanels();
    });
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("INDIVIDUAL"), nullptr);
    for (auto& panel : devicePanelsContainer.getDevicePanels())
    {
        auto deviceNameAndSerialNumber = panel->getDeviceNameAndSerialNumber();
        if (deviceNameAndSerialNumber.isNotEmpty())
        {
            deviceNameAndSerialNumber += "   ";
        }
        juce::PopupMenu::Item item(deviceNameAndSerialNumber + panel->getConnection().getInfo()->toString());

        item.action = [this, panel = panel.get()]
        {
            dataLoggerTime.setTime(juce::RelativeTime());
            dataLoggerStartStopButton.setToggleState(false, juce::sendNotificationSync);
            devicePanelsContainer.removePanel(*panel);
        };

        auto colourTag = std::make_unique<juce::DrawableRectangle>();
        int _, height;
        getLookAndFeel().getIdealPopupMenuItemSize({}, false, {}, _, height);
        colourTag->setRectangle(juce::Rectangle<float>(0.0f, 0.0f, (float) DevicePanelHeader::colourTagWidth, (float) height));
        colourTag->setFill({ panel->getColourTag() });
        item.image = std::move(colourTag);

        menu.addItem(item);
    }

    return menu;
}

juce::PopupMenu MenuStrip::getWindowMenu() const
{
    juce::PopupMenu menu;

    const auto addWindowItem = [&](const auto& id)
    {
        const auto toggled = Window::findWindow(windowLayout, id).isValid();
        menu.addItem(getWindowTitle(id), true, toggled, [this, id = id, toggled]
        {
            if (toggled)
            {
                for (auto child = Window::findWindow(windowLayout, id); child.isValid() && child.getNumChildren() == 0;)
                {
                    auto parent = child.getParent();
                    parent.removeChild(child, nullptr);
                    child = parent;
                }
                return;
            }

            auto totalWindowSizes = 0.0f;
            for (const auto window : windowLayout.getRoot())
            {
                totalWindowSizes += (float) window.getProperty(WindowIDs::size, 1.0f);
            }

            const auto newSize = (totalWindowSizes == 0.0f) ? 1.0f : (totalWindowSizes / (float) windowLayout.getRoot().getNumChildren());
            windowLayout.getRoot().appendChild({ id, {{ WindowIDs::size, newSize }}}, nullptr);
        });
    };
    addWindowItem(WindowIDs::DeviceSettings);
    addWindowItem(WindowIDs::ThreeDView);
    addWindowItem(WindowIDs::SerialAccessoryTerminal);
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("GRAPHS"), nullptr);
    addWindowItem(WindowIDs::Gyroscope);
    addWindowItem(WindowIDs::Accelerometer);
    addWindowItem(WindowIDs::Magnetometer);
    addWindowItem(WindowIDs::EulerAngles);
    addWindowItem(WindowIDs::LinearAcceleration);
    addWindowItem(WindowIDs::EarthAcceleration);
    addWindowItem(WindowIDs::HighGAccelerometer);
    addWindowItem(WindowIDs::Temperature);
    addWindowItem(WindowIDs::BatteryPercentage);
    addWindowItem(WindowIDs::BatteryVoltage);
    addWindowItem(WindowIDs::RssiPercentage);
    addWindowItem(WindowIDs::RssiPower);
    addWindowItem(WindowIDs::ReceivedMessageRate);
    addWindowItem(WindowIDs::ReceivedDataRate);

    return menu;
}

juce::PopupMenu MenuStrip::getWindowLayoutMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Default", [this]
    {
        setWindowLayout({});
    });
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SINGLE WINDOW"), nullptr);

    const auto addWindowItem = [this](const auto& id, auto& popupMenu)
    {
        popupMenu.addItem(getWindowTitle(id), [this, id = id]
        {
            windowLayout.removeAllChildren(nullptr);
            juce::ValueTree row(WindowIDs::Row);
            juce::ValueTree window(id);
            window.setProperty(WindowIDs::size, 0.5f, nullptr);
            row.appendChild(window, nullptr);
            windowLayout.appendChild(row, nullptr);
        });
    };
    addWindowItem(WindowIDs::DeviceSettings, menu);
    addWindowItem(WindowIDs::ThreeDView, menu);
    addWindowItem(WindowIDs::SerialAccessoryTerminal, menu);
    juce::PopupMenu graphs;
    addWindowItem(WindowIDs::Gyroscope, graphs);
    addWindowItem(WindowIDs::Accelerometer, graphs);
    addWindowItem(WindowIDs::Magnetometer, graphs);
    addWindowItem(WindowIDs::EulerAngles, graphs);
    addWindowItem(WindowIDs::LinearAcceleration, graphs);
    addWindowItem(WindowIDs::EarthAcceleration, graphs);
    addWindowItem(WindowIDs::HighGAccelerometer, graphs);
    addWindowItem(WindowIDs::Temperature, graphs);
    addWindowItem(WindowIDs::BatteryPercentage, graphs);
    addWindowItem(WindowIDs::BatteryVoltage, graphs);
    addWindowItem(WindowIDs::RssiPercentage, graphs);
    addWindowItem(WindowIDs::RssiPower, graphs);
    addWindowItem(WindowIDs::ReceivedMessageRate, graphs);
    addWindowItem(WindowIDs::ReceivedDataRate, graphs);
    menu.addSubMenu("Graph", graphs);

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("CUSTOM LAYOUTS"), nullptr);
    menu.addItem("Save As...", [this]
    {
        DialogLauncher::launchDialog(std::make_unique<SaveWindowLayoutDialog>(), [this]
        {
            if (auto* saveWindowLayoutDialog = dynamic_cast<SaveWindowLayoutDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                const auto layoutName = saveWindowLayoutDialog->getLayoutName();

                const auto save = [this, layoutName]
                {
                    CustomLayouts().save(layoutName, windowLayout);
                };

                if (CustomLayouts().exists(layoutName))
                {
                    DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("This name already exists. Do you want to overwrite this layout?"), save);
                }
                else
                {
                    save();
                }
            }
        });
    });

    for (auto layout : CustomLayouts().load())
    {
        menu.addItem(layout.first, [&, layout]
        {
            setWindowLayout(layout.second);
        });
    }

    return menu;
}

juce::PopupMenu MenuStrip::getPanelLayoutMenu()
{
    juce::PopupMenu menu;

    const auto addItem = [&](auto type, const auto& title)
    {
        menu.addItem(title, true, devicePanelsContainer.getLayout() == type, [&, type]
        {
            devicePanelsContainer.setLayout(type);
            devicePanelLayoutButton.setIcon(layoutIcons.at(type), {});
            setWindowLayout({});
        });
    };
    addItem(DevicePanelContainer::Layout::rows, "Rows");
    addItem(DevicePanelContainer::Layout::columns, "Columns");
    addItem(DevicePanelContainer::Layout::grid, "Grid");
    addItem(DevicePanelContainer::Layout::accordion, "Accordion");

    return menu;
}

juce::PopupMenu MenuStrip::getToolsMenu() const
{
    juce::PopupMenu menu;
    menu.addItem("File Converter", []
    {
        DialogLauncher::launchDialog(std::make_unique<FileConverterDialog>(), []
        {
            if (const auto* const fileConverterDialog = dynamic_cast<FileConverterDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                DialogLauncher::launchDialog(std::make_unique<FileConverterProgressDialog>(fileConverterDialog->getDestination(), fileConverterDialog->getSource()));
            }
        });
    });
    return menu;
}

void MenuStrip::setWindowLayout(juce::ValueTree windowLayout_)
{
    if (windowLayout_.isValid() == false)
    {
        if (devicePanelsContainer.getDevicePanels().size() == 1)
        {
            windowLayout_ = DefaultLayout::single;
        }
        else
        {
            switch (devicePanelsContainer.getLayout())
            {
                case DevicePanelContainer::Layout::rows:
                    windowLayout_ = DefaultLayout::rows;
                    break;

                case DevicePanelContainer::Layout::columns:
                    windowLayout_ = DefaultLayout::columns;
                    break;

                case DevicePanelContainer::Layout::grid:
                    windowLayout_ = DefaultLayout::grid;
                    break;

                case DevicePanelContainer::Layout::accordion:
                    windowLayout_ = DefaultLayout::accordion;
                    break;
            }
        }
    }

    windowLayout.copyPropertiesAndChildrenFrom(windowLayout_, nullptr);
}

void MenuStrip::componentChildrenChanged(juce::Component&)
{
    const auto devicePanelsSize = devicePanelsContainer.getDevicePanels().size();

    devicePanelLayoutButton.setEnabled(devicePanelsSize > 1);

    for (auto& component : std::vector<std::reference_wrapper<juce::Component>>({ disconnectButton, showHideWindowButton, windowLayoutButton,
                                                                                  shutdownButton, sendCommandButton, dataLoggerStartStopButton, dataLoggerTime }))
    {
        component.get().setEnabled(devicePanelsSize > 0);
    }

    const auto layout = devicePanelsSize <= 2 ? DevicePanelContainer::Layout::rows :
                        devicePanelsSize == 3 ? DevicePanelContainer::Layout::columns :
                        devicePanelsSize == 4 ? DevicePanelContainer::Layout::grid :
                        DevicePanelContainer::Layout::accordion;
    devicePanelsContainer.setLayout(layout);
    devicePanelLayoutButton.setIcon(layoutIcons.at(layout), {});
    setWindowLayout({});
}

void MenuStrip::valueChanged(juce::Value& value)
{
    applicationErrorsButton.setToggleState((int) (juce::var) value > 0, juce::dontSendNotification);
}

void MenuStrip::timerCallback()
{
    const auto relativeTime = juce::Time::getCurrentTime() - dataLoggerStartTime;
    if (dataLoggerSettings.unlimited == false && relativeTime.inSeconds() >= dataLoggerSettings.seconds)
    {
        dataLoggerStartStopButton.setToggleState(false, juce::sendNotificationSync);
        dataLoggerTime.setTime(juce::RelativeTime::seconds(dataLoggerSettings.seconds));
    }
    else
    {
        dataLoggerTime.setTime(relativeTime);
    }
}
