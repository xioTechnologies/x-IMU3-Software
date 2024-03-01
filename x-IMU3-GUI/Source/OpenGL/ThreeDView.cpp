#include "CustomLookAndFeel.h"
#include "ThreeDView.h"

ThreeDView::ThreeDView(GLRenderer& renderer_) : OpenGLComponent(renderer_.getContext()), renderer(renderer_)
{
    renderer.addComponent(*this);
}

ThreeDView::~ThreeDView()
{
    renderer.removeComponent(*this);
}

void ThreeDView::render()
{
    std::scoped_lock lock(settingsMutex);

    const auto bounds = toOpenGLBounds(getBoundsInMainWindow());
    auto& camera = renderer.getResources().orbitCamera;

    // Update camera for current component view settings
    camera.setViewportBounds(bounds);
    camera.setOrbit(settings.cameraAzimuth, settings.cameraElevation, settings.cameraOrbitDistance);

    const auto projectionMatrix = camera.getPerspectiveProjectionMatrix();
    const auto viewMatrix = camera.getViewMatrix();

    // Convert device rotation to OpenGL coordinate space where OpenGL +X+Y+Z = Earth +X+Z-Y
    const auto deviceRotation = glm::mat4_cast(glm::quat(quaternionW, quaternionX, quaternionZ, -1.0f * quaternionY));

    // Calculations to ensure model does not pass through floor at any orientation
    const float modelScale = 0.55f;
    const float normalizedModelMaxExtent = 1.0f;
    const float gapBetweenModelAndFloor = 0.1f;
    const float floorHeight = -(normalizedModelMaxExtent + gapBetweenModelAndFloor) * modelScale;

    // Create rotation matrix based on axes convention user setting
    glm::mat4 axesConventionRotation(1.0f);
    switch (settings.axesConvention)
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
    GLHelpers::ScopedCapability scopedCapability(juce::gl::GL_SCISSOR_TEST, true);
    GLHelpers::viewportAndScissor(bounds); // clip drawing to bounds
    juce::OpenGLHelpers::clear(UIColours::backgroundDark);

    const bool renderModelBehindWorldAndCompass = camera.getPosition().y < floorHeight; // depth sorting required by compass
    if (renderModelBehindWorldAndCompass && settings.modelEnabled)
    {
        renderModel(projectionMatrix, viewMatrix, deviceRotation, axesConventionRotation, modelScale);
    }

    if (settings.worldEnabled)
    {
        renderWorld(projectionMatrix, viewMatrix, axesConventionRotation, floorHeight);
    }

    if (settings.compassEnabled)
    {
        renderCompass(projectionMatrix, viewMatrix, floorHeight);
    }

    if (!renderModelBehindWorldAndCompass && settings.modelEnabled)
    {
        renderModel(projectionMatrix, viewMatrix, deviceRotation, axesConventionRotation, modelScale);
    }

    if (settings.axesEnabled && hudEnabled)
    {
        renderAxes(deviceRotation, axesConventionRotation);
    }
}

void ThreeDView::setSettings(Settings settings_)
{
    std::scoped_lock _(settingsMutex);
    settings = settings_;

    renderer.getResources().user.setModel(settings.userModel);
}

ThreeDView::Settings ThreeDView::getSettings() const
{
    std::scoped_lock _(settingsMutex);
    return settings;
}

bool ThreeDView::isLoading() const
{
    switch (settings.model)
    {
        case Model::board:
            return renderer.getResources().board.isLoading();
        case Model::housing:
            return renderer.getResources().housing.isLoading();
        case Model::user:
            return renderer.getResources().user.isLoading();
    }
    return false;
}

void ThreeDView::setHudEnabled(const bool enabled)
{
    hudEnabled = enabled;
}

void ThreeDView::update(const float x, const float y, const float z, const float w)
{
    quaternionX = x;
    quaternionY = y;
    quaternionZ = z;
    quaternionW = w;
}

void ThreeDView::renderModel(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation, const float modelScale) const
{
    const auto& camera = renderer.getResources().orbitCamera;
    const auto rotateModelFlat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });

    const auto modelMatrix = axesConventionRotation * deviceRotation * rotateModelFlat * glm::scale(glm::mat4(1.0f), glm::vec3(modelScale));

    auto& threeDViewShader = renderer.getResources().threeDViewShader;
    threeDViewShader.use();
    threeDViewShader.cameraPosition.set(camera.getPosition());
    threeDViewShader.modelMatrix.set(modelMatrix);
    threeDViewShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(modelMatrix)));
    threeDViewShader.viewMatrix.set(viewMatrix);
    threeDViewShader.projectionMatrix.set(projectionMatrix);
    threeDViewShader.lightColour.set({ 1.0f, 1.0f, 1.0f });
    threeDViewShader.lightPosition.set(glm::vec3(glm::vec4(-4.0f, 8.0f, 8.0f, 1.0f) * camera.getRotationMatrix())); // light positions further away increase darkness of shadows
    threeDViewShader.lightIntensity.set(1.0f);

    switch (settings.model)
    {
        case Model::board:
            renderer.getResources().board.renderWithMaterials(threeDViewShader);
            break;
        case Model::housing:
            renderer.getResources().housing.renderWithMaterials(threeDViewShader);
            break;
        case Model::user:
            renderer.getResources().user.renderWithMaterials(threeDViewShader);
            break;
    }
}

void ThreeDView::renderWorld(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& axesConventionRotation, const float floorHeight) const
{
    GLHelpers::ScopedCapability _(juce::gl::GL_CULL_FACE, false); // allow front and back face of grid to be seen

    // World Grid - tiles have width/height of 1.0 OpenGL units when `gridTilingFactor` in Grid3D.frag os equivalent to the scale of the grid
    const auto scaleGrid = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
    const auto translateGrid = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, floorHeight, 0.0f));
    renderer.getResources().grid3DShader.use();
    renderer.getResources().grid3DShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * translateGrid * scaleGrid * axesConventionRotation);
    renderer.getResources().plane.render();
}

void ThreeDView::renderCompass(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const float floorHeight) const
{
    // Compass is rendered in same plane as world grid, so to prevent z-fighting, disables depth test and performs manual depth sort for model in render()
    GLHelpers::ScopedCapability disableDepthTest(juce::gl::GL_DEPTH_TEST, false); // place compass in front of all other world objects
    GLHelpers::ScopedCapability disableCullFace(juce::gl::GL_CULL_FACE, false); // allow front and back face of compass to be seen

    const auto compassRotateScale = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    const auto compassModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorHeight, 0.0f)) * compassRotateScale;
    auto& unlitShader = renderer.getResources().unlitShader;
    unlitShader.use();
    const auto brightness = 0.8f;
    unlitShader.colour.set(glm::vec4(glm::vec3(brightness), 1.0f)); // tint color to decrease brightness
    unlitShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * compassModelMatrix); // top compass layer above grid
    unlitShader.setTextureImage(renderer.getResources().compassTexture);
    renderer.getResources().plane.render();
    renderer.getResources().compassTexture.unbind();
}

void ThreeDView::renderAxes(const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const
{
    renderAxesForDeviceOrientation(deviceRotation, axesConventionRotation); // attached to model
    renderAxesForWorldOrientation(axesConventionRotation); // in HUD top right
}

void ThreeDView::renderAxesInstance(const glm::mat4& modelMatrix, const glm::mat4& projectionMatrix) const
{
    const auto bounds = toOpenGLBounds(getBoundsInMainWindow()); // already accounts for context.getRenderingScale()
    const auto& screenSpaceShader = renderer.getResources().screenSpaceLitShader;
    const auto& camera = renderer.getResources().orbitCamera;

    const auto viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * camera.getRotationMatrix(); // axes not affected by zoom, so this is a custom "camera" translation of -1 on the Z axis so the objects become visible and are not clipped out of view

    const double screenPixelScale = context.getRenderingScale();

    // Keep object constant size in screen pixels regardless of viewport size by scaling by the inverse of the screen scale
    // Ref: https://community.khronos.org/t/draw-an-object-that-looks-the-same-size-regarles-the-distance-in-perspective-view/67804/6
    const auto objectSizeInPixels = 90.0f * (float) screenPixelScale;
    const auto screenHeightPixels = (float) bounds.getHeight();
    glm::vec4 originInClipSpace = projectionMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // get origin in normalized device coordinates
    const auto normalizedScreenScale = objectSizeInPixels / screenHeightPixels;
    const auto inverseScreenScale = normalizedScreenScale * originInClipSpace.w; // multiply by clip coordinate w to cancel out the division by w done by OpenGL via the viewport transform

    screenSpaceShader.use();
    screenSpaceShader.cameraPosition.set(camera.getPosition());
    screenSpaceShader.lightColour.set({ 1.0f, 1.0f, 1.0f });
    screenSpaceShader.lightPosition.set(glm::vec3(glm::vec4(-4.0f, 8.0f, 8.0f, 1.0f) * camera.getRotationMatrix())); // light positions further away increase darkness of shadows
    screenSpaceShader.lightIntensity.set(1.0f);

    screenSpaceShader.viewMatrix.set(viewMatrix);
    screenSpaceShader.projectionMatrix.set(projectionMatrix);

    // Earth x-axis (aligned with OpenGL +x-axis)
    const auto xRotate = glm::mat4(1.0f);
    const auto xModel = modelMatrix * xRotate;
    screenSpaceShader.materialColour.setRGBA(UIColours::graphX);
    screenSpaceShader.inverseScreenScale.set(inverseScreenScale);
    screenSpaceShader.modelMatrix.set(xModel);
    screenSpaceShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(xModel)));
    renderer.getResources().arrow.render();

    // Earth y-axis (aligned with OpenGL -z-axis)
    const auto yRotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const auto yModel = modelMatrix * yRotate;
    screenSpaceShader.materialColour.setRGBA(UIColours::graphY);
    screenSpaceShader.inverseScreenScale.set(inverseScreenScale);
    screenSpaceShader.modelMatrix.set(yModel);
    screenSpaceShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(yModel)));
    renderer.getResources().arrow.render();

    // Earth z-axis (aligned with OpenGL +y-axis)
    const auto zRotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    const auto zModel = modelMatrix * zRotate;
    screenSpaceShader.materialColour.setRGBA(UIColours::graphZ);
    screenSpaceShader.inverseScreenScale.set(inverseScreenScale);
    screenSpaceShader.modelMatrix.set(zModel);
    screenSpaceShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(zModel)));
    renderer.getResources().arrow.render();

    // Text labels XYZ
    renderer.getResources().textShader.use();

    const auto textDistanceFromOrigin = 1.3f;
    const auto textDistanceFromOriginZ = textDistanceFromOrigin * inverseScreenScale;
    const auto paddingOffset = 0.06f; // account for distance of glyph from render square edge
    const auto textDistanceFromOriginXY = (textDistanceFromOrigin - paddingOffset) * inverseScreenScale;

    const auto xTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(textDistanceFromOriginXY, 0.0f, 0.0f)); // Earth x-axis aligns with OpenGL +x-axis
    const auto yTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -textDistanceFromOriginXY)); // Earth y-axis aligns with OpenGL -z-axis
    const auto zTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, textDistanceFromOriginZ, 0.0f)); // Earth z-axis aligns with OpenGL +y-axis

    const auto textTransform = projectionMatrix * viewMatrix * modelMatrix;

    const auto& text = renderer.getResources().get3DViewAxesText();
    text.drawChar3D(renderer.getResources(), 'X', UIColours::graphX, textTransform * xTranslate, bounds);
    text.drawChar3D(renderer.getResources(), 'Y', UIColours::graphY, textTransform * yTranslate, bounds);
    text.drawChar3D(renderer.getResources(), 'Z', UIColours::graphZ, textTransform * zTranslate, bounds);
}

void ThreeDView::renderAxesForDeviceOrientation(const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const
{
    renderAxesInstance(axesConventionRotation * deviceRotation, renderer.getResources().orbitCamera.getOrthogonalProjectionMatrix());
}

void ThreeDView::renderAxesForWorldOrientation(const glm::mat4& axesConventionRotation) const
{
    const auto bounds = toOpenGLBounds(getBoundsInMainWindow()); // already accounts for context.getRenderingScale()
    const double screenPixelScale = context.getRenderingScale();

    // Use Normalized Device Coordinates (NDC) to position in top right corner of viewport
    const auto pixelOffsetFromTopRight = glm::vec2(148.0f, 152.0f) * (float) screenPixelScale;
    const glm::vec2 viewportPixelDimensions(bounds.getWidth(), bounds.getHeight());
    const auto pixelOffsetNDC = pixelOffsetFromTopRight / viewportPixelDimensions;
    const glm::vec2 topRightNDC(1.0f, 1.0f);
    const auto ndcMat = glm::translate(glm::mat4(1.0f), glm::vec3(topRightNDC - pixelOffsetNDC, 0.0f));

    renderAxesInstance(axesConventionRotation, ndcMat * renderer.getResources().orbitCamera.getOrthogonalProjectionMatrix());
}
