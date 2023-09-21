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
    component.resources = resources.get();
    components.push_back(&component);
}

void GLRenderer::removeComponent(OpenGLComponent& component)
{
    std::lock_guard<std::mutex> _(sharedGLDataLock);
    components.erase(std::remove(components.begin(), components.end(), &component), components.end());
    component.resources = nullptr;
}

void GLRenderer::resetDefaultOpenGLState()
{
    // Blend settings
    juce::gl::glEnable(juce::gl::GL_BLEND);
    juce::gl::glBlendFunc(juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA); // alpha value of top fragment determines blend

    // Cull settings
    juce::gl::glEnable(juce::gl::GL_CULL_FACE);
    juce::gl::glCullFace(juce::gl::GL_BACK); // back faces not drawn
    juce::gl::glFrontFace(juce::gl::GL_CCW); // front facing polygons are counter-clockwise (default)

    // Depth settings
    juce::gl::glEnable(juce::gl::GL_DEPTH_TEST);

    // Anti-aliasing & sampling
    juce::gl::glEnable(juce::gl::GL_LINE_SMOOTH);
    juce::gl::glEnable(juce::gl::GL_MULTISAMPLE);
}

void GLRenderer::newOpenGLContextCreated()
{
    std::lock_guard<std::mutex> _(sharedGLDataLock);

    // All texture, shader, model and buffer resources are created here
    resources = std::make_unique<GLResources>(context);

    // If components have already been added, ensure they have valid resources
    for (auto& component : components)
    {
        component->resources = resources.get();
    }
}

void GLRenderer::renderOpenGL()
{
    resetDefaultOpenGLState(); // JUCE paint() modifies OpenGL state, so we must set default state every render loop

    // Clear entire OpenGL screen with the background color and clear depth/stencil buffers
    juce::gl::glDisable(juce::gl::GL_SCISSOR_TEST);
    juce::OpenGLHelpers::clear(UIColours::backgroundDark);

    // Render components
    {
        std::lock_guard<std::mutex> _(sharedGLDataLock);
        for (auto& component : components)
        {
            component->render();
        }
    }

    juce::gl::glDisable(juce::gl::GL_CULL_FACE); // prevent cull interference with JUCE Component paint() via OpenGL
}

void GLRenderer::openGLContextClosing()
{
    {
        std::lock_guard<std::mutex> _(sharedGLDataLock);
        components.clear();
    }

    resources.reset(); // Frees all GL shader, model, texture, buffer resources
}
