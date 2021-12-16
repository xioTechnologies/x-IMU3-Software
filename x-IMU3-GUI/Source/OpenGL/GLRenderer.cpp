#include "../CustomLookAndFeel.h"
#include "GLRenderer.h"

GLRenderer::GLRenderer(juce::Component& attachTo)
{
    context.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    context.setRenderer(this);
    context.setContinuousRepainting(true);
    context.attachTo(attachTo);
}

GLRenderer::~GLRenderer()
{
    context.detach();
}

juce::OpenGLContext& GLRenderer::getContext()
{
    return context;
}

void GLRenderer::addComponent(OpenGLComponent& component)
{
    std::lock_guard<std::mutex> _(componentsLock);
    components.push_back(&component);
}

void GLRenderer::removeComponent(const OpenGLComponent& component)
{
    for (size_t index = 0; index < components.size(); index++)
    {
        if (components[index] == &component)
        {
            std::lock_guard<std::mutex> _(componentsLock);
            components.erase(components.begin() + (int) index);
            return;
        }
    }
}

GLResources& GLRenderer::getResources()
{
    return *resources;
}

void GLRenderer::turnCullingOff()
{
    juce::gl::glDisable(juce::gl::GL_CULL_FACE);
}

void GLRenderer::turnDepthTestOff()
{
    juce::gl::glDisable(juce::gl::GL_DEPTH_TEST);
}

void GLRenderer::refreshScreen(const juce::Colour& colour, const juce::Rectangle<GLint>& viewport)
{
    juce::gl::glEnable(juce::gl::GL_BLEND);
    juce::gl::glEnable(juce::gl::GL_CULL_FACE);
    juce::gl::glEnable(juce::gl::GL_DEPTH_TEST);
    juce::gl::glEnable(juce::gl::GL_SCISSOR_TEST);

    juce::gl::glLineWidth(1.0f);
    juce::gl::glEnable(juce::gl::GL_LINE_SMOOTH);
    juce::gl::glEnable(juce::gl::GL_MULTISAMPLE);

    juce::gl::glCullFace(juce::gl::GL_BACK);
    juce::gl::glFrontFace(juce::gl::GL_CCW);
    juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

    juce::gl::glViewport(viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight());
    juce::gl::glScissor(viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight());
    juce::gl::glClearColor(colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha());
    juce::gl::glClear(juce::gl::GL_COLOR_BUFFER_BIT | juce::gl::GL_DEPTH_BUFFER_BIT);

    juce::gl::glDisable(juce::gl::GL_SCISSOR_TEST);
}

const juce::Matrix3D<GLfloat>& GLRenderer::getProjectionMatrix(juce::Rectangle<GLint> viewport)
{
    const float farClip = 1000.0f;
    const float nearClip = 0.1f;
    const auto aspectRatio = viewport.toFloat().getAspectRatio();
    const auto tanHalfFov = tan(juce::degreesToRadians(30.0f) / 2.0f);

    // Formula to build a 3D perspective view using data above (taken from GLM math library)
    // We are using a column-major matrix implementation here
    projectionMatrix.mat[0] = 1.0f / (aspectRatio * tanHalfFov);
    projectionMatrix.mat[5] = 1.0f / (tanHalfFov);
    projectionMatrix.mat[10] = -(farClip + nearClip) / (farClip - nearClip);
    projectionMatrix.mat[11] = -1.0f;
    projectionMatrix.mat[14] = -(2.0f * farClip * nearClip) / (farClip - nearClip);
    projectionMatrix.mat[15] = 0.0f;

    return projectionMatrix;
}

void GLRenderer::newOpenGLContextCreated()
{
    //All texture, shader, model and buffer resources are created here
    resources = std::make_unique<GLResources>(context);
}

void GLRenderer::renderOpenGL()
{
    //Only clear the screen once before multiple target render calls
    juce::OpenGLHelpers::clear(UIColours::background);

    std::lock_guard<std::mutex> _(componentsLock);
    for (auto& component : components)
    {
        component->render();
    }
}

void GLRenderer::openGLContextClosing()
{
    //All texture, shader, model and buffer resources are freed here
    resources.reset();
}
