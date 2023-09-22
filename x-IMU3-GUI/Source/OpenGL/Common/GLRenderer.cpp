#include "CustomLookAndFeel.h"
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

        /*  As a possible future optimization, OpenGL instancing (batch rendering) could be implemented for some ThreeDView and Graph rendering.
         *  Instancing improves performance of CPU to GPU communication by rendering similar GL data in a single draw command instead of issuing a draw command per-instance.
         *
         *  The text drawn in Graph by OpenGL may be the easiest candidate for an instancing optimization. Instead of issuing a draw
         *  command for each text string, all OpenGL text strings on screen could be drawn in a single draw command to the GPU. To
         *  implement this, we would need to add a global queue of text data to GLRenderer. Instead of drawing Text on its own, each
         *  Graph component would add data to this queue in it's render() function to describe how it wants its text rendered. Then
         *  after all OpenGL components have completed rendering, GLRender would empty the queue of text data and send it off to the
         *  GPU in one draw command. This queue empty and draw command would happen here, immediately after looping through all OpenGL
         *  components.
         *
         *  Ref: https://learnopengl.com/Advanced-OpenGL/Instancing
         */
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
