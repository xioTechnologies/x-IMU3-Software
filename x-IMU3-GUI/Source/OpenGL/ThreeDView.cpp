#include "../CustomLookAndFeel.h"
#include "Convert.h"
#include "ThreeDView.h"

ThreeDView::Settings& ThreeDView::Settings::operator=(const ThreeDView::Settings& other)
{
    cameraAzimuth = other.cameraAzimuth.load();
    cameraElevation = other.cameraElevation.load();
    cameraOrbitDistance = other.cameraOrbitDistance.load();
    worldEnabled = other.worldEnabled.load();
    modelEnabled = other.modelEnabled.load();
    axesEnabled = other.axesEnabled.load();
    compassEnabled = other.compassEnabled.load();
    model = other.model.load();
    axesConvention = other.axesConvention.load();
    return *this;
}

ThreeDView::ThreeDView(GLRenderer& renderer_, const Settings& settings_) : OpenGLComponent(renderer_.getContext()),
                                                                           renderer(renderer_),
                                                                           settings(settings_)
{
    renderer.addComponent(*this);
}

ThreeDView::~ThreeDView()
{
    renderer.removeComponent(*this);
}

void ThreeDView::render()
{
    const auto bounds = toOpenGLBounds(getBoundsInMainWindow());
    auto& resources = renderer.getResources();
    auto& camera = resources.orbitCamera;

    // Update camera for current component view settings
    camera.setViewportBounds(bounds);
    camera.setOrbit(settings.cameraAzimuth, settings.cameraElevation, settings.cameraOrbitDistance);

    const auto projectionMatrix = camera.getPerspectiveProjectionMatrix();
    const auto viewMatrix = camera.getViewMatrix();

    // Convert device rotation from x-io coordinate space into OpenGL coordinate space where OpenGL X == x-io X, OpenGL Y == x-io Z, OpenGL Z == z-io -Y
    const auto deviceRotation = glm::mat4_cast(glm::quat(quaternionW.load(), quaternionX.load(), quaternionZ.load(), -1.0f * quaternionY.load()));

    // Calculations to ensure model does not pass through floor at any orientation
    const float modelScale = 0.55f;
    const float normalizedModelMaxExtent = 1.0f;
    const float gapBetweenModelAndFloor = 0.1f;
    const float floorHeight = -(normalizedModelMaxExtent + gapBetweenModelAndFloor) * modelScale;

    // Create rotation matrix based on axes convention user setting
    glm::mat4 axesConventionRotation(1.0f);
    switch (settings.axesConvention.load())
    {
        case AxesConvention::nwu:
            break;
        case AxesConvention::enu:
            axesConventionRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case AxesConvention::ned:
            axesConventionRotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;
    }

    // Draw scene
    renderer.refreshScreen(UIColours::backgroundDark, bounds);

    bool renderModelBehindWorldAndCompass = camera.getPosition().y < floorHeight; // depth sorting required by compass
    if (renderModelBehindWorldAndCompass && settings.modelEnabled)
    {
        renderModel(resources, projectionMatrix, viewMatrix, deviceRotation, axesConventionRotation, modelScale);
    }

    if (settings.worldEnabled)
    {
        renderWorld(resources, projectionMatrix, viewMatrix, axesConventionRotation, floorHeight);
    }

    if (settings.compassEnabled)
    {
        renderCompass(resources, projectionMatrix, viewMatrix, floorHeight);
    }

    if (!renderModelBehindWorldAndCompass && settings.modelEnabled)
    {
        renderModel(resources, projectionMatrix, viewMatrix, deviceRotation, axesConventionRotation, modelScale);
    }

    if (settings.axesEnabled)
    {
        renderAxes(resources, bounds, deviceRotation, axesConventionRotation);
    }
}

void ThreeDView::update(const float x, const float y, const float z, const float w)
{
    quaternionX = x;
    quaternionY = y;
    quaternionZ = z;
    quaternionW = w;
}

void ThreeDView::setCustomModel(const juce::File& file)
{
    renderer.getResources().custom.setModel(file);
}

bool ThreeDView::isLoading() const
{
    switch (settings.model.load())
    {
        case Model::board:
            return renderer.getResources().board.isLoading();
        case Model::housing:
            return renderer.getResources().housing.isLoading();
        case Model::custom:
            return renderer.getResources().custom.isLoading();
    }
    return false;
}

void ThreeDView::renderModel(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation, const float modelScale) const
{
    const auto& camera = resources.orbitCamera;
    const auto rotateModelFlat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });

    const auto modelMatrix = axesConventionRotation * deviceRotation * rotateModelFlat * glm::scale(glm::mat4(1.0f), glm::vec3(modelScale));

    auto& threeDViewShader = resources.threeDViewShader;
    threeDViewShader.use();
    threeDViewShader.cameraPosition.set(camera.getPosition());
    threeDViewShader.modelMatrix.set(modelMatrix);
    threeDViewShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(modelMatrix)));
    threeDViewShader.viewMatrix.set(viewMatrix);
    threeDViewShader.projectionMatrix.set(projectionMatrix);
    threeDViewShader.isTextured.set(false);
    threeDViewShader.lightColour.set({ 1.0f, 1.0f, 1.0f });
    threeDViewShader.lightPosition.set(glm::vec3(glm::vec4(-4.0f, 8.0f, 8.0f, 1.0f) * camera.getRotationMatrix())); // light positions further away increase darkness of shadows
    threeDViewShader.lightIntensity.set(1.0f);

    switch (settings.model.load())
    {
        case Model::board:
            resources.board.renderWithMaterials(threeDViewShader);
            break;
        case Model::housing:
            resources.housing.renderWithMaterials(threeDViewShader);
            break;
        case Model::custom:
            resources.custom.renderWithMaterials(threeDViewShader);
            break;
    }
}

void ThreeDView::renderWorld(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& axesConventionRotation, const float floorHeight)
{
    GLUtil::ScopedCapability _(juce::gl::GL_CULL_FACE, false); // allow front and back face of grid to be seen

    // World Grid - tiles have width/height of 1.0 OpenGL units when `gridTilingFactor` in Grid3D.frag os equivalent to the scale of the grid
    const auto scaleGrid = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
    const auto translateGrid = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, floorHeight, 0.0f));
    resources.grid3DShader.use();
    resources.grid3DShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * translateGrid * scaleGrid * axesConventionRotation);
    resources.plane.render();
}

void ThreeDView::renderCompass(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float floorHeight)
{
    // Compass is rendered in same plane as world grid, so to prevent z-fighting, disables depth test and performs manual depth sort for model in render()
    GLUtil::ScopedCapability disableDepthTest(juce::gl::GL_DEPTH_TEST, false); // place compass in front of all other world objects
    GLUtil::ScopedCapability disableCullFace(juce::gl::GL_CULL_FACE, false); // allow front and back face of compass to be seen

    const auto compassRotateScale = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    const auto compassModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorHeight, 0.0f)) * compassRotateScale;
    auto& unlitShader = resources.unlitShader;
    unlitShader.use();
    const auto brightness = 0.8f;
    unlitShader.colour.set(glm::vec4(glm::vec3(brightness), 1.0f)); // tint color to decrease brightness
    unlitShader.isTextured.set(true);
    unlitShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * compassModelMatrix); // top compass layer above grid
    resources.compassTexture.bind();
    resources.plane.render();
    resources.compassTexture.unbind();
}

void ThreeDView::renderAxes(GLResources& resources, const juce::Rectangle<int>& viewportBounds, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const
{
    auto& text = resources.get3DViewAxisText();
    // TODO: Optimization: Only necessary IF bounds have changed . . .
    text.setScale({ 1.0f / (float) viewportBounds.getWidth(), 1.0f / (float) viewportBounds.getHeight() }); // sets text scale to the normalized size of a screen pixel

    renderAxesForDeviceOrientation(resources, deviceRotation, axesConventionRotation); // attached to model
    renderAxesForWorldOrientation(resources, axesConventionRotation); // in HUD top right
}

void ThreeDView::renderAxesInstance(GLResources& resources, const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix) const
{
    const auto bounds = toOpenGLBounds(getBoundsInMainWindow()); // already accounts for context.getRenderingScale()
    const auto& screenSpaceShader = resources.screenSpaceLitShader;
    const auto& camera = resources.orbitCamera;

    const auto viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * camera.getRotationMatrix(); // axes not affected by zoom, so this is a custom "camera" translation of -1 on the Z axis so the objects become visible and are not clipped out of view

    const double screenPixelScale = context.getRenderingScale();

    // Keep object constant size in screen pixels regardless of viewport size by scaling by the inverse of the screen scale
    // Ref: https://community.khronos.org/t/draw-an-object-that-looks-the-same-size-regarles-the-distance-in-perspective-view/67804/6
    const float objectSizeInPixels = 90.0f * (float) screenPixelScale;
    const float screenHeightPixels = (float) bounds.getHeight();
    glm::vec4 originInClipSpace = projectionMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // get origin in normalized device coordinates
    const float normalizedScreenScale = objectSizeInPixels / screenHeightPixels;
    const float inverseScreenScale = normalizedScreenScale * originInClipSpace.w; // multiply by clip coordinate w to cancel out the division by w done by OpenGL via the viewport transform

    screenSpaceShader.use();
    screenSpaceShader.cameraPosition.set(camera.getPosition());
    screenSpaceShader.isTextured.set(false);
    screenSpaceShader.lightColour.set({ 1.0f, 1.0f, 1.0f });
    screenSpaceShader.lightPosition.set(glm::vec3(glm::vec4(-4.0f, 8.0f, 8.0f, 1.0f) * camera.getRotationMatrix())); // light positions further away increase darkness of shadows
    screenSpaceShader.lightIntensity.set(1.0f);

    screenSpaceShader.viewMatrix.set(viewMatrix);
    screenSpaceShader.projectionMatrix.set(projectionMatrix);

    // X-Axis in x-io coordinate space aligns with OpenGL +X axis
    const glm::mat4 xRotate = glm::mat4(1.0f);
    const glm::mat4 xModel = modelMatrix * xRotate;
    screenSpaceShader.materialColour.setRGBA(UIColours::graphRed);
    screenSpaceShader.inverseScreenScale.set(inverseScreenScale);
    screenSpaceShader.modelMatrix.set(xModel);
    screenSpaceShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(xModel)));
    resources.arrow.render();

    // Y-Axis in x-io coordinate space aligns with OpenGL -Z axis
    const glm::mat4 yRotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 yModel = modelMatrix * yRotate;
    screenSpaceShader.materialColour.setRGBA(UIColours::graphGreen);
    screenSpaceShader.inverseScreenScale.set(inverseScreenScale);
    screenSpaceShader.modelMatrix.set(yModel);
    screenSpaceShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(yModel)));
    resources.arrow.render();

    // Z-Axis in x-io coordinate space aligns with OpenGL +Y axis
    const glm::mat4 zRotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 zModel = modelMatrix * zRotate;
    screenSpaceShader.materialColour.setRGBA(UIColours::graphBlue);
    screenSpaceShader.inverseScreenScale.set(inverseScreenScale);
    screenSpaceShader.modelMatrix.set(zModel);
    screenSpaceShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(zModel)));
    resources.arrow.render();

    // Text labels XYZ
    {
        GLUtil::ScopedCapability _(juce::gl::GL_CULL_FACE, false); // TODO: why does Text need culling disabled?

        renderer.getResources().textShader.use();

        const auto textDistanceFromOrigin = 1.3f;
        const auto textDistanceFromOriginZ = textDistanceFromOrigin * inverseScreenScale;
        const auto paddingOffset = 0.06f; // account for distance of glyph from render square edge
        const auto textDistanceFromOriginXY = (textDistanceFromOrigin - paddingOffset) * inverseScreenScale;

        const auto xTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(textDistanceFromOriginXY, 0.0f, 0.0f)); // X-Axis in x-io coordinate space aligns with OpenGL +X axis
        const auto yTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -textDistanceFromOriginXY)); // Y-Axis in x-io coordinate space aligns with OpenGL -Z axis
        const auto zTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, textDistanceFromOriginZ, 0.0f)); // Z-Axis in x-io coordinate space aligns with OpenGL +Y axis

        const auto textTransform = projectionMatrix * viewMatrix * modelMatrix;

        auto& text = resources.get3DViewAxisText();
        text.renderScreenSpace(resources, "X", UIColours::graphRed, textTransform * xTranslate);
        text.renderScreenSpace(resources, "Y", UIColours::graphGreen, textTransform * yTranslate);
        text.renderScreenSpace(resources, "Z", UIColours::graphBlue, textTransform * zTranslate);
    }
}

void ThreeDView::renderAxesForDeviceOrientation(GLResources& resources, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const
{
    renderAxesInstance(resources, axesConventionRotation * deviceRotation, resources.orbitCamera.getOrthogonalProjectionMatrix());
}

void ThreeDView::renderAxesForWorldOrientation(GLResources& resources, const glm::mat4& axesConventionRotation) const
{
    const auto bounds = toOpenGLBounds(getBoundsInMainWindow()); // already accounts for context.getRenderingScale()
    const double screenPixelScale = context.getRenderingScale();

    // Use Normalized Device Coordinates (NDC) to position in top right corner of viewport
    const glm::vec2 pixelOffsetFromTopRight = glm::vec2(148.0f, 152.0f) * (float) screenPixelScale;
    const glm::vec2 viewportPixelDimensions(bounds.getWidth(), bounds.getHeight());
    const glm::vec2 pixelOffsetNDC = pixelOffsetFromTopRight / viewportPixelDimensions;
    const glm::vec2 topRightNDC(1.0f, 1.0f);
    const auto ndcMat = glm::translate(glm::mat4(1.0f), glm::vec3(topRightNDC - pixelOffsetNDC, 0.0f));

    renderAxesInstance(resources, axesConventionRotation, ndcMat * resources.orbitCamera.getOrthogonalProjectionMatrix());
}
