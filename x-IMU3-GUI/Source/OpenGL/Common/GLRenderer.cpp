#include "../../CustomLookAndFeel.h"
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
    std::lock_guard<std::mutex> _(sharedGLDataLock);
    components.push_back(&component);
}

void GLRenderer::removeComponent(OpenGLComponent& component)
{
    std::lock_guard<std::mutex> _(sharedGLDataLock);
    components.erase(std::remove(components.begin(), components.end(), &component), components.end());
}

GLResources& GLRenderer::getResources()
{
    return *resources;
}

void GLRenderer::refreshScreen(const juce::Colour& colour, const juce::Rectangle<GLint>& viewport)
{
    // TODO: Remove the majority of these settings but keep scissor test, viewport, and clear color.
    // Move some of these settings to beginning of GLRender::render() and some inverses of these settings
    // to the end of GLRender::render() so GL state is ready for JUCE Component GL rendering which happens
    // immediately after.
    // NOTE: We will also want to run scissor test per GLComponent. Each GL component should ONLY draw
    // in its viewport, so glDisable(GL_SCISSOR_TEST) must happen AFTER an individual GL component has
    // rendered itself. Or we could just put it at the end of the GLRenderer::render() loop, making sure
    // we call glScissor for each GL component.
    // Might also be advantageous to do a non-scissored glClear to the entire viewport at the start of
    // GLRenderer::render() so we do not need to do individual clears. This would be most convenient if
    // the background colors of all GL components were the same, but if not, this may not be necessary

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

void GLRenderer::newOpenGLContextCreated()
{
    //All texture, shader, model and buffer resources are created here
    resources = std::make_unique<GLResources>(context);
}

void GLRenderer::renderOpenGL()
{
    std::lock_guard<std::mutex> _(sharedGLDataLock);

    // TODO: This may not be necessary, could only clear per GLComponent viewport
    // Clear the entire OpenGL screen with the background color
    juce::OpenGLHelpers::clear(UIColours::backgroundDark);

    // Render components
    for (auto& component : components)
    {
        component->render();
    }

    // Prevent cull interference with JUCE Component paint() via OpenGL regardless of the last used cull settings of any of the OpenGL components
    juce::gl::glDisable(juce::gl::GL_CULL_FACE);
}

void GLRenderer::openGLContextClosing()
{
    {
        std::lock_guard<std::mutex> _(sharedGLDataLock);
        components.clear();
    }

    resources.reset(); // Frees all GL shader, model, texture, buffer resources
}
