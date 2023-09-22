#pragma once

#include "Common/GLRenderer.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGLComponent.h"

class ThreeDView : public OpenGLComponent
{
public:
    enum class Model
    {
        board,
        housing,
        custom,
    };

    enum class AxesConvention
    {
        nwu,
        enu,
        ned,
    };

    struct Settings
    {
        float cameraAzimuth = 45.0f;
        float cameraElevation = 20.0f;
        float cameraOrbitDistance = 2.5f;

        bool worldEnabled = true;
        bool modelEnabled = true;
        bool axesEnabled = true;
        bool compassEnabled = true;
        Model model { Model::housing };
        AxesConvention axesConvention { AxesConvention::nwu };
    };

    explicit ThreeDView(GLRenderer& renderer_);

    ~ThreeDView() override;

    void render() override;

    void setSettings(Settings settings_);

    Settings getSettings() const;

    void setCustomModel(const juce::File& file);

    bool isLoading() const;

    void setHudEnabled(const bool enabled);

    void update(const float x, const float y, const float z, const float w);

private:
    GLRenderer& renderer;

    mutable std::mutex settingsMutex;
    Settings settings;

    std::atomic<float> quaternionX { 0.0f }, quaternionY { 0.0f }, quaternionZ { 0.0f }, quaternionW { 1.0f };

    std::atomic<bool> hudEnabled { true };

    void renderModel(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation, const float modelScale) const;

    void renderWorld(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& axesConventionRotation, const float floorHeight) const;

    void renderCompass(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float floorHeight) const;

    void renderAxes(const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const;

    void renderAxesInstance(const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix) const;

    void renderAxesForDeviceOrientation(const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const;

    void renderAxesForWorldOrientation(const glm::mat4& axesConventionRotation) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeDView)
};
