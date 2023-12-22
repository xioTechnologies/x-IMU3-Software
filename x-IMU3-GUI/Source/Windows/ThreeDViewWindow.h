#pragma once

#include "ApplicationSettings.h"
#include "ConnectionPanel/ConnectionPanel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGL/Common/GLRenderer.h"
#include "OpenGL/ThreeDView.h"
#include "Widgets/PopupMenuHeader.h"
#include "Widgets/SimpleLabel.h"
#include "Window.h"
#include "Ximu3.hpp"

class ThreeDViewWindow : public Window,
                         private juce::Timer
{
public:
    ThreeDViewWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer);

    ~ThreeDViewWindow() override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseDoubleClick(const juce::MouseEvent& mouseEvent) override;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;

private:
    ThreeDView threeDView;

    SimpleLabel rollLabel { "Roll:", UIFonts::getDefaultFont(), juce::Justification::topLeft },
            rollValue { "", UIFonts::getDefaultFont(), juce::Justification::topLeft },
            pitchLabel { "Pitch:", UIFonts::getDefaultFont(), juce::Justification::topLeft },
            pitchValue { "", UIFonts::getDefaultFont(), juce::Justification::topLeft },
            yawLabel { "Yaw:", UIFonts::getDefaultFont(), juce::Justification::topLeft },
            yawValue { "", UIFonts::getDefaultFont(), juce::Justification::topLeft };

    std::atomic<float> roll { 0.0f }, pitch { 0.0f }, yaw { 0.0f };

    SimpleLabel loadingLabel { "", UIFonts::getDefaultFont(), juce::Justification::bottomRight };

    juce::Point<int> lastMousePosition;

    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;
    uint64_t quaternionCallbackID;

    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;
    uint64_t rotationMatrixCallbackID;

    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;
    uint64_t eulerAnglesCallbackID;

    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;
    uint64_t linearAccelerationCallbackID;

    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;
    uint64_t earthAccelerationCallbackID;

    std::function<void(ximu3::XIMU3_AhrsStatusMessage)> ahrsStatusMessageCallback;
    uint64_t ahrsStatusMessageCallbackID;

    bool compactView = false;

    std::atomic<bool> angularRateRecoveryState { false };
    std::atomic<bool> accelerationRecoveryState { false };
    std::atomic<bool> magneticRecoveryState { false };

    Icon angularRateRecoveryIcon { BinaryData::speed_grey_svg, "Angular Rate Recovery" };
    Icon accelerationRecoveryIcon { BinaryData::vibration_grey_svg, "Acceleration Recovery" };
    Icon magneticRecoveryIcon { BinaryData::magnet_grey_svg, "Magnetic Recovery" };

    const juce::File userModelsDirectory = ApplicationSettings::getDirectory().getChildFile("User Models");

    static float wrapAngle(float angle);

    void writeToValueTree(const ThreeDView::Settings& settings);

    ThreeDView::Settings readFromValueTree() const;

    void updateEulerAnglesVisibilities();

    void updateAhrsStatusVisibilities();

    juce::PopupMenu getMenu() override;

    void timerCallback() override;

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeDViewWindow)
};
