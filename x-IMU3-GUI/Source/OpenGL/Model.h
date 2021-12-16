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

    ~Model();

    void setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);

    void render(const GLResources& resources, bool isTextured);

    void setModel(const juce::String& objFileContent, const juce::String& mtlFileContent);

    void setModel(const juce::File& objFile);

    bool isLoading() const;

private:
    class ObjectLoader;

    juce::OpenGLContext& context;

    std::unique_ptr<WavefrontObjFile> object;
    std::vector<std::unique_ptr<Buffer>> buffers;

    mutable std::mutex objectLoaderLock;
    std::unique_ptr<ObjectLoader> objectLoader;

    void fillBuffers();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Model)
};
