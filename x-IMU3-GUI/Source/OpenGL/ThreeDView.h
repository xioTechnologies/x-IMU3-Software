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
        Settings& operator=(const Settings& other);

        std::atomic<float> cameraAzimuth = 45.0f;
        std::atomic<float> cameraElevation = 20.0f;
        std::atomic<float> cameraOrbitDistance = 2.5f;

        std::atomic<bool> worldEnabled = true;
        std::atomic<bool> modelEnabled = true;
        std::atomic<bool> axesEnabled = true;
        std::atomic<bool> compassEnabled = true;
        std::atomic<Model> model { Model::housing };
        std::atomic<AxesConvention> axesConvention { AxesConvention::nwu };
    };

    explicit ThreeDView(GLRenderer& renderer_, const Settings& settings_);

    ~ThreeDView() override;

    void render() override;

    void update(const float x, const float y, const float z, const float w);

    void setCustomModel(const juce::File& file);

    bool isLoading() const;

private:
    GLRenderer& renderer;
    const Settings& settings;
    std::atomic<float> quaternionX { 0.0f }, quaternionY { 0.0f }, quaternionZ { 0.0f }, quaternionW { 1.0f };

    void renderModel(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation, const float modelScale) const;

    void renderWorld(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& axesConventionRotation, const float floorHeight);

    void renderCompass(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float floorHeight);

    void renderAxes(const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const;

    void renderAxesInstance(const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix) const;

    void renderAxesForDeviceOrientation(const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const;

    void renderAxesForWorldOrientation(const glm::mat4& axesConventionRotation) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThreeDView)
};
