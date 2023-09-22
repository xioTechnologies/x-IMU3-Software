#include "./DevicePanelContainer.h"
#include "Convert.h"
#include "ThreeDViewWindow.h"

ThreeDViewWindow::ThreeDViewWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer, juce::ValueTree settingsTree_)
        : Window(windowLayout_, type_, devicePanel_, "3D View Menu", std::bind(&ThreeDViewWindow::getMenu, this)),
          threeDView(glRenderer),
          settingsTree(settingsTree_)
{
    addAndMakeVisible(threeDView);

    addAndMakeVisible(rollLabel);
    addAndMakeVisible(rollValue);
    addAndMakeVisible(pitchLabel);
    addAndMakeVisible(pitchValue);
    addAndMakeVisible(yawLabel);
    addAndMakeVisible(yawValue);

    addAndMakeVisible(loadingLabel);

    quaternionCallbackID = devicePanel.getConnection()->addQuaternionCallback(quaternionCallback = [&](auto message)
    {
        threeDView.update(message.x_element, message.y_element, message.z_element, message.w_element);

        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);

        roll = eulerAngles.x;
        pitch = eulerAngles.y;
        yaw = eulerAngles.z;
    });

    rotationMatrixCallbackID = devicePanel.getConnection()->addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)
    {
        const auto quaternion = Convert::toQuaternion(message.xx_element, message.xy_element, message.xz_element,
                                                      message.yx_element, message.yy_element, message.yz_element,
                                                      message.zx_element, message.zy_element, message.zz_element);

        quaternionCallback({ message.timestamp, quaternion.scalar, quaternion.vector.x, quaternion.vector.y, quaternion.vector.z });
    });

    eulerAnglesCallbackID = devicePanel.getConnection()->addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)
    {
        const auto quaternion = Convert::toQuaternion(message.roll, message.pitch, message.yaw);

        threeDView.update(quaternion.vector.x, quaternion.vector.y, quaternion.vector.z, quaternion.scalar);

        roll = message.roll;
        pitch = message.pitch;
        yaw = message.yaw;
    });

    linearAccelerationCallbackID = devicePanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
    {
        quaternionCallback({ message.timestamp, message.w_element, message.x_element, message.y_element, message.z_element });
    });

    earthAccelerationCallbackID = devicePanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        quaternionCallback({ message.timestamp, message.w_element, message.x_element, message.y_element, message.z_element });
    });

    settingsTree.addListener(this);
    threeDView.setSettings(readFromValueTree());

    startTimerHz(25);
    timerCallback();
}

ThreeDViewWindow::~ThreeDViewWindow()
{
    devicePanel.getConnection()->removeCallback(quaternionCallbackID);
    devicePanel.getConnection()->removeCallback(rotationMatrixCallbackID);
    devicePanel.getConnection()->removeCallback(eulerAnglesCallbackID);
    devicePanel.getConnection()->removeCallback(linearAccelerationCallbackID);
    devicePanel.getConnection()->removeCallback(earthAccelerationCallbackID);
}

void ThreeDViewWindow::resized()
{
    Window::resized();
    juce::Rectangle<int> bounds = getContentBounds();

    compactView = (getWidth() < 300) || (getHeight() < 150);

    threeDView.setBounds(bounds);
    threeDView.setHudEnabled(compactView == false);

    updateLabelVisibilities();

    bounds.reduce(10, 10);

    const auto setRow = [&](auto& label, auto& value)
    {
        auto row = bounds.removeFromTop(20);
        label.setBounds(row.removeFromLeft(50));
        value.setBounds(row);
    };

    setRow(rollLabel, rollValue);
    setRow(pitchLabel, pitchValue);
    setRow(yawLabel, yawValue);

    loadingLabel.setBounds(bounds.removeFromRight(100).removeFromBottom(20));
}

void ThreeDViewWindow::mouseDown(const juce::MouseEvent& mouseEvent)
{
    lastMousePosition = mouseEvent.getPosition();

    if (mouseEvent.mods.isPopupMenu())
    {
        getMenu().show();
    }
}

void ThreeDViewWindow::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay() != nullptr)
    {
        return;
    }

    const auto scale = 0.5f;

    auto settings = threeDView.getSettings();
    settings.cameraAzimuth = wrapAngle(settings.cameraAzimuth + (scale * (mouseEvent.getPosition().getX() - lastMousePosition.getX())));
    settings.cameraElevation = wrapAngle(settings.cameraElevation + (scale * (mouseEvent.getPosition().getY() - lastMousePosition.getY())));
    writeToValueTree(settings);

    lastMousePosition = mouseEvent.getPosition();
}

void ThreeDViewWindow::mouseDoubleClick(const juce::MouseEvent&)
{
    auto settings = threeDView.getSettings();
    settings.cameraAzimuth = ThreeDView::Settings().cameraAzimuth;
    settings.cameraElevation = ThreeDView::Settings().cameraElevation;
    settings.cameraOrbitDistance = ThreeDView::Settings().cameraOrbitDistance;
    writeToValueTree(settings);
}

void ThreeDViewWindow::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    auto settings = threeDView.getSettings();
    settings.cameraOrbitDistance = juce::jlimit(1.0f, 4.0f, settings.cameraOrbitDistance + (0.5f * -wheel.deltaY));
    writeToValueTree(settings);
}

float ThreeDViewWindow::wrapAngle(float angle)
{
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }

    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}

void ThreeDViewWindow::writeToValueTree(const ThreeDView::Settings& settings)
{
    settingsTree.setProperty("cameraAzimuth", settings.cameraAzimuth, nullptr);
    settingsTree.setProperty("cameraElevation", settings.cameraElevation, nullptr);
    settingsTree.setProperty("cameraOrbitDistance", settings.cameraOrbitDistance, nullptr);
    settingsTree.setProperty("worldEnabled", settings.worldEnabled, nullptr);
    settingsTree.setProperty("modelEnabled", settings.modelEnabled, nullptr);
    settingsTree.setProperty("axesEnabled", settings.axesEnabled, nullptr);
    settingsTree.setProperty("compassEnabled", settings.compassEnabled, nullptr);
    settingsTree.setProperty("model", static_cast<int>(settings.model), nullptr);
    settingsTree.setProperty("axesConvention", static_cast<int>(settings.axesConvention), nullptr);
}

ThreeDView::Settings ThreeDViewWindow::readFromValueTree() const
{
    ThreeDView::Settings settings;
    settings.cameraAzimuth = settingsTree.getProperty("cameraAzimuth", settings.cameraAzimuth);
    settings.cameraElevation = settingsTree.getProperty("cameraElevation", settings.cameraElevation);
    settings.cameraOrbitDistance = settingsTree.getProperty("cameraOrbitDistance", settings.cameraOrbitDistance);
    settings.worldEnabled = settingsTree.getProperty("worldEnabled", settings.worldEnabled);
    settings.modelEnabled = settingsTree.getProperty("modelEnabled", settings.modelEnabled);
    settings.axesEnabled = settingsTree.getProperty("axesEnabled", settings.axesEnabled);
    settings.compassEnabled = settingsTree.getProperty("compassEnabled", settings.axesEnabled);
    settings.model = static_cast<ThreeDView::Model>((int) settingsTree.getProperty("model", static_cast<int>(settings.model)));
    settings.axesConvention = static_cast<ThreeDView::AxesConvention>((int) settingsTree.getProperty("axesConvention", static_cast<int>(settings.axesConvention)));
    return settings;
}

void ThreeDViewWindow::updateLabelVisibilities()
{
    const auto visible = (settingsTree.getProperty("eulerAnglesEnabled", true) && compactView == false);

    rollLabel.setVisible(visible);
    pitchLabel.setVisible(visible);
    yawLabel.setVisible(visible);
    rollValue.setVisible(visible);
    pitchValue.setVisible(visible);
    yawValue.setVisible(visible);
}

juce::PopupMenu ThreeDViewWindow::getMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Restore Defaults", true, false, [&]
    {
        settingsTree.removeAllProperties(nullptr);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("World", true, threeDView.getSettings().worldEnabled, [&]
    {
        auto settings = threeDView.getSettings();
        settings.worldEnabled = !settings.worldEnabled;
        writeToValueTree(settings);
    });
    menu.addItem("Model", true, threeDView.getSettings().modelEnabled, [&]
    {
        auto settings = threeDView.getSettings();
        settings.modelEnabled = !settings.modelEnabled;
        writeToValueTree(settings);
    });
    menu.addItem("Euler Angles", compactView == false, settingsTree.getProperty("eulerAnglesEnabled", true), [&]
    {
        settingsTree.setProperty("eulerAnglesEnabled", (bool) settingsTree.getProperty("eulerAnglesEnabled", true) == false, nullptr);
    });
    menu.addItem("Axes", compactView == false, threeDView.getSettings().axesEnabled, [&]
    {
        auto settings = threeDView.getSettings();
        settings.axesEnabled = !settings.axesEnabled;
        writeToValueTree(settings);
    });
    menu.addItem("Compass", true, threeDView.getSettings().compassEnabled, [&]
    {
        auto settings = threeDView.getSettings();
        settings.compassEnabled = !settings.compassEnabled;
        writeToValueTree(settings);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("MODEL"), nullptr);
    menu.addItem("Board", true, threeDView.getSettings().model == ThreeDView::Model::board, [&]
    {
        auto settings = threeDView.getSettings();
        settings.model = ThreeDView::Model::board;
        writeToValueTree(settings);
    });
    menu.addItem("Housing", true, threeDView.getSettings().model == ThreeDView::Model::housing, [&]
    {
        auto settings = threeDView.getSettings();
        settings.model = ThreeDView::Model::housing;
        writeToValueTree(settings);
    });
    menu.addItem("Custom", true, threeDView.getSettings().model == ThreeDView::Model::custom, [&]
    {
        juce::FileChooser fileChooser("Select Custom Model", juce::File(), "*.obj");

        if (fileChooser.browseForFileToOpen())
        {
            threeDView.setCustomModel(fileChooser.getResult());

            auto settings = threeDView.getSettings();
            settings.model = ThreeDView::Model::custom;
            writeToValueTree(settings);
        }
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("AXES CONVENTION"), nullptr);
    menu.addItem("North-West-Up (NWU)", true, threeDView.getSettings().axesConvention == ThreeDView::AxesConvention::nwu, [&]
    {
        auto settings = threeDView.getSettings();
        settings.axesConvention = ThreeDView::AxesConvention::nwu;
        writeToValueTree(settings);
    });
    menu.addItem("East-North-Up (ENU)", true, threeDView.getSettings().axesConvention == ThreeDView::AxesConvention::enu, [&]
    {
        auto settings = threeDView.getSettings();
        settings.axesConvention = ThreeDView::AxesConvention::enu;
        writeToValueTree(settings);
    });
    menu.addItem("North-East-Down (NED)", true, threeDView.getSettings().axesConvention == ThreeDView::AxesConvention::ned, [&]
    {
        auto settings = threeDView.getSettings();
        settings.axesConvention = ThreeDView::AxesConvention::ned;
        writeToValueTree(settings);
    });

    return menu;
}

void ThreeDViewWindow::timerCallback()
{
    static const auto formatAngle = [](const float angle)
    {
        auto text = juce::String(angle, 1);

        if (text == "-0.0")
        {
            text = "0.0";
        }
        else if (text == "-180.0")
        {
            text = "180.0";
        }

        return text + "°";
    };
    rollValue.setText(formatAngle(roll));
    pitchValue.setText(formatAngle(pitch));
    yawValue.setText(formatAngle(yaw));

    loadingLabel.setText(threeDView.isLoading() ? "Loading.." : "");
}

void ThreeDViewWindow::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if (property.toString() == "eulerAnglesEnabled")
    {
        updateLabelVisibilities();
        return;
    }

    threeDView.setSettings(readFromValueTree());
}
