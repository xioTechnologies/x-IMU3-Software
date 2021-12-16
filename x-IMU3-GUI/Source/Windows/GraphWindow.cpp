#include "../Widgets/PopupMenuHeader.h"
#include "GraphWindow.h"
#include "Helpers.h"
#include "WindowIDs.h"

GraphWindow::GraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer) : Window(windowLayout, type, devicePanel_)
{
    static const juce::String degreeSymbol(juce::CharPointer_UTF8("\xc2\xba"));

    const auto setLabels = [&](const juce::String& yAxis, const std::vector<Graph::LegendItem>& legend)
    {
#ifdef USE_GRAPH
        graph = std::make_unique<Graph>(glRenderer, yAxis, legend);
        addAndMakeVisible(*graph);
#else
        addAndMakeVisible(yAxisLabel);
        yAxisLabel.setText(yAxis);

        for (const auto& item : legend)
        {
            auto& legendLabel = legendLabels.emplace_back(std::make_unique<SimpleLabel>());
            addAndMakeVisible(*legendLabel);
            legendLabel->setText(item.label);
            legendLabel->setColour(juce::Label::textColourId, item.colour);

            auto& argumentLabel = argumentLabels.emplace_back(std::make_unique<SimpleLabel>());
            addAndMakeVisible(*argumentLabel);

            arguments.push_back(std::make_unique<std::atomic<float>>(0.0f));
        }
        addAndMakeVisible(timestampLabel);

        startTimerHz(25);
        timerCallback();
#endif
    };

    if (type == WindowIDs::Gyroscope)
    {
        setLabels("Angular velocity (" + degreeSymbol + "/s)", {{ "X", UIColours::graphRed },
                                                                { "Y", UIColours::graphGreen },
                                                                { "Z", UIColours::graphBlue }});

        callbackIDs = { devicePanel.getConnection().addInertialCallback(inertialCallback = [&](auto message)
        {
            update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });
        }) };
    }
    if (type == WindowIDs::Accelerometer)
    {
        setLabels("Acceleration (g)", {{ "X", UIColours::graphRed },
                                       { "Y", UIColours::graphGreen },
                                       { "Z", UIColours::graphBlue }});

        callbackIDs = { devicePanel.getConnection().addInertialCallback(inertialCallback = [&](auto message)
        {
            update(message.timestamp, { message.accelerometer_x, message.accelerometer_y, message.accelerometer_z });
        }) };
    }
    if (type == WindowIDs::Magnetometer)
    {
        setLabels("Intensity (uT)", {{ "X", UIColours::graphRed },
                                     { "Y", UIColours::graphGreen },
                                     { "Z", UIColours::graphBlue }});

        callbackIDs = { devicePanel.getConnection().addMagnetometerCallback(magnetometerCallback = [&](auto message)
        {
            update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
        }) };
    }
    if (type == WindowIDs::EulerAngles)
    {
        setLabels("Angle (" + degreeSymbol + ")", {{ "Roll",  UIColours::graphRed },
                                                   { "Pitch", UIColours::graphGreen },
                                                   { "Yaw",   UIColours::graphBlue }});

        callbackIDs.push_back(devicePanel.getConnection().addQuaternionCallback(quaternionCallback = [&](auto message)
        {
            const auto eulerAngles = Helpers::ToEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);
            update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
        }));

        callbackIDs.push_back(devicePanel.getConnection().addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)
        {
            const auto quaternion = Helpers::ToQuaternion(message.xx_element, message.xy_element, message.xz_element,
                                                          message.yx_element, message.yy_element, message.yz_element,
                                                          message.zx_element, message.zy_element, message.zz_element);
            const auto eulerAngles = Helpers::ToEulerAngles(quaternion.vector.x, quaternion.vector.y, quaternion.vector.z, quaternion.scalar);
            update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
        }));

        callbackIDs.push_back(devicePanel.getConnection().addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)
        {
            update(message.timestamp, { message.roll, message.pitch, message.yaw });
        }));

        callbackIDs.push_back(devicePanel.getConnection().addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
        {
            const auto eulerAngles = Helpers::ToEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);
            update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
        }));

        callbackIDs.push_back(devicePanel.getConnection().addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
        {
            const auto eulerAngles = Helpers::ToEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);
            update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
        }));
    }
    if (type == WindowIDs::LinearAcceleration)
    {
        setLabels("Acceleration (g)", {{ "X", UIColours::graphRed },
                                       { "Y", UIColours::graphGreen },
                                       { "Z", UIColours::graphBlue }});

        callbackIDs = { devicePanel.getConnection().addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
        {
            update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
        }) };
    }
    if (type == WindowIDs::EarthAcceleration)
    {
        setLabels("Acceleration (g)", {{ "X", UIColours::graphRed },
                                       { "Y", UIColours::graphGreen },
                                       { "Z", UIColours::graphBlue }});

        callbackIDs = { devicePanel.getConnection().addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
        {
            update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
        }) };
    }
    if (type == WindowIDs::HighGAccelerometer)
    {
        setLabels("Acceleration (g)", {{ "X", UIColours::graphRed },
                                       { "Y", UIColours::graphGreen },
                                       { "Z", UIColours::graphBlue }});

        callbackIDs = { devicePanel.getConnection().addHighGCallback(highGCallback = [&](auto message)
        {
            update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
        }) };
    }
    if (type == WindowIDs::Temperature)
    {
        setLabels("Temperature (" + degreeSymbol + "C)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addTemperatureCallback(temperatureCallback = [&](auto message)
        {
            update(message.timestamp, { message.temperature });
        }) };
    }
    if (type == WindowIDs::BatteryPercentage)
    {
        setLabels("Percentage (%)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addBatteryCallback(batteryCallback = [&](auto message)
        {
            update(message.timestamp, { message.percentage });
        }) };
    }
    if (type == WindowIDs::BatteryVoltage)
    {
        setLabels("Voltage (V)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addBatteryCallback(batteryCallback = [&](auto message)
        {
            update(message.timestamp, { message.voltage });
        }) };
    }
    if (type == WindowIDs::RssiPercentage)
    {
        setLabels("Percentage (%)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addRssiCallback(rssiCallback = [&](auto message)
        {
            update(message.timestamp, { message.percentage });
        }) };
    }
    if (type == WindowIDs::RssiPower)
    {
        setLabels("Power (dBm)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addRssiCallback(rssiCallback = [&](auto message)
        {
            update(message.timestamp, { message.power });
        }) };
    }
    if (type == WindowIDs::ReceivedMessageRate)
    {
        setLabels("Receive rate (messages/s)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addStatisticsCallback(statisticsCallback = [&](auto message)
        {
            update(message.timestamp, { (float) message.message_rate });
        }) };
    }
    if (type == WindowIDs::ReceivedDataRate)
    {
        setLabels("Receive rate (kB/s)", {{{}, juce::Colours::yellow }});

        callbackIDs = { devicePanel.getConnection().addStatisticsCallback(statisticsCallback = [&](auto message)
        {
            update(message.timestamp, { (float) message.data_rate / 1000.0f });
        }) };
    }
}

GraphWindow::~GraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection().removeCallback(callbackID);
    }
}

#ifndef USE_GRAPH

void GraphWindow::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    Window::paint(g);
}

#endif

void GraphWindow::resized()
{
    Window::resized();

    juce::Rectangle<int> bounds = getContentBounds();

#ifdef USE_GRAPH
    graph->setBounds(bounds);
#else
    bounds.removeFromLeft(10);
    bounds.removeFromTop(10);

    const auto setLabel = [&](auto& label)
    {
        auto row = bounds.removeFromTop(20);
        label.setBounds(row);
    };

    const auto setLabels = [&](auto& labels)
    {
        auto row = bounds.removeFromTop(20);
        for (auto& label : labels)
        {
            label->setBounds(row.removeFromLeft(70));
        }
    };

    setLabel(yAxisLabel);
    setLabels(legendLabels);
    setLabels(argumentLabels);
    setLabel(timestampLabel);
#endif
}

void GraphWindow::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.mods.isRightButtonDown())
    {
        // Scroll wheel = zoom vertical
        // Scroll wheel + ctrl = zoom horizontal
        // Mouse drag = scroll horizontal / vertical

        juce::PopupMenu menu;

        menu.addItem("Restore Defaults", true, false, [&]
        {
            settings = {};
            isPaused = false;
#ifdef USE_GRAPH
            graph->setSettings(settings);
#endif
        });

        menu.addItem("Clear", true, false, [&]
        {
#ifdef USE_GRAPH
            graph->clear();
#endif
        });
        menu.addItem("Pause", true, isPaused, [&]
        {
            isPaused = !isPaused;
        });
        menu.addItem("Horizontal Autoscale", true, settings.horizontal.autoscale, [&]
        {
            settings.horizontal.autoscale = !settings.horizontal.autoscale;
#ifdef USE_GRAPH
            graph->setSettings(settings);
#endif
        });
        menu.addItem("Vertical Autoscale", true, settings.vertical.autoscale, [&]
        {
            settings.vertical.autoscale = !settings.vertical.autoscale;
#ifdef USE_GRAPH
            graph->setSettings(settings);
#endif
        });

        menu.showMenuAsync({});
    }

#ifdef USE_GRAPH
    lastMousePosition = mouseEvent.getPosition();
#endif
}

#ifdef USE_GRAPH

void GraphWindow::mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel)
{
    static constexpr auto deltaScale = 2.0f;

    if (mouseEvent.mods.isAltDown())
    {
        const auto delta = juce::jmap<float>(wheel.deltaY, 0.0f, deltaScale, 0.0f, settings.vertical.max - settings.vertical.min);
        settings.vertical.max = settings.vertical.max - delta;
        settings.vertical.min = settings.vertical.min + delta;
    }
    else
    {
        const auto delta = juce::jmap<float>(wheel.deltaY, 0.0f, deltaScale, 0.0f, settings.horizontal.max - settings.horizontal.min);
        settings.horizontal.min = std::min(0.0f, settings.horizontal.min + delta);
    }
    graph->setSettings(settings);
}

#endif

#ifndef USE_GRAPH

void GraphWindow::timerCallback()
{
    for (size_t index = 0; index < arguments.size(); index++)
    {
        argumentLabels[index]->setText(juce::String(*arguments[index], 3));
    }

    timestampLabel.setText(juce::String(timestamp / 1e6, 3) + " s");
}

#endif

void GraphWindow::update(const uint64_t timestamp_, const std::vector<float>& arguments_)
{
    if (isPaused)
    {
        return;
    }

#ifdef USE_GRAPH
    graph->update(timestamp_, arguments_);
#else
    timestamp = timestamp_;

    for (size_t index = 0; index < arguments_.size(); index++)
    {
        *arguments[index] = arguments_[index];
    }
#endif
}
