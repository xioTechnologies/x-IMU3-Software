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

        std::atomic<bool> isModelEnabled = true;
        std::atomic<bool> isWorldEnabled = true;
        std::atomic<bool> isCompassEnabled = true;
        std::atomic<bool> isAxesEnabled = true;
        std::atomic<Model> model { Model::housing };
        std::atomic<AxesConvention> axesConvention { AxesConvention::nwu };
    };

    explicit ThreeDView(GLRenderer& renderer_);

    ~ThreeDView() override;

    void render() override;

    void update(const float x, const float y, const float z, const float w);

    void setSettings(const Settings& settings_);

    void setCustomModel(const juce::File& file);

    bool isLoading() const;

private:
    GLRenderer& renderer;
    std::atomic<float> quaternionX { 0.0f }, quaternionY { 0.0f }, quaternionZ { 0.0f }, quaternionW { 1.0f };
    Settings settings;

    void renderIMUModel(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation, const float modelScale) const;

    void renderWorldGrid(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& axesConventionRotationGLM, const float floorHeight);

    void renderCompass(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float floorHeight);

    void renderAxes(GLResources& resources, const juce::Rectangle<int>& viewportBounds, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const;

    void renderAxesWorldSpace(GLResources& resources, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const;

    void renderAxesScreenSpace(GLResources& resources, const glm::mat4& axesConventionRotation) const;
};
