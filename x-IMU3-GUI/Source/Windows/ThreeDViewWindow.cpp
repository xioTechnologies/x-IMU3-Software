#include "../DevicePanelContainer.h"
#include "Convert.h"
#include "ThreeDViewWindow.h"

ThreeDViewWindow::ThreeDViewWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : Window(devicePanel_, windowLayout_, type_, "3D View Menu", std::bind(&ThreeDViewWindow::getMenu, this)),
          threeDView(glRenderer)
{
    addAndMakeVisible(threeDView);

    addAndMakeVisible(rollLabel);
    addAndMakeVisible(rollValue);
    addAndMakeVisible(pitchLabel);
    addAndMakeVisible(pitchValue);
    addAndMakeVisible(yawLabel);
    addAndMakeVisible(yawValue);

    addAndMakeVisible(loadingLabel);

    quaternionCallbackID = devicePanel.getConnection().addQuaternionCallback(quaternionCallback = [&](auto message)
    {
        threeDView.update(message.x_element, message.y_element, message.z_element, message.w_element);

        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);

        roll = eulerAngles.x;
        pitch = eulerAngles.y;
        yaw = eulerAngles.z;
    });

    rotationMatrixCallbackID = devicePanel.getConnection().addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)
    {
        const auto quaternion = Convert::toQuaternion(message.xx_element, message.xy_element, message.xz_element,
                                                      message.yx_element, message.yy_element, message.yz_element,
                                                      message.zx_element, message.zy_element, message.zz_element);

        quaternionCallback({ message.timestamp, quaternion.scalar, quaternion.vector.x, quaternion.vector.y, quaternion.vector.z });
    });

    eulerAnglesCallbackID = devicePanel.getConnection().addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)
    {
        const auto quaternion = Convert::toQuaternion(message.roll, message.pitch, message.yaw);

        threeDView.update(quaternion.vector.x, quaternion.vector.y, quaternion.vector.z, quaternion.scalar);

        roll = message.roll;
        pitch = message.pitch;
        yaw = message.yaw;
    });

    linearAccelerationCallbackID = devicePanel.getConnection().addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
    {
        quaternionCallback({ message.timestamp, message.w_element, message.x_element, message.y_element, message.z_element });
    });

    earthAccelerationCallbackID = devicePanel.getConnection().addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        quaternionCallback({ message.timestamp, message.w_element, message.x_element, message.y_element, message.z_element });
    });

    startTimerHz(25);
    timerCallback();
}

ThreeDViewWindow::~ThreeDViewWindow()
{
    devicePanel.getConnection().removeCallback(quaternionCallbackID);
    devicePanel.getConnection().removeCallback(rotationMatrixCallbackID);
    devicePanel.getConnection().removeCallback(eulerAnglesCallbackID);
    devicePanel.getConnection().removeCallback(linearAccelerationCallbackID);
    devicePanel.getConnection().removeCallback(earthAccelerationCallbackID);
}

void ThreeDViewWindow::resized()
{
    Window::resized();
    juce::Rectangle<int> bounds = getContentBounds();
    threeDView.setBounds(bounds);

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
}

void ThreeDViewWindow::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay() != nullptr)
    {
        return;
    }

    const auto scale = 0.5f;

    settings.cameraAzimuth = wrapAngle(settings.cameraAzimuth + (scale * (mouseEvent.getPosition().getX() - lastMousePosition.getX())));
    settings.cameraElevation = wrapAngle(settings.cameraElevation + (scale * (mouseEvent.getPosition().getY() - lastMousePosition.getY())));
    threeDView.setSettings(settings);

    lastMousePosition = mouseEvent.getPosition();
}

void ThreeDViewWindow::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    settings.cameraOrbitDistance = juce::jlimit(1.0f, 4.0f, settings.cameraOrbitDistance + (0.5f * -wheel.deltaY));
    threeDView.setSettings(settings);
}

void ThreeDViewWindow::setEulerAnglesVisible(const bool visible)
{
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
        settings = {};
        threeDView.setSettings(settings);
        setEulerAnglesVisible(true);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("Euler Angles", true, rollLabel.isVisible(), [&]
    {
        setEulerAnglesVisible(!rollLabel.isVisible());
    });
    menu.addItem("Model", true, settings.isModelEnabled, [&]
    {
        settings.isModelEnabled = !settings.isModelEnabled;
        threeDView.setSettings(settings);
    });
    menu.addItem("World", true, settings.isWorldEnabled, [&]
    {
        settings.isWorldEnabled = !settings.isWorldEnabled;
        threeDView.setSettings(settings);
    });
    menu.addItem("Compass", true, settings.isCompassEnabled, [&]
    {
        settings.isCompassEnabled = !settings.isCompassEnabled;
        threeDView.setSettings(settings);
    });
    menu.addItem("Axes", true, settings.isAxesEnabled, [&]
    {
        settings.isAxesEnabled = !settings.isAxesEnabled;
        threeDView.setSettings(settings);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("MODEL"), nullptr);
    menu.addItem("Board", true, settings.model == ThreeDView::Model::board, [&]
    {
        settings.model = ThreeDView::Model::board;
        threeDView.setSettings(settings);
    });
    menu.addItem("Housing", true, settings.model == ThreeDView::Model::housing, [&]
    {
        settings.model = ThreeDView::Model::housing;
        threeDView.setSettings(settings);
    });
    menu.addItem("Custom", true, settings.model == ThreeDView::Model::custom, [&]
    {
        juce::FileChooser fileChooser("Select Custom Model", juce::File(), "*.obj");

        if (fileChooser.browseForFileToOpen())
        {
            threeDView.setCustomModel(fileChooser.getResult());
            settings.model = ThreeDView::Model::custom;
            threeDView.setSettings(settings);
        }
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("AXES CONVENTION"), nullptr);
    menu.addItem("North-West-Up (NWU)", true, settings.axesConvention == ThreeDView::AxesConvention::nwu, [&]
    {
        settings.axesConvention = ThreeDView::AxesConvention::nwu;
        threeDView.setSettings(settings);
    });
    menu.addItem("East-North-Up (ENU)", true, settings.axesConvention == ThreeDView::AxesConvention::enu, [&]
    {
        settings.axesConvention = ThreeDView::AxesConvention::enu;
        threeDView.setSettings(settings);
    });
    menu.addItem("North-East-Down (NED)", true, settings.axesConvention == ThreeDView::AxesConvention::ned, [&]
    {
        settings.axesConvention = ThreeDView::AxesConvention::ned;
        threeDView.setSettings(settings);
    });

    return menu;
}

void ThreeDViewWindow::timerCallback()
{
    static const auto formatAngle = [](const float angle)
    {
        const auto text = juce::String(angle, 1);
        return ((text == "-0.0") ? "0.0" : text) + "°";
    };
    rollValue.setText(formatAngle(roll));
    pitchValue.setText(formatAngle(pitch));
    yawValue.setText(formatAngle(yaw));
    loadingLabel.setText(threeDView.isLoading() ? "Loading..." : "");
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
