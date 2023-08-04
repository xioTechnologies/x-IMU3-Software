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
#include "Dialogs/SearchingForConnectionsDialog.h"
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

    for (const auto& buttonGroup : buttonGroups)
    {
        addAndMakeVisible(buttonGroup.label);
        for (auto* const button : buttonGroup.buttons)
        {
            addAndMakeVisible(button);
        }
    }

    searchButton.onClick = [this]
    {
        std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections;

        for (auto* const devicePanel : devicePanelContainer.getDevicePanels())
        {
            existingConnections.push_back(devicePanel->getConnection()->getInfo());
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<SearchingForConnectionsDialog>(std::move(existingConnections)), [this]
        {
            if (auto* dialog = dynamic_cast<SearchingForConnectionsDialog*>(DialogQueue::getSingleton().getActive()))
            {
                for (const auto& connectionInfo : dialog->getConnectionInfos())
                {
                    devicePanelContainer.connectToDevice(*connectionInfo);
                }
            }
            return true;
        });
    };

    if (ApplicationSettings::getSingleton().searchForConnections.showOnStartup)
    {
        searchButton.triggerClick();
    }

    shutdownButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to shutdown all devices?"), [this]
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("shutdown", {}), devicePanelContainer.getDevicePanels()));
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
            juce::File(dataLoggerSettings.directory).getChildFile(dataLoggerName).revealToUser();
            return;
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<DataLoggerSettingsDialog>(dataLoggerSettings), [this]
        {
            if (auto* dialog = dynamic_cast<DataLoggerSettingsDialog*>(DialogQueue::getSingleton().getActive()))
            {
                dataLoggerSettings = dialog->getSettings();

                dataLoggerName = dataLoggerSettings.name;
                if (dataLoggerSettings.appendDateAndTime)
                {
                    dataLoggerName += juce::Time::getCurrentTime().formatted(" %Y-%m-%d %H-%M-%S");
                }

                const auto startDataLogger = [&]
                {
                    std::vector<ximu3::Connection*> connections;
                    for (auto* const devicePanel : devicePanelContainer.getDevicePanels())
                    {
                        connections.push_back(devicePanel->getConnection().get());
                    }

                    dataLogger = std::make_unique<ximu3::DataLogger>(dataLoggerSettings.directory.toStdString(),
                                                                     dataLoggerName.toStdString(),
                                                                     connections);

                    if (dataLogger->getResult() != ximu3::XIMU3_ResultOk)
                    {
                        DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>("Data logger failed."));
                        return;
                    }

                    dataLoggerStartTime = juce::Time::getCurrentTime();
                    startTimerHz(25);
                    dataLoggerStartStopButton.setToggleState(true, juce::dontSendNotification);
                };

                const auto directory = juce::File(dataLoggerSettings.directory).getChildFile(dataLoggerName);
                if (directory.exists())
                {
                    DialogQueue::getSingleton().pushFront(std::make_unique<DoYouWantToReplaceItDialog>(dataLoggerName), [directory, startDataLogger]
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
        DialogQueue::getSingleton().pushFront(std::make_unique<ApplicationSettingsDialog>());
    };

    versionButton.onClick = []
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AboutDialog>());
    };
    versionButton.setColour(juce::TextButton::buttonColourId, {});
    versionButton.setColour(juce::TextButton::buttonOnColourId, {});

    devicePanelContainer.onDevicePanelsSizeChanged = [&]
    {
        for (auto& component : std::vector<std::reference_wrapper<juce::Component>>({ disconnectButton, windowsButton, windowLayoutButton, devicePanelLayoutButton,
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
}

void MenuStrip::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundLightest);
}

void MenuStrip::resized()
{
    static constexpr int buttonY = 8;
    static constexpr int buttonHeight = 22;
    static constexpr int labelCentreY = 40;

    // Calculate groupMargin
    auto groupMargin = (float) getWidth();
    for (const auto& buttonGroup : buttonGroups)
    {
        for (auto* const button : buttonGroup.buttons)
        {
            const auto buttonWidth = [&]
            {
                if (button == &dataLoggerTime)
                {
                    return 112;
                }
                if (button == &versionButton)
                {
                    return versionButton.getBestWidthForHeight(buttonHeight);
                }
                return 35;
            }();

            button->setSize(buttonWidth, buttonHeight);
            groupMargin -= button->getWidth();
        }
    }
    groupMargin /= buttonGroups.size();

    // Set bounds
    auto x = groupMargin / 2;
    for (const auto& buttonGroup : buttonGroups)
    {
        for (auto* const button : buttonGroup.buttons)
        {
            button->setBounds((int) x, buttonY, button->getWidth(), button->getHeight());
            x += button->getWidth();
        }
        x += groupMargin;

        const auto labelCentreX = (buttonGroup.buttons.front()->getX() + buttonGroup.buttons.back()->getRight()) / 2;
        const auto labelWidth = (int) std::ceil(buttonGroup.label.getTextWidth());
        const auto labelHeight = (int) std::ceil(UIFonts::getDefaultFont().getHeight());
        buttonGroup.label.setBounds(juce::Rectangle<int>(labelWidth, labelHeight).withCentre({ labelCentreX, labelCentreY }));
    }
}

int MenuStrip::getMinimumWidth() const
{
    int width = 0;
    for (const auto& buttonGroup : buttonGroups)
    {
        for (auto* const button : buttonGroup.buttons)
        {
            width += button->getWidth();
        }
    }
    return width;
}

void MenuStrip::addDevices(juce::PopupMenu& menu, std::function<void(DevicePanel&)> action)
{
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("INDIVIDUAL"), nullptr);

    for (auto* const devicePanel : devicePanelContainer.getDevicePanels())
    {
        juce::PopupMenu::Item item(devicePanel->getDeviceDescriptor() + "   " + devicePanel->getConnection()->getInfo()->toString());

        item.action = [devicePanel, action]
        {
            action(*devicePanel);
        };

        auto tag = std::make_unique<juce::DrawableRectangle>();
        int _, height;
        getLookAndFeel().getIdealPopupMenuItemSize({}, false, {}, _, height);
        tag->setRectangle(juce::Rectangle<float>(0.0f, 0.0f, (float) UILayout::tagWidth, (float) height));
        tag->setFill({ devicePanel->getTag() });
        item.image = std::move(tag);

        menu.addItem(item);
    }
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
        if (auto* dialog = dynamic_cast<NewConnectionDialog*>(DialogQueue::getSingleton().getActive()))
        {
            auto connectionInfo = dialog->getConnectionInfo();
            devicePanelContainer.connectToDevice(*connectionInfo);
            ConnectionHistory().update(*connectionInfo);
        }
        return true;
    };
    menu.addItem("New USB Connection", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<UsbConnectionDialog>(), connectCallback);
    });
    menu.addItem("New Serial Connection", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SerialConnectionDialog>(), connectCallback);
    });
    menu.addItem("New TCP Connection", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<TcpConnectionDialog>(), connectCallback);
    });
    menu.addItem("New UDP Connection", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<UdpConnectionDialog>(), connectCallback);
    });
    menu.addItem("New Bluetooth Connection", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<BluetoothConnectionDialog>(), connectCallback);
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

    menu.addItem("Disconnect All (" + juce::String(devicePanelContainer.getDevicePanels().size()) + ")", [this]
    {
        disconnect(nullptr);
    });
    addDevices(menu, [&](auto& devicePanel)
    {
        disconnect(&devicePanel);
    });

    return menu;
}

juce::PopupMenu MenuStrip::getWindowMenu() const
{
    juce::PopupMenu menu;

    const auto addWindowItem = [&](const auto& id)
    {
        const auto toggled = findWindow(windowLayout, id).isValid();
        menu.addItem(getWindowTitle(id), true, toggled, [this, id = id, toggled]
        {
            if (toggled)
            {
                for (auto child = findWindow(windowLayout, id); child.isValid() && child.getNumChildren() == 0;)
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
        DialogQueue::getSingleton().pushFront(std::make_unique<SaveWindowLayoutDialog>(), [this]
        {
            if (auto* saveWindowLayoutDialog = dynamic_cast<SaveWindowLayoutDialog*>(DialogQueue::getSingleton().getActive()))
            {
                const auto layoutName = saveWindowLayoutDialog->getLayoutName();

                const auto save = [this, layoutName]
                {
                    CustomLayouts().save(layoutName, windowLayout);
                    return true;
                };

                if (CustomLayouts().exists(layoutName))
                {
                    DialogQueue::getSingleton().pushFront(std::make_unique<DoYouWantToReplaceItDialog>(layoutName), save);
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

juce::PopupMenu MenuStrip::getSendCommandMenu()
{
    juce::PopupMenu menu;

    const auto toAll = "Send Command to All (" + juce::String(devicePanelContainer.getDevicePanels().size()) + ")";
    menu.addItem(toAll, [&, toAll]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendCommandDialog>(toAll), [this]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogQueue::getSingleton().getActive()))
            {
                DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(dialog->getCommand(), devicePanelContainer.getDevicePanels()));
            }
            return true;
        });
    });

    addDevices(menu, [&](auto& devicePanel)
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendCommandDialog>("Send Command to " + devicePanel.getDeviceDescriptor(), devicePanel.getTag()), [&, devicePanel = &devicePanel]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogQueue::getSingleton().getActive()))
            {
                DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(dialog->getCommand(), std::vector<DevicePanel*>({ devicePanel })));
            }
            return true;
        });
    });

    return menu;
}

juce::PopupMenu MenuStrip::getToolsMenu()
{
    juce::PopupMenu menu;
    menu.addItem("Set Date and Time", devicePanelContainer.getDevicePanels().size() > 0, false, [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Do you want to set the date and time on all devices to match the computer?"), [&]
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("time", juce::Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S")), devicePanelContainer.getDevicePanels()));
            return true;
        });
    });
    menu.addItem("Convert File (.ximu3)", []
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ConvertFileDialog>(), []
        {
            if (const auto* const convertFileDialog = dynamic_cast<ConvertFileDialog*>(DialogQueue::getSingleton().getActive()))
            {
                const auto startFileConverter = [source = convertFileDialog->getSource(), destination = convertFileDialog->getDestination()]
                {
                    DialogQueue::getSingleton().pushFront(std::make_unique<ConvertingFileDialog>(source, destination));
                };

                const auto directory = juce::File(convertFileDialog->getDestination()).getChildFile(juce::File(convertFileDialog->getSource()).getFileNameWithoutExtension());
                if (directory.exists())
                {
                    DialogQueue::getSingleton().pushFront(std::make_unique<DoYouWantToReplaceItDialog>(directory.getFileName()), [directory, startFileConverter]
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
            DialogQueue::getSingleton().pushFront(std::make_unique<UpdateFirmwareDialog>(), []
            {
                if (const auto* const updateFirmwareDialog = dynamic_cast<UpdateFirmwareDialog*>(DialogQueue::getSingleton().getActive()))
                {
                    DialogQueue::getSingleton().pushFront(std::make_unique<UpdatingFirmwareDialog>(updateFirmwareDialog->getConnectionInfo(), updateFirmwareDialog->getFileName()));
                }
                return true;
            });
        };

        if (devicePanelContainer.getDevicePanels().size() > 0)
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("All connections must be closed before updating the firmware. Do you want to continue?"), [&, launchUpdateFirmwareDialog]
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
    const auto timePassed = juce::Time::getCurrentTime() - dataLoggerStartTime;
    if (dataLoggerSettings.getTime().has_value() && timePassed >= *dataLoggerSettings.getTime())
    {
        dataLoggerStartStopButton.setToggleState(false, juce::sendNotificationSync);
        dataLoggerTime.setTime(*dataLoggerSettings.getTime());
    }
    else
    {
        dataLoggerTime.setTime(timePassed);
    }
}
