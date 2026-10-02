#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include <atomic>
#include "OpenGL/Shaders/LitShader.h"
#include "OpenGL/ThreeDView/WavefrontObjFile.h"

class Model {
public:
    Model(juce::ThreadPool &threadPool, const juce::String &objFileContent, const juce::String &mtlFileContent);

    Model(juce::ThreadPool &threadPool, const juce::File &objFile);

    void render();

    void renderWithMaterials(const LitShader &shader);

    bool isLoading() const;

private:
    struct State {
        std::unique_ptr<WavefrontObjFile> object;
        std::atomic<bool> loaded{false};
    };

    const std::shared_ptr<State> state = std::make_shared<State>();

    // A GLBuffer is used per Shape from the WavefrontObjFile to store shape data on the GPU
    struct GLBuffer {
        explicit GLBuffer(const WavefrontObjFile::Shape &shape);

        ~GLBuffer();

        void fillBuffers();

        GLuint vao{}; // vertex array object
        GLuint ebo{}; // element buffer object
        GLuint vbo{}; // vertex buffer object

        GLuint indicesSize = 0;

        const WavefrontObjFile::Shape &associatedShape;
    };

    std::vector<std::unique_ptr<GLBuffer> > glBuffers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Model)
};
