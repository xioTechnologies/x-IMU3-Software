#pragma once

#include "GLRenderer.h"
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

    struct Settings
    {
        Settings& operator=(const Settings& other);

        std::atomic<float> azimuth = -45.0f;
        std::atomic<float> elevation = -20.0f;
        std::atomic<float> zoom = -2.5f;
        std::atomic<bool> isModelEnabled = true;
        std::atomic<bool> isStageEnabled = true;
        std::atomic<bool> isAxesEnabled = true;
        std::atomic<Model> model { Model::housing };
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

    juce::Matrix3D<GLfloat> rotation(const float roll, const float pitch, const float yaw);

    juce::Matrix3D<GLfloat> translation(const float x, const float y, const float z);

    juce::Matrix3D<GLfloat> scale(const float value);
};
