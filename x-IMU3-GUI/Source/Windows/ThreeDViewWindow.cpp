#include "../DevicePanelContainer.h"
#include "Helpers.h"
#include "ThreeDViewWindow.h"

ThreeDViewWindow::ThreeDViewWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : Window(windowLayout, type, devicePanel_),
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

        const auto eulerAngles = Helpers::ToEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);

        roll = eulerAngles.x;
        pitch = eulerAngles.y;
        yaw = eulerAngles.z;
    });

    rotationMatrixCallbackID = devicePanel.getConnection().addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)
    {
        const auto quaternion = Helpers::ToQuaternion(message.xx_element, message.xy_element, message.xz_element,
                                                      message.yx_element, message.yy_element, message.yz_element,
                                                      message.zx_element, message.zy_element, message.zz_element);

        quaternionCallback({ message.timestamp, quaternion.scalar, quaternion.vector.x, quaternion.vector.y, quaternion.vector.z });
    });

    eulerAnglesCallbackID = devicePanel.getConnection().addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)
    {
        const auto quaternion = Helpers::ToQuaternion(message.roll, message.pitch, message.yaw);

        threeDView.update(-1.0f * quaternion.vector.x, -1.0f * quaternion.vector.y, -1.0f * quaternion.vector.z, quaternion.scalar);

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
    if (mouseEvent.mods.isPopupMenu())
    {
        juce::PopupMenu menu;

        menu.addItem("Euler Angles", true, rollLabel.isVisible(), [&]
        {
            const auto isVisible = !rollLabel.isVisible();
            rollLabel.setVisible(isVisible);
            pitchLabel.setVisible(isVisible);
            yawLabel.setVisible(isVisible);
            rollValue.setVisible(isVisible);
            pitchValue.setVisible(isVisible);
            yawValue.setVisible(isVisible);
        });
        menu.addItem("Model", true, settings.isModelEnabled, [&]
        {
            settings.isModelEnabled = !settings.isModelEnabled;
            threeDView.setSettings(settings);
        });
        menu.addItem("Stage", true, settings.isStageEnabled, [&]
        {
            settings.isStageEnabled = !settings.isStageEnabled;
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

        menu.showMenuAsync({});
    }

    lastMousePosition = mouseEvent.getPosition();
}

void ThreeDViewWindow::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay() != nullptr)
    {
        return;
    }

    const auto scale = -0.5f;

    settings.azimuth = wrapAngle(settings.azimuth + (scale * (mouseEvent.getPosition().getX() - lastMousePosition.getX())));
    settings.elevation = wrapAngle(settings.elevation + (scale * (mouseEvent.getPosition().getY() - lastMousePosition.getY())));
    threeDView.setSettings(settings);

    lastMousePosition = mouseEvent.getPosition();
}

void ThreeDViewWindow::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    settings.zoom = juce::jlimit(-4.0f, -1.0f, settings.zoom + (0.5f * wheel.deltaY));
    threeDView.setSettings(settings);
}

void ThreeDViewWindow::timerCallback()
{
    rollValue.setText(juce::String(roll.load(), 1) + "°");
    pitchValue.setText(juce::String(pitch.load(), 1) + "°");
    yawValue.setText(juce::String(yaw.load(), 1) + "°");
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
