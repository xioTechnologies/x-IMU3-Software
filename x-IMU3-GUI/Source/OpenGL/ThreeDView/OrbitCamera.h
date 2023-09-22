#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

// 3D camera that orbits the world origin (0, 0, 0)
class OrbitCamera
{
public:
    OrbitCamera()
    {
    }

    // Updates projectionMatrix from view size
    void setViewportBounds(juce::Rectangle<GLint> viewport_)
    {
        setViewportAndAspectRatio(viewport_);
        updateProjectionMatrices();
    }

    // Updates viewMatrix & rotationMatrix from orbit parameters
    void setOrbit(float azimuthDegrees_, float altitudeDegrees_, float distanceFromOrigin_)
    {
        azimuthDegrees = azimuthDegrees_;
        altitudeDegrees = altitudeDegrees_;
        distanceFromOrigin = glm::abs(distanceFromOrigin_); // Only use positive distance

        const auto rotateAzimuth = glm::rotate(glm::mat4 { 1.0f }, glm::radians(azimuthDegrees), glm::vec3 { 0.0f, 1.0f, 0.0f });
        const auto rotateAltitude = glm::rotate(glm::mat4 { 1.0f }, glm::radians(altitudeDegrees), glm::vec3 { 1.0f, 0.0f, 0.0f });
        rotationMatrix = rotateAltitude * rotateAzimuth;

        const auto cameraStartPosition = glm::vec3(0.0f, 0.0f, distanceFromOrigin);
        cameraPosition = glm::vec3(glm::vec4(cameraStartPosition, 1.0f) * rotationMatrix);

        const auto translation = glm::translate(glm::mat4 { 1.0f }, -cameraStartPosition);

        viewMatrix = translation * rotationMatrix;
    }

    glm::mat4 getViewMatrix() const
    {
        return viewMatrix;
    }

    glm::mat4 getRotationMatrix() const
    {
        return rotationMatrix;
    }

    glm::vec3 getPosition() const
    {
        return cameraPosition;
    }

    glm::mat4 getPerspectiveProjectionMatrix() const
    {
        return perspectiveProjectionMatrix;
    }

    glm::mat4 getOrthogonalProjectionMatrix() const
    {
        return orthogonalProjectionMatrix;
    }

    glm::mat4 getProjectionMatrixForViewport(juce::Rectangle<GLint> viewport_)
    {
        setViewportBounds(viewport_);
        return perspectiveProjectionMatrix;
    }

    float getDistanceFromOrigin() const
    {
        return distanceFromOrigin;
    }

    float getAspectRatio() const
    {
        return aspectRatio;
    }

    float fieldOfViewDegrees = 30.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;

private:

    void setViewportAndAspectRatio(const juce::Rectangle<GLint> viewport_)
    {
        viewport = viewport_;
        // Prevent divide by 0 to calculate aspect ratio
        if (viewport.getHeight() == 0)
        {
            viewport.setHeight(1);
        }
        aspectRatio = viewport.toFloat().getAspectRatio(true);
    }

    void updateProjectionMatrices()
    {
        perspectiveProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDegrees), aspectRatio, nearClip, farClip);

        const auto scale = 1.0f; // orthogonal view of camera could optionally be scaled (zoomed) with this value, but it is recommended to use a view matrix for this instead, ref: https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_Examples.html
        orthogonalProjectionMatrix = glm::ortho(-scale * aspectRatio, scale * aspectRatio, -scale, scale, nearClip, farClip);
    }

    glm::mat4 perspectiveProjectionMatrix { 1.0f };
    glm::mat4 orthogonalProjectionMatrix { 1.0f };
    juce::Rectangle<GLint> viewport;
    float aspectRatio = 4.0f / 3.0f;

    glm::vec3 cameraPosition = { 0.0f, 0.0f, 0.0f };
    glm::mat4 viewMatrix { 1.0f };
    glm::mat4 rotationMatrix { 1.0f }; // only rotation part of viewMatrix

    float distanceFromOrigin = 2.5f;
    float azimuthDegrees = 45.0f; // horizontal movement
    float altitudeDegrees = 20.0f; // vertical movement

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrbitCamera)
};
