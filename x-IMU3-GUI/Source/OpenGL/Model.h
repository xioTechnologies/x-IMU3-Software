#pragma once

#include "Buffer.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Shader.h"
#include "WavefrontObjFile.h"

class GLResources;

class Model
{
public:
    explicit Model(juce::OpenGLContext& context_);

    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);

    void render(const GLResources& resources, bool isTextured);

    void setModel(const juce::String& objFileContent, const juce::String& mtlFileContent);

    void setModel(const juce::File& objFile);

    bool isLoading() const;

private:
    juce::OpenGLContext& context;

    std::mutex objectLock;
    std::shared_ptr<WavefrontObjFile> object;
    bool fillBuffersPending = false;
    std::atomic<bool> loading { false };

    std::vector<std::unique_ptr<Buffer>> buffers;

    void fillBuffers();

    JUCE_DECLARE_WEAK_REFERENCEABLE(Model)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Model)
};
