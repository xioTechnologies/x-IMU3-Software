#pragma once

#include "../DevicePanel/DevicePanel.h"
#include "../OpenGL/Common/GLRenderer.h"
#include "../OpenGL/ThreeDView.h"
#include "../Widgets/PopupMenuHeader.h"
#include "../Widgets/SimpleLabel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Window.h"
#include "Ximu3.hpp"

class ThreeDViewWindow : public Window,
                         private juce::Timer
{
public:
    ThreeDViewWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~ThreeDViewWindow() override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;

private:
    ThreeDView threeDView;
    static ThreeDView::Settings settings;
    static bool eulerAnglesEnabled;

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

    juce::PopupMenu getMenu();

    void timerCallback() override;

    static float wrapAngle(float angle);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeDViewWindow)
};
