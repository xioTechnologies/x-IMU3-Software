#include "ApplicationSettings.h"
#include "ConnectionHistory.h"
#include "CustomLayouts.h"
#include "CustomLookAndFeel.h"
#include "DevicePanelContainer.h"
#include "Dialogs/AboutDialog.h"
#include "Dialogs/ApplicationSettingsDialog.h"
#include "Dialogs/AreYouSureDialog.h"
#include "Dialogs/ConvertFileDialog.h"
#include "Dialogs/ConvertingFileDialog.h"
#include "Dialogs/ErrorDialog.h"
#include "Dialogs/NewConnectionDialog.h"
#include "Dialogs/SaveWindowLayoutDialog.h"
#include "Dialogs/SearchForConnectionsDialog.h"
#include "Dialogs/SendCommandDialog.h"
#include "Dialogs/SendingCommandDialog.h"
#include "Dialogs/UpdateFirmwareDialog.h"
#include "Dialogs/UpdatingFirmwareDialog.h"
#include "MenuStrip.h"
#include "Widgets/PopupMenuHeader.h"
#include "Windows/WindowIDs.h"

MenuStrip::MenuStrip(juce::ValueTree& windowLayout_, DevicePanelContainer& devicePanelContainer_) : windowLayout(windowLayout_),
                                                                                                    devicePanelContainer(devicePanelContainer_)
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

        for (auto* const devicePanel : devicePanelContainer.getDevicePanels())
        {
            existingConnections.push_back(devicePanel->getConnection().getInfo());
        }

        DialogLauncher::launchDialog(std::make_unique<SearchForConnectionsDialog>(std::move(existingConnections)), [this]
        {
            if (auto* dialog = dynamic_cast<SearchForConnectionsDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                for (const auto& connectionInfo : dialog->getConnectionInfos())
                {
                    devicePanelContainer.connectToDevice(*connectionInfo);
                }
            }
            return true;
        });
    };

    if (ApplicationSettings::getSingleton().showSearchForConnectionsOnStartup)
    {
        searchButton.triggerClick();
    }

    shutdownButton.onClick = [this]
    {
        DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("Are you sure you want to shutdown all devices?"), [this]
        {
            DialogLauncher::launchDialog(std::make_unique<SendingCommandDialog>(CommandMessage("shutdown", {}), devicePanelContainer.getDevicePanels()));
            return true;
        });
    };

    sendCommandButton.onClick = [this]
    {
        DialogLauncher::launchDialog(std::make_unique<SendCommandDialog>("Send Command to All Devices"), [this]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                DialogLauncher::launchDialog(std::make_unique<SendingCommandDialog>(dialog->getCommand(), devicePanelContainer.getDevicePanels()));
            }
            return true;
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

                auto name = dataLoggerSettings.name;
                if (dataLoggerSettings.appendDateAndTime)
                {
                    name += juce::Time::getCurrentTime().formatted(" %Y-%m-%d %H-%M-%S");
                }

                const auto startDataLogger = [&, name]
                {
                    std::vector<ximu3::Connection*> connections;
                    for (auto* const devicePanel : devicePanelContainer.getDevicePanels())
                    {
                        connections.push_back(&devicePanel->getConnection());
                    }

                    bool failed = false;

                    dataLogger = std::make_unique<ximu3::DataLogger>(dataLoggerSettings.directory.toStdString(),
                                                                     name.toStdString(),
                                                                     connections,
                                                                     [&, name](ximu3::XIMU3_Result result)
                                                                     {
                                                                         if (result == ximu3::XIMU3_ResultOk)
                                                                         {
                                                                             juce::File(dataLoggerSettings.directory).getChildFile(name).revealToUser();
                                                                         }
                                                                         else
                                                                         {
                                                                             failed = true; // callback will be on same thread if data logger fails
                                                                         }
                                                                     });

                    if (failed)
                    {
                        DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Data logger failed."));
                        return;
                    }

                    dataLoggerStartTime = juce::Time::getCurrentTime();
                    startTimerHz(25);
                    dataLoggerStartStopButton.setToggleState(true, juce::dontSendNotification);
                };

                const auto directory = juce::File(dataLoggerSettings.directory).getChildFile(name);
                if (directory.exists())
                {
                    DialogLauncher::launchDialog(std::make_unique<DoYouWantToReplaceItDialog>(name), [directory, startDataLogger]
                    {
                        directory.deleteRecursively();
                        startDataLogger();
                        return true;
                    });
                }
                else
                {
                    startDataLogger();
                }
            }
            return true;
        });
    };

    mainSettingsButton.onClick = []
    {
        DialogLauncher::launchDialog(std::make_unique<ApplicationSettingsDialog>());
    };

    versionButton.onClick = []
    {
        DialogLauncher::launchDialog(std::make_unique<AboutDialog>());
    };
    versionButton.setColour(juce::TextButton::buttonColourId, {});
    versionButton.setColour(juce::TextButton::buttonOnColourId, {});

    devicePanelContainer.onDevicePanelsSizeChanged = [&]
    {
        for (auto& component : std::vector<std::reference_wrapper<juce::Component>>({ disconnectButton, showHideWindowButton, windowLayoutButton, devicePanelLayoutButton,
                                                                                      shutdownButton, sendCommandButton, dataLoggerStartStopButton, dataLoggerTime, }))
        {
            component.get().setEnabled(devicePanelContainer.getDevicePanels().size() > 0);
        }

        if (devicePanelContainer.getDevicePanels().size() <= 1)
        {
            devicePanelContainer.setLayout(DevicePanelContainer::Layout::single);
            devicePanelLayoutButton.setIcon(layoutIcons.at(DevicePanelContainer::Layout::single), {});
        }
        else
        {
            if (preferredMultipleDevicePanelLayout.has_value() == false)
            {
                preferredMultipleDevicePanelLayout = DevicePanelContainer::Layout::grid;

                windowLayout.removeAllChildren(nullptr);
                windowLayout.appendChild({ WindowIDs::Row, {},
                                           {
                                                   { WindowIDs::ThreeDView, {{ WindowIDs::size, 1 }}},
                                           }}, nullptr);
            }

            devicePanelContainer.setLayout(*preferredMultipleDevicePanelLayout);
            devicePanelLayoutButton.setIcon(layoutIcons.at(*preferredMultipleDevicePanelLayout), {});
        }
    };
    devicePanelContainer.onDevicePanelsSizeChanged();

    static constexpr int buttonHeight = 22;

    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.alignContent = juce::FlexBox::AlignContent::stretch;
    flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

    for (auto& buttonGroup : buttonGroups)
    {
        auto& groupBox = buttonGroup.groupBox;
        auto& buttonBox = buttonGroup.buttonBox;

        groupBox.flexDirection = juce::FlexBox::Direction::column;
        buttonBox.justifyContent = juce::FlexBox::JustifyContent::center;

        groupBox.items.add(juce::FlexItem(buttonBox)
                                   .withHeight(buttonHeight)
                                   .withFlex(0.0f));
        groupBox.items.add(juce::FlexItem(buttonGroup.label)
                                   .withFlex(0.5f));

        flexBox.items.add(juce::FlexItem(groupBox)
                                  .withMargin(8)
                                  .withFlex(1.0f, 0));

        for (auto& button : buttonGroup.buttons)
        {
            const auto buttonWidth = [&]
            {
                if (&button.get() == &dataLoggerTime)
                {
                    return 112;
                }
                if (&button.get() == &versionButton)
                {
                    return versionButton.getBestWidthForHeight(buttonHeight);
                }
                return 35;
            }();

            buttonBox.items.add(juce::FlexItem(button)
                                        .withWidth((float) buttonWidth)
                                        .withHeight(buttonHeight));

            flexBox.items.getReference(flexBox.items.size() - 1).width += buttonWidth;
        }
    }
}

void MenuStrip::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundLightest);
}

void MenuStrip::resized()
{
    flexBox.performLayout(getLocalBounds().toFloat());
}

void MenuStrip::disconnect(const DevicePanel* const devicePanel)
{
    dataLoggerTime.setTime(juce::RelativeTime());
    dataLoggerStartStopButton.setToggleState(false, juce::sendNotificationSync);

    if (devicePanel != nullptr)
    {
        devicePanelContainer.removePanel(*devicePanel);
    }
    else
    {
        devicePanelContainer.removeAllPanels();
    }
}

juce::PopupMenu MenuStrip::getManualConnectMenu()
{
    juce::PopupMenu menu;

    const auto connectCallback = [this]
    {
        if (auto* dialog = dynamic_cast<NewConnectionDialog*>(DialogLauncher::getLaunchedDialog()))
        {
            auto connectionInfo = dialog->getConnectionInfo();
            devicePanelContainer.connectToDevice(*connectionInfo);
            ConnectionHistory().update(*connectionInfo);
        }
        return true;
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
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("CONNECTION HISTORY"), nullptr);

    for (auto& connectionInfo : ConnectionHistory().get())
    {
        const auto connectionInfoString = connectionInfo->toString();
        menu.addItem(connectionInfoString, [this, connectionInfo = std::shared_ptr<ximu3::ConnectionInfo>(connectionInfo.release())]
        {
            devicePanelContainer.connectToDevice(*connectionInfo);
        });
    }

    return menu;
}

juce::PopupMenu MenuStrip::getDisconnectMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Disconnect All", [this]
    {
        disconnect(nullptr);
    });
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("INDIVIDUAL"), nullptr);
    for (auto* const devicePanel : devicePanelContainer.getDevicePanels())
    {
        juce::PopupMenu::Item item(devicePanel->getDeviceDescriptor() + "   " + devicePanel->getConnection().getInfo()->toString());

        item.action = [this, devicePanel]
        {
            disconnect(devicePanel);
        };

        auto colourTag = std::make_unique<juce::DrawableRectangle>();
        int _, height;
        getLookAndFeel().getIdealPopupMenuItemSize({}, false, {}, _, height);
        colourTag->setRectangle(juce::Rectangle<float>(0.0f, 0.0f, (float) DevicePanelHeader::colourTagWidth, (float) height));
        colourTag->setFill({ devicePanel->getColourTag() });
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

    menu.addItem("Default", devicePanelContainer.getDevicePanels().size() == 1, false, [this]
    {
        setWindowLayout({});
        preferredMultipleDevicePanelLayout = {};
    });

    juce::PopupMenu singleWindowMenu;
    const auto addSingleWindowItem = [&](const auto& id)
    {
        singleWindowMenu.addItem(getWindowTitle(id), [this, id = id]
        {
            windowLayout.removeAllChildren(nullptr);
            juce::ValueTree row(WindowIDs::Row);
            juce::ValueTree window(id);
            window.setProperty(WindowIDs::size, 0.5f, nullptr);
            row.appendChild(window, nullptr);
            windowLayout.appendChild(row, nullptr);
        });
    };
    addSingleWindowItem(WindowIDs::DeviceSettings);
    addSingleWindowItem(WindowIDs::ThreeDView);
    addSingleWindowItem(WindowIDs::SerialAccessoryTerminal);
    singleWindowMenu.addSeparator();
    singleWindowMenu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("GRAPHS"), nullptr);
    addSingleWindowItem(WindowIDs::Gyroscope);
    addSingleWindowItem(WindowIDs::Accelerometer);
    addSingleWindowItem(WindowIDs::Magnetometer);
    addSingleWindowItem(WindowIDs::EulerAngles);
    addSingleWindowItem(WindowIDs::LinearAcceleration);
    addSingleWindowItem(WindowIDs::EarthAcceleration);
    addSingleWindowItem(WindowIDs::HighGAccelerometer);
    addSingleWindowItem(WindowIDs::Temperature);
    addSingleWindowItem(WindowIDs::BatteryPercentage);
    addSingleWindowItem(WindowIDs::BatteryVoltage);
    addSingleWindowItem(WindowIDs::RssiPercentage);
    addSingleWindowItem(WindowIDs::RssiPower);
    addSingleWindowItem(WindowIDs::ReceivedMessageRate);
    addSingleWindowItem(WindowIDs::ReceivedDataRate);
    menu.addSubMenu("Single Window", singleWindowMenu);

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
                    return true;
                };

                if (CustomLayouts().exists(layoutName))
                {
                    DialogLauncher::launchDialog(std::make_unique<DoYouWantToReplaceItDialog>(layoutName), save);
                }
                else
                {
                    save();
                }
            }
            return true;
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

    const auto addItem = [&](const auto layout, const auto& title, const auto enabled)
    {
        menu.addItem(title, enabled, devicePanelContainer.getLayout() == layout, [&, layout]
        {
            devicePanelContainer.setLayout(layout);
            devicePanelLayoutButton.setIcon(layoutIcons.at(layout), {});

            if (layout != DevicePanelContainer::Layout::single)
            {
                preferredMultipleDevicePanelLayout = layout;
            }
        });
    };
    addItem(DevicePanelContainer::Layout::single, "Single", devicePanelContainer.getDevicePanels().size() == 1);
    addItem(DevicePanelContainer::Layout::rows, "Rows", devicePanelContainer.getDevicePanels().size() > 1);
    addItem(DevicePanelContainer::Layout::columns, "Columns", devicePanelContainer.getDevicePanels().size() > 1);
    addItem(DevicePanelContainer::Layout::grid, "Grid", devicePanelContainer.getDevicePanels().size() > 1);
    addItem(DevicePanelContainer::Layout::accordion, "Accordion", devicePanelContainer.getDevicePanels().size() > 1);

    return menu;
}

juce::PopupMenu MenuStrip::getToolsMenu()
{
    juce::PopupMenu menu;
    menu.addItem("Set Date and Time", devicePanelContainer.getDevicePanels().size() > 0, false, [&]
    {
        DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("Do you want to set the date and time on all devices to match the computer?"), [&]
        {
            DialogLauncher::launchDialog(std::make_unique<SendingCommandDialog>(CommandMessage("time", juce::Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S")), devicePanelContainer.getDevicePanels()));
            return true;
        });
    });
    menu.addItem("Convert File (.ximu3)", []
    {
        DialogLauncher::launchDialog(std::make_unique<ConvertFileDialog>(), []
        {
            if (const auto* const convertFileDialog = dynamic_cast<ConvertFileDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                const auto startFileConverter = [source = convertFileDialog->getSource(), destination = convertFileDialog->getDestination()]
                {
                    DialogLauncher::launchDialog(std::make_unique<ConvertingFileDialog>(source, destination));
                };

                const auto directory = juce::File(convertFileDialog->getDestination()).getChildFile(juce::File(convertFileDialog->getSource()).getFileNameWithoutExtension());
                if (directory.exists())
                {
                    DialogLauncher::launchDialog(std::make_unique<DoYouWantToReplaceItDialog>(directory.getFileName()), [directory, startFileConverter]
                    {
                        directory.deleteRecursively();
                        startFileConverter();
                        return true;
                    });
                }
                else
                {
                    startFileConverter();
                }
            }
            return true;
        });
    });
    menu.addItem("Update Firmware", [&]
    {
        const auto launchUpdateFirmwareDialog = []
        {
            DialogLauncher::launchDialog(std::make_unique<UpdateFirmwareDialog>(), []
            {
                if (const auto* const updateFirmwareDialog = dynamic_cast<UpdateFirmwareDialog*>(DialogLauncher::getLaunchedDialog()))
                {
                    DialogLauncher::launchDialog(std::make_unique<UpdatingFirmwareDialog>(updateFirmwareDialog->getConnectionInfo(), updateFirmwareDialog->getFileName()));
                }
                return true;
            });
        };

        if (devicePanelContainer.getDevicePanels().size() > 0)
        {
            DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("All connections must be closed before updating the firmware. Do you want to continue?"), [&, launchUpdateFirmwareDialog]
            {
                disconnect(nullptr);
                launchUpdateFirmwareDialog();
                return true;
            });
            return;
        }

        launchUpdateFirmwareDialog();
    });
    return menu;
}

void MenuStrip::setWindowLayout(juce::ValueTree windowLayout_)
{
    if (windowLayout_.isValid() == false)
    {
        windowLayout_ = { WindowIDs::Row, {},
                          {
                                  { WindowIDs::DeviceSettings, {{ WindowIDs::size, 0.4 }}},
                                  { WindowIDs::Column, {}, {
                                          { WindowIDs::Row, {}, {
                                                  { WindowIDs::Gyroscope, {}},
                                                  { WindowIDs::Accelerometer, {}},
                                                  { WindowIDs::Magnetometer, {}},
                                                  { WindowIDs::HighGAccelerometer, {}},
                                          }},
                                          { WindowIDs::Row, {}, {
                                                  { WindowIDs::EulerAngles, {{ WindowIDs::size, 1 }}},
                                                  { WindowIDs::ThreeDView, {{ WindowIDs::size, 3 }}},
                                          }}
                                  }}
                          }};
    }

    windowLayout.copyPropertiesAndChildrenFrom(windowLayout_, nullptr);
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
