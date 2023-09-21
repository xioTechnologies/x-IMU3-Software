#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGL/Shaders/LitShader.h"
#include "OpenGL/ThreeDView/WavefrontObjFile.h"

class GLResources;

class Model
{
public:
    explicit Model(juce::OpenGLContext& context_);

    void render();

    void renderWithMaterials(const LitShader& shader);

    void setModel(const juce::String& objFileContent, const juce::String& mtlFileContent);

    void setModel(const juce::File& objFile);

    bool isLoading() const;

private:
    juce::OpenGLContext& context;

    std::mutex objectLock;
    std::shared_ptr<WavefrontObjFile> object;
    bool fillBuffersPending = false;
    std::atomic<bool> loading { false };

    // A GLBuffer is used per Shape from the WavefrontObjFile to store shape data on the GPU
    struct GLBuffer
    {
        explicit GLBuffer(const WavefrontObjFile::Shape& shape);

        ~GLBuffer();

        void fillBuffers();

        GLuint vao {}; // vertex array object
        GLuint ebo {}; // element buffer object
        GLuint vbo {}; // vertex buffer object

        GLuint indicesSize = 0;

        const WavefrontObjFile::Shape& associatedShape;
    };

    std::vector<std::unique_ptr<GLBuffer>> glBuffers;

    void fillBuffers();

    JUCE_DECLARE_WEAK_REFERENCEABLE(Model)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Model)
};
