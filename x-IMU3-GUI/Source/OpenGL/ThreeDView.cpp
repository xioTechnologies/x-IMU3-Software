#include "../CustomLookAndFeel.h"
#include "Convert.h"
#include "ThreeDView.h"

ThreeDView::Settings& ThreeDView::Settings::operator=(const ThreeDView::Settings& other)
{
    cameraAzimuth = other.cameraAzimuth.load();
    cameraElevation = other.cameraElevation.load();
    cameraOrbitDistance = other.cameraOrbitDistance.load();
    isModelEnabled = other.isModelEnabled.load();
    isWorldEnabled = other.isWorldEnabled.load();
    isAxesEnabled = other.isAxesEnabled.load();
    model = other.model.load();
    axesConvention = other.axesConvention.load();
    return *this;
}

ThreeDView::ThreeDView(GLRenderer& renderer_) : OpenGLComponent(renderer_.getContext()),
                                                renderer(renderer_)
{
    renderer.addComponent(*this);
}

ThreeDView::~ThreeDView()
{
    renderer.removeComponent(*this);
}

void ThreeDView::render()
{
    using namespace ::juce::gl;

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

    if (settings.isModelEnabled)
    {
        renderIMUModel(resources, projectionMatrix, viewMatrix, deviceRotation, axesConventionRotation, modelScale);
    }

    if (settings.isWorldEnabled)
    {
        renderWorldGrid(resources, projectionMatrix, viewMatrix, axesConventionRotation, floorHeight);
    }

    if (settings.isAxesEnabled)
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

void ThreeDView::setSettings(const Settings& settings_)
{
    settings = settings_;
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

void ThreeDView::renderIMUModel(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation, const float modelScale) const
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

void ThreeDView::renderWorldGrid(GLResources& resources, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::mat4& axesConventionRotationGLM, const float floorHeight)
{
    using namespace ::juce::gl;

    glDisable(GL_CULL_FACE); // allow front and back face of grid and compass planes to be seen

    // World Grid - tiles have width/height of 1.0 OpenGL units when `gridTilingFactor` in Grid3D.frag os equivalent to the scale of the grid
    const auto scaleGrid = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
    const auto translateGrid = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, floorHeight, 0.0f));
    resources.grid3DShader.use();
    resources.grid3DShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * translateGrid * scaleGrid * axesConventionRotationGLM);
    resources.plane.render();

    // Compass - will be moved to new HUD element soon
    // Rendered in two plane layers, one above grid, one below grid. We could render only 1 plane and do manual sorting for transparent objects (must draw further objects first), but this code will be replaced soon so not worth the optimization.
    const auto compassOffsetFromGrid = 0.001f;
    const auto compassRotateScale = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.65f));
    const auto compassTopModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorHeight + compassOffsetFromGrid, 0.0f)) * compassRotateScale;
    const auto compassBottomModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorHeight - compassOffsetFromGrid, 0.0f)) * compassRotateScale;
    auto& unlitShader = resources.unlitShader;
    unlitShader.use();
    unlitShader.colour.set(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f)); // tint color to 80% brightness
    unlitShader.isTextured.set(true);
    resources.compassTexture.bind();
    unlitShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * compassTopModelMatrix); // top compass layer above grid
    resources.plane.render();
    unlitShader.modelViewProjectionMatrix.set(projectionMatrix * viewMatrix * compassBottomModelMatrix); // bottom compass layer below grid
    resources.plane.render();
    resources.compassTexture.unbind();

    glEnable(GL_CULL_FACE); // restore cull state
}

void ThreeDView::renderAxes(GLResources& resources, const juce::Rectangle<int>& viewportBounds, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const
{
    auto& text = resources.get3DViewAxisText();
    // TODO: Optimization: Only necessary IF bounds have changed . . .
    // NOTE: This could be moved to GLRenderer, so text scale of whole app is always updated when viewport changes instead of manually updating it per GLComponent.
    text.setScale({ 1.0f / (float) viewportBounds.getWidth(), 1.0f / (float) viewportBounds.getHeight() }); // sets text scale to the normalized size of a screen pixel

    renderAxesWorldSpace(resources, deviceRotation, axesConventionRotation); // attached to model, shows orientation
    renderAxesScreenSpace(resources, axesConventionRotation); // in HUD top right
}

// TODO: We may want to make these axes a constant size in screen pixels, like renderAxesScreenSpace(), so the size does not scale with the viewport. Test this out in practice, this current implementation may actually be better for the average use case.
void ThreeDView::renderAxesWorldSpace(GLResources& resources, const glm::mat4& deviceRotation, const glm::mat4& axesConventionRotation) const
{
    using namespace ::juce::gl;

    const auto& threeDViewShader = resources.threeDViewShader;
    const auto& camera = resources.orbitCamera;

    const auto projectionMatrix = camera.getOrthogonalProjectionMatrix();
    const auto viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * camera.getRotationMatrix(); // axes not affected by zoom, so this is a custom "camera" translation of -1 on the Z axis so the objects become visible and are not clipped out of view
    const auto axesScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f));
    const auto modelMatrixNoScale = axesConventionRotation * deviceRotation;
    const auto modelMatrix = modelMatrixNoScale * axesScaleMatrix;

    // Setup lighting for axes
    threeDViewShader.use();
    threeDViewShader.cameraPosition.set(camera.getPosition());
    threeDViewShader.projectionMatrix.set(projectionMatrix);
    threeDViewShader.isTextured.set(false);
    threeDViewShader.lightColour.set({ 1.0f, 1.0f, 1.0f });
    threeDViewShader.lightPosition.set(glm::vec3(glm::vec4(-4.0f, 8.0f, 8.0f, 1.0f) * camera.getRotationMatrix())); // light positions further away increase darkness of shadows
    threeDViewShader.lightIntensity.set(1.0f);
    threeDViewShader.viewMatrix.set(viewMatrix);

    // X-Axis in x-io coordinate space aligns with OpenGL +X axis
    const auto xModelMatrix = modelMatrix;
    threeDViewShader.materialColour.setRGBA(UIColours::graphRed);
    threeDViewShader.modelMatrix.set(xModelMatrix);
    threeDViewShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(xModelMatrix)));
    resources.arrow.render();

    // Y-Axis in x-io coordinate space aligns with OpenGL -Z axis
    const auto yModelMatrix = modelMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    threeDViewShader.materialColour.setRGBA(UIColours::graphGreen);
    threeDViewShader.modelMatrix.set(yModelMatrix);
    threeDViewShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(yModelMatrix)));
    resources.arrow.render();

    // Z-Axis in x-io coordinate space aligns with OpenGL +Y axis
    const auto zModelMatrix = modelMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    threeDViewShader.materialColour.setRGBA(UIColours::graphBlue);
    threeDViewShader.modelMatrix.set(zModelMatrix);
    threeDViewShader.modelMatrixInverseTranspose.set(glm::mat3(glm::inverseTranspose(zModelMatrix)));
    resources.arrow.render();

    // Text labels XYZ
    // TODO: Maybe Test can be refactored so disable culling is not necessary. I bet winding order on text planes is backwards! Needs to be counter-clockwize to be visible facing user. I bet culprit is GLResources::createTextBuffer, just fix to be correct winding order.
    glDisable(GL_CULL_FACE); // text needs disabled culling

    renderer.getResources().textShader.use();

    // TODO: Right now, because the text does not scale with viewport like the model axes, the text is not always a consistent distance from the arrows. We will likely refactor to have constant sized axes just like the screen space axes.
    const auto textDistanceFromOrigin = 0.37f;
    const auto mvpMatrixNoScale = projectionMatrix * viewMatrix * modelMatrixNoScale;

    auto& text = resources.get3DViewAxisText();

    // X-Axis in x-io coordinate space aligns with OpenGL +X axis
    text.renderScreenSpace(resources, "X", UIColours::graphRed, mvpMatrixNoScale * glm::translate(glm::mat4(1.0f), glm::vec3(textDistanceFromOrigin, 0.0f, 0.0f)) * axesScaleMatrix);

    // Y-Axis in x-io coordinate space aligns with OpenGL -Z axis
    text.renderScreenSpace(resources, "Y", UIColours::graphGreen, mvpMatrixNoScale * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -textDistanceFromOrigin)) * axesScaleMatrix);

    // Z-Axis in x-io coordinate space aligns with OpenGL +Y axis
    text.renderScreenSpace(resources, "Z", UIColours::graphBlue, mvpMatrixNoScale * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, textDistanceFromOrigin, 0.0f)) * axesScaleMatrix);

    glEnable(GL_CULL_FACE); // restore cull state
}

void ThreeDView::renderAxesScreenSpace(GLResources& resources, const glm::mat4& axesConventionRotation) const
{
    using namespace ::juce::gl;

    const auto bounds = toOpenGLBounds(getBoundsInMainWindow()); // already accounts for context.getRenderingScale()
    const auto& screenSpaceShader = resources.screenSpaceLitShader;
    const auto& camera = resources.orbitCamera;

    const auto modelMatrix = axesConventionRotation;
    const auto viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * camera.getRotationMatrix(); // axes not affected by zoom, so this is a custom "camera" translation of -1 on the Z axis so the objects become visible and are not clipped out of view
    const auto projectionMatrix = camera.getOrthogonalProjectionMatrix();

    const double screenPixelScale = context.getRenderingScale();

    // Use Normalized Device Coordinates (NDC) to position in top right corner of viewport
    const glm::vec2 pixelOffsetFromTopRight = glm::vec2(150.0f, 150.0f) * (float) screenPixelScale;
    const glm::vec2 viewportPixelDimensions(bounds.getWidth(), bounds.getHeight());
    const glm::vec2 pixelOffsetNDC = pixelOffsetFromTopRight / viewportPixelDimensions;
    const glm::vec2 topRightNDC(1.0f, 1.0f);
    const auto ndcMat = glm::translate(glm::mat4(1.0f), glm::vec3(topRightNDC - pixelOffsetNDC, 0.0f));

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
    screenSpaceShader.projectionMatrix.set(ndcMat * projectionMatrix);

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
    // TODO: Maybe Test can be refactored so disable culling is not necessary. I bet winding order on text planes is backwards! Needs to be counter-clockwize to be visible facing user. I bet culprit is GLResources::createTextBuffer, just fix to be correct winding order.
    glDisable(GL_CULL_FACE); // text needs disabled culling

    renderer.getResources().textShader.use();

    const auto textDistanceFromOrigin = 1.3f;

    const auto xTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(textDistanceFromOrigin * inverseScreenScale, 0.0f, 0.0f)); // X-Axis in x-io coordinate space aligns with OpenGL +X axis
    const auto yTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -textDistanceFromOrigin * inverseScreenScale)); // Y-Axis in x-io coordinate space aligns with OpenGL -Z axis
    const auto zTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, textDistanceFromOrigin * inverseScreenScale, 0.0f)); // Z-Axis in x-io coordinate space aligns with OpenGL +Y axis

    const auto textTransform = ndcMat * projectionMatrix * viewMatrix * modelMatrix;

    auto& text = resources.get3DViewAxisText();
    text.renderScreenSpace(resources, "X", UIColours::graphRed, textTransform * xTranslate);
    text.renderScreenSpace(resources, "Y", UIColours::graphGreen, textTransform * yTranslate);
    text.renderScreenSpace(resources, "Z", UIColours::graphBlue, textTransform * zTranslate);

    glEnable(GL_CULL_FACE); // restore cull state
}
