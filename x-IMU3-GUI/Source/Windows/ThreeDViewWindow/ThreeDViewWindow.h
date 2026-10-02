#pragma once

#include "../Window.h"
#include "ConnectionPanel/ConnectionPanel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Models.h"
#include "OpenGL/Common/OpenGLRenderer.h"
#include "OpenGL/ThreeDView.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ThreeDViewWindow : public Window {
public:
    ThreeDViewWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_, OpenGLRenderer &openGLRenderer);

    ~ThreeDViewWindow() override;

    void resized() override;

    void mouseDown(const juce::MouseEvent &mouseEvent) override;

    void mouseDrag(const juce::MouseEvent &mouseEvent) override;

    void mouseDoubleClick(const juce::MouseEvent &mouseEvent) override;

    void mouseWheelMove(const juce::MouseEvent &, const juce::MouseWheelDetails &wheel) override;

private:
    ThreeDView threeDView;

    SimpleLabel rollLabel{"Roll:", UIFonts::getDefaultFont(), juce::Justification::topLeft},
            rollValue{"", UIFonts::getDefaultFont(), juce::Justification::topLeft},
            pitchLabel{"Pitch:", UIFonts::getDefaultFont(), juce::Justification::topLeft},
            pitchValue{"", UIFonts::getDefaultFont(), juce::Justification::topLeft},
            yawLabel{"Yaw:", UIFonts::getDefaultFont(), juce::Justification::topLeft},
            yawValue{"", UIFonts::getDefaultFont(), juce::Justification::topLeft};
    std::atomic<float> roll{0.0f}, pitch{0.0f}, yaw{0.0f};
    juce::TimedCallback rollPitchYawTimer{
        [&] {
            const auto formatAngle = [](const float angle) {
                auto text = juce::String(angle, 1);

                if (text == "-0.0") {
                    text = "0.0";
                } else if (text == "-180.0") {
                    text = "180.0";
                }

                return text + "°";
            };
            rollValue.setText(formatAngle(roll));
            pitchValue.setText(formatAngle(pitch));
            yawValue.setText(formatAngle(yaw));
        }
    };

    SimpleLabel infoLabel{"", UIFonts::getDefaultFont(), juce::Justification::bottomRight};
    juce::TimedCallback loadingTimer{
        [&] {
            if (threeDView.isLoading() == false) {
                infoLabel.setText("");
                loadingTimer.stopTimer();
            }
        }
    };

    juce::Point<int> lastMousePosition;

    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;
    uint64_t quaternionCallbackId;

    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;
    uint64_t rotationMatrixCallbackId;

    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;
    uint64_t eulerAnglesCallbackId;

    std::function<void(ximu3::XIMU3_AhrsStatusMessage)> ahrsStatusMessageCallback;
    uint64_t ahrsStatusMessageCallbackId;

    std::function<void(ximu3::XIMU3_PingResponse)> pingCallback;
    uint64_t pingCallbackId;

    bool compactView = false;

    SimpleLabel ahrsStatusLabel { "", UIFonts::getDefaultFont(), juce::Justification::centredTop };
    juce::TimedCallback ahrsStatusLabelTimer{
        [&] {
            ahrsStatusLabel.setText("");
            ahrsStatusLabelTimer.stopTimer();
        }
    };

    SimpleLabel axesConventionLabel{"", UIFonts::getDefaultFont(), juce::Justification::topRight};

    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::String currentModel;

    static float wrapAngle(float angle);

    static juce::String toString(const ThreeDView::AxesConvention axesConvention);

    void writeToValueTree(const ThreeDView::Settings &settings);

    ThreeDView::Settings readFromValueTree() const;

    void updateModel();

    void updateEulerAnglesVisibilities();

    void updateAhrsStatusVisibility();

    void updateAxesConventionLabel();

    juce::PopupMenu getMenu() override;

    void valueTreePropertyChanged(juce::ValueTree &, const juce::Identifier &property) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeDViewWindow)
};
