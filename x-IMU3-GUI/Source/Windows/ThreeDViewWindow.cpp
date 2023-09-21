#include "../DevicePanelContainer.h"
#include "Convert.h"
#include "ThreeDViewWindow.h"

ThreeDView::Settings ThreeDViewWindow::settings;
bool ThreeDViewWindow::eulerAnglesEnabled = true;

ThreeDViewWindow::ThreeDViewWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : Window(windowLayout_, type_, devicePanel_, "3D View Menu", std::bind(&ThreeDViewWindow::getMenu, this)),
          threeDView(glRenderer, settings)
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

    lastMousePosition = mouseEvent.getPosition();
}

void ThreeDViewWindow::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    settings.cameraOrbitDistance = juce::jlimit(1.0f, 4.0f, settings.cameraOrbitDistance + (0.5f * -wheel.deltaY));
}

juce::PopupMenu ThreeDViewWindow::getMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Restore Defaults", true, false, [&]
    {
        settings = {};
        eulerAnglesEnabled = true;
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("World", true, settings.worldEnabled, [&]
    {
        settings.worldEnabled = !settings.worldEnabled;
    });
    menu.addItem("Model", true, settings.modelEnabled, [&]
    {
        settings.modelEnabled = !settings.modelEnabled;
    });
    menu.addItem("Euler Angles", true, rollLabel.isVisible(), [&]
    {
        eulerAnglesEnabled = !eulerAnglesEnabled;
    });
    menu.addItem("Axes", true, settings.axesEnabled, [&]
    {
        settings.axesEnabled = !settings.axesEnabled;
    });
    menu.addItem("Compass", true, settings.compassEnabled, [&]
    {
        settings.compassEnabled = !settings.compassEnabled;
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("MODEL"), nullptr);
    menu.addItem("Board", true, settings.model == ThreeDView::Model::board, [&]
    {
        settings.model = ThreeDView::Model::board;
    });
    menu.addItem("Housing", true, settings.model == ThreeDView::Model::housing, [&]
    {
        settings.model = ThreeDView::Model::housing;
    });
    menu.addItem("Custom", true, settings.model == ThreeDView::Model::custom, [&]
    {
        juce::FileChooser fileChooser("Select Custom Model", juce::File(), "*.obj");

        if (fileChooser.browseForFileToOpen())
        {
            threeDView.setCustomModel(fileChooser.getResult());
            settings.model = ThreeDView::Model::custom;
        }
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("AXES CONVENTION"), nullptr);
    menu.addItem("North-West-Up (NWU)", true, settings.axesConvention == ThreeDView::AxesConvention::nwu, [&]
    {
        settings.axesConvention = ThreeDView::AxesConvention::nwu;
    });
    menu.addItem("East-North-Up (ENU)", true, settings.axesConvention == ThreeDView::AxesConvention::enu, [&]
    {
        settings.axesConvention = ThreeDView::AxesConvention::enu;
    });
    menu.addItem("North-East-Down (NED)", true, settings.axesConvention == ThreeDView::AxesConvention::ned, [&]
    {
        settings.axesConvention = ThreeDView::AxesConvention::ned;
    });

    return menu;
}

void ThreeDViewWindow::timerCallback()
{
    rollLabel.setVisible(eulerAnglesEnabled);
    pitchLabel.setVisible(eulerAnglesEnabled);
    yawLabel.setVisible(eulerAnglesEnabled);
    rollValue.setVisible(eulerAnglesEnabled);
    pitchValue.setVisible(eulerAnglesEnabled);
    yawValue.setVisible(eulerAnglesEnabled);

    if (eulerAnglesEnabled)
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
    }

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
