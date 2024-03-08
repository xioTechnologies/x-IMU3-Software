#include "ApplicationSettings.h"
#include "ConnectionPanelContainer.h"
#include "CustomLookAndFeel.h"
#include "Dialogs/AboutDialog.h"
#include "Dialogs/ApplicationSettingsDialog.h"
#include "Dialogs/AvailableConnectionsDialog.h"
#include "Dialogs/ConvertFilesDialog.h"
#include "Dialogs/ConvertingFileDialog.h"
#include "Dialogs/ManualConnectionDialog.h"
#include "Dialogs/MessageDialog.h"
#include "Dialogs/SaveWindowLayoutDialog.h"
#include "Dialogs/SendCommandDialog.h"
#include "Dialogs/SendingCommandDialog.h"
#include "Dialogs/SendNoteCommandDialog.h"
#include "Dialogs/UpdateFirmwareDialog.h"
#include "MenuStrip.h"
#include "RecentConnections.h"
#include "Widgets/PopupMenuHeader.h"
#include "WindowLayouts.h"
#include "Windows/WindowIDs.h"

MenuStrip::MenuStrip(juce::ValueTree& windowLayout_, juce::ThreadPool& threadPool_, ConnectionPanelContainer& connectionPanelContainer_) : windowLayout(windowLayout_),
                                                                                                                                           threadPool(threadPool_),
                                                                                                                                           connectionPanelContainer(connectionPanelContainer_)
{
    setWindowLayout(juce::ValueTree::fromXml(previousWindowLayout.loadFileAsString()));

    for (const auto& buttonGroup : buttonGroups)
    {
        for (auto* const button : buttonGroup)
        {
            addAndMakeVisible(button);
        }
    }

    availableConnectionsButton.onClick = [this]
    {
        std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections;

        for (auto* const connectionPanel : connectionPanelContainer.getConnectionPanels())
        {
            existingConnections.push_back(connectionPanel->getConnection()->getInfo());
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<AvailableConnectionsDialog>(std::move(existingConnections)), [this]
        {
            if (auto* dialog = dynamic_cast<AvailableConnectionsDialog*>(DialogQueue::getSingleton().getActive()))
            {
                for (const auto& connectionInfo : dialog->getConnectionInfos())
                {
                    connectionPanelContainer.connectToDevice(*connectionInfo);
                }
            }
            return true;
        });
    };

    if (ApplicationSettings::getSingleton().availableConnections.showOnStartup)
    {
        availableConnectionsButton.triggerClick();
    }

    shutdownButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to shutdown all devices?"), [this]
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("shutdown", {}), connectionPanelContainer.getConnectionPanels()));
            return true;
        });
    };

    zeroHeadingButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("heading", 0), connectionPanelContainer.getConnectionPanels()));
    };

    noteButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendNoteCommandDialog>("Send Note Command to All (" + juce::String(connectionPanelContainer.getConnectionPanels().size()) + ")"), [this]
        {
            if (auto* dialog = dynamic_cast<SendNoteCommandDialog*>(DialogQueue::getSingleton().getActive()))
            {
                DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("note", dialog->getNote()), connectionPanelContainer.getConnectionPanels()));
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
            juce::File(dataLoggerSettings.directory).getChildFile(dataLoggerName).revealToUser();
            return;
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<DataLoggerSettingsDialog>(dataLoggerSettings), [this]
        {
            if (auto* dialog = dynamic_cast<DataLoggerSettingsDialog*>(DialogQueue::getSingleton().getActive()))
            {
                dataLoggerSettings = dialog->getSettings();

                dataLoggerName = dataLoggerSettings.name;

                const auto startDataLogger = [&]
                {
                    std::vector<ximu3::Connection*> connections;
                    for (auto* const connectionPanel : connectionPanelContainer.getConnectionPanels())
                    {
                        connections.push_back(connectionPanel->getConnection().get());
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

    aboutButton.onClick = [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AboutDialog>(latestVersion));
    };

    threadPool.addJob([&, self = SafePointer<juce::Component>(this)]
                      {
                          const auto parsed = juce::JSON::parse(juce::URL("https://api.github.com/repos/xioTechnologies/x-IMU3-Software/releases/latest").readEntireTextStream());

                          juce::MessageManager::callAsync([&, self, parsed]
                                                          {
                                                              if (self == nullptr)
                                                              {
                                                                  return;
                                                              }

                                                              if (const auto* const object = parsed.getDynamicObject())
                                                              {
                                                                  latestVersion = object->getProperty("tag_name");
                                                                  aboutButton.setToggleState(latestVersion != ("v" + juce::JUCEApplication::getInstance()->getApplicationVersion()), juce::dontSendNotification);
                                                                  resized();
                                                              }
                                                          });
                      });

    connectionPanelContainer.onConnectionPanelsSizeChanged = [&]
    {
        for (auto& component : std::vector<std::reference_wrapper<juce::Component>>({ disconnectButton, windowsButton, shutdownButton, zeroHeadingButton, noteButton, sendCommandButton, dataLoggerStartStopButton, dataLoggerTime, }))
        {
            component.get().setEnabled(connectionPanelContainer.getConnectionPanels().size() > 0);
        }

        connectionLayoutButton.setEnabled(connectionPanelContainer.getConnectionPanels().size() > 1);
    };
    connectionPanelContainer.onConnectionPanelsSizeChanged();
}

MenuStrip::~MenuStrip()
{
    previousWindowLayout.replaceWithText(windowLayout.toXmlString());
}

void MenuStrip::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundLightest);
}

void MenuStrip::resized()
{
    static constexpr int buttonHeight = 22;

    // Calculate groupMargin
    auto groupMargin = (float) getWidth();
    for (const auto& buttonGroup : buttonGroups)
    {
        for (auto* const button : buttonGroup)
        {
            const auto buttonWidth = [&]
            {
                if (button == &dataLoggerTime)
                {
                    return 112;
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
        for (auto* const button : buttonGroup)
        {
            button->setBounds((int) x, getHeight() / 2 - button->getHeight() / 2, button->getWidth(), button->getHeight());
            x += button->getWidth();
        }
        x += groupMargin;
    }
}

int MenuStrip::getMinimumWidth() const
{
    int width = 0;
    for (const auto& buttonGroup : buttonGroups)
    {
        for (auto* const button : buttonGroup)
        {
            width += button->getWidth();
        }
    }
    return width;
}

void MenuStrip::addDevices(juce::PopupMenu& menu, std::function<void(ConnectionPanel&)> action)
{
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("INDIVIDUAL"), nullptr);

    for (auto* const connectionPanel : connectionPanelContainer.getConnectionPanels())
    {
        juce::PopupMenu::Item item(connectionPanel->getTitle());

        item.action = [connectionPanel, action]
        {
            action(*connectionPanel);
        };

        auto tag = std::make_unique<juce::DrawableRectangle>();
        int _, height;
        getLookAndFeel().getIdealPopupMenuItemSize({}, false, {}, _, height);
        tag->setRectangle(juce::Rectangle<float>(0.0f, 0.0f, (float) UILayout::tagWidth, (float) height));
        tag->setFill({ connectionPanel->getTag() });
        item.image = std::move(tag);

        menu.addItem(item);
    }
}

void MenuStrip::disconnect(const ConnectionPanel* const connectionPanel)
{
    dataLoggerTime.setTime(juce::RelativeTime());
    dataLoggerStartStopButton.setToggleState(false, juce::sendNotificationSync);

    if (connectionPanel != nullptr)
    {
        connectionPanelContainer.removePanel(*connectionPanel);
    }
    else
    {
        connectionPanelContainer.removeAllPanels();
    }
}

juce::PopupMenu MenuStrip::getManualConnectionMenu()
{
    juce::PopupMenu menu;

    const auto connectCallback = [this]
    {
        if (auto* dialog = dynamic_cast<ManualConnectionDialog*>(DialogQueue::getSingleton().getActive()))
        {
            auto connectionInfo = dialog->getConnectionInfo();
            connectionPanelContainer.connectToDevice(*connectionInfo);
            RecentConnections().update(*connectionInfo);
        }
        return true;
    };
    menu.addItem("USB", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ManualUsbConnectionDialog>(), connectCallback);
    });
    menu.addItem("Serial", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ManualSerialConnectionDialog>(), connectCallback);
    });
    menu.addItem("TCP", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ManualTcpConnectionDialog>(), connectCallback);
    });
    menu.addItem("UDP", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ManualUdpConnectionDialog>(), connectCallback);
    });
    menu.addItem("Bluetooth", [connectCallback]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ManualBluetoothConnectionDialog>(), connectCallback);
    });

    if (auto connectionInfos = RecentConnections().get(); connectionInfos.empty() == false)
    {
        menu.addSeparator();
        menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("RECENT"), nullptr);

        for (auto& connectionInfo : connectionInfos)
        {
            const auto connectionInfoString = connectionInfo->toString();
            menu.addItem(connectionInfoString, [this, connectionInfo = std::shared_ptr<ximu3::ConnectionInfo>(connectionInfo.release())]
            {
                connectionPanelContainer.connectToDevice(*connectionInfo);
            });
        }
    }

    return menu;
}

juce::PopupMenu MenuStrip::getDisconnectMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Disconnect All (" + juce::String(connectionPanelContainer.getConnectionPanels().size()) + ")", [this]
    {
        disconnect(nullptr);
    });
    addDevices(menu, [&](auto& connectionPanel)
    {
        disconnect(&connectionPanel);
    });

    return menu;
}

juce::PopupMenu MenuStrip::getConnectionLayoutMenu()
{
    juce::PopupMenu menu;

    const auto addItem = [&](const auto layout, const auto& title)
    {
        menu.addItem(title, true, connectionPanelContainer.getLayout() == layout, [&, layout]
        {
            connectionPanelContainer.setLayout(layout);
            connectionLayoutButton.setIcon(layoutIcons.at(layout), {});
        });
    };
    addItem(ConnectionPanelContainer::Layout::rows, "Rows");
    addItem(ConnectionPanelContainer::Layout::columns, "Columns");
    addItem(ConnectionPanelContainer::Layout::grid, "Grid");
    addItem(ConnectionPanelContainer::Layout::accordion, "Accordion");

    return menu;
}

juce::PopupMenu MenuStrip::getWindowMenu()
{
    juce::PopupMenu menu;

    const auto tile = [&](const bool horizontally)
    {
        juce::ValueTree windowLayout_(WindowIDs::Row);

        for (const auto& [windowType, _] : windowTitles)
        {
            if (findWindow(windowLayout, windowType).isValid())
            {
                (horizontally ? windowLayout_.getOrCreateChildWithName(WindowIDs::Column, nullptr) : windowLayout_).appendChild({ windowType, {}}, nullptr);
            }
        }
        setWindowLayout({ WindowIDs::Row, {}, { windowLayout_ }});
    };

    juce::PopupMenu arrangeMenu;
    arrangeMenu.addItem("Default Layout", [&]
    {
        setWindowLayout({});
    });
    arrangeMenu.addItem("Tile Horizontally", [&, tile]
    {
        tile(true);
    });
    arrangeMenu.addItem("Tile Vertically", [&, tile]
    {
        tile(false);
    });
    arrangeMenu.addItem("Save As...", [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SaveWindowLayoutDialog>(), [this]
        {
            if (auto* saveWindowLayoutDialog = dynamic_cast<SaveWindowLayoutDialog*>(DialogQueue::getSingleton().getActive()))
            {
                const auto layoutName = saveWindowLayoutDialog->getLayoutName();

                const auto save = [this, layoutName]
                {
                    WindowLayouts().save(layoutName, windowLayout);
                    return true;
                };

                if (WindowLayouts().exists(layoutName))
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

    if (const auto layouts = WindowLayouts().load(); layouts.empty() == false)
    {
        arrangeMenu.addSeparator();
        arrangeMenu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SAVED"), nullptr);

        for (auto layout : layouts)
        {
            arrangeMenu.addItem(layout.first, [&, layout]
            {
                setWindowLayout(layout.second);
            });
        }
    }

    menu.addSubMenu("Arrange", arrangeMenu);

    const auto addWindowItem = [&](const auto& id)
    {
        const auto toggled = findWindow(windowLayout, id).isValid();
        menu.addItem(windowTitles.at(id), true, toggled, [this, id = id, toggled]
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

            const auto newSize = juce::exactlyEqual(totalWindowSizes, 0.0f) ? 1.0f : (totalWindowSizes / (float) windowLayout.getRoot().getNumChildren());
            windowLayout.getRoot().appendChild({ id, {{ WindowIDs::size, newSize }}}, nullptr);
        });
    };

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("DEVICE SETTINGS"), nullptr);
    addWindowItem(WindowIDs::DeviceSettings);
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SENSORS"), nullptr);
    addWindowItem(WindowIDs::Gyroscope);
    addWindowItem(WindowIDs::Accelerometer);
    addWindowItem(WindowIDs::Magnetometer);
    addWindowItem(WindowIDs::HighGAccelerometer);
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("AHRS"), nullptr);
    addWindowItem(WindowIDs::ThreeDView);
    addWindowItem(WindowIDs::EulerAngles);
    addWindowItem(WindowIDs::LinearAcceleration);
    addWindowItem(WindowIDs::EarthAcceleration);
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SERIAL ACCESSORY"), nullptr);
    addWindowItem(WindowIDs::SerialAccessoryTerminal);
    addWindowItem(WindowIDs::SerialAccessoryCsvs);
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("STATUS"), nullptr);
    addWindowItem(WindowIDs::Temperature);
    addWindowItem(WindowIDs::BatteryPercentage);
    addWindowItem(WindowIDs::BatteryVoltage);
    addWindowItem(WindowIDs::RssiPercentage);
    addWindowItem(WindowIDs::RssiPower);
    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("CONNNECTION"), nullptr);
    addWindowItem(WindowIDs::ReceivedMessageRate);
    addWindowItem(WindowIDs::ReceivedDataRate);

    return menu;
}

juce::PopupMenu MenuStrip::getSendCommandMenu()
{
    juce::PopupMenu menu;

    const auto toAll = "Send Command to All (" + juce::String(connectionPanelContainer.getConnectionPanels().size()) + ")";
    menu.addItem(toAll, [&, toAll]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendCommandDialog>(toAll), [this]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogQueue::getSingleton().getActive()))
            {
                DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(dialog->getCommand(), connectionPanelContainer.getConnectionPanels()));
            }
            return true;
        });
    });

    addDevices(menu, [&](auto& connectionPanel)
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendCommandDialog>("Send Command to " + connectionPanel.getTitle(), connectionPanel.getTag()), [&, connectionPanel = &connectionPanel]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogQueue::getSingleton().getActive()))
            {
                DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(dialog->getCommand(), std::vector<ConnectionPanel*>({ connectionPanel })));
            }
            return true;
        });
    });

    return menu;
}

juce::PopupMenu MenuStrip::getToolsMenu()
{
    juce::PopupMenu menu;
    menu.addItem("Set Date and Time", connectionPanelContainer.getConnectionPanels().size() > 0, false, [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Do you want to set the date and time on all devices to match the computer?"), [&]
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("time", juce::Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S")), connectionPanelContainer.getConnectionPanels()));
            return true;
        });
    });
    menu.addItem("Convert .ximu3 Files", []
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<ConvertFilesDialog>(), []
        {
            if (const auto* const convertFileDialog = dynamic_cast<ConvertFilesDialog*>(DialogQueue::getSingleton().getActive()))
            {
                ConvertingFileDialog::show(convertFileDialog->getFiles(), convertFileDialog->getDestination());
            }
            return true;
        });
    });
    menu.addItem("Update Firmware", [&]
    {
        if (connectionPanelContainer.getConnectionPanels().size() > 0)
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("All connections must be closed before updating the firmware. Do you want to continue?"), [&]
            {
                disconnect(nullptr);
                UpdateFirmwareDialog::launch(threadPool);
                return true;
            });
            return;
        }

        UpdateFirmwareDialog::launch(threadPool);
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
