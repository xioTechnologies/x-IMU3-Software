#pragma once

#include <juce_opengl/juce_opengl.h>
#include <span>

class LineBuffer
{
public:
    explicit LineBuffer(bool includeBrightnessAttribute_) : includeBrightnessAttribute(includeBrightnessAttribute_)
    {
        using namespace ::juce::gl;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }

    ~LineBuffer()
    {
        using namespace ::juce::gl;
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    void fillBuffers(const std::span<const juce::Point<GLfloat>>& vertices)
    {
        static constexpr int floatsInPoint = 2;
        static_assert(sizeof(juce::Point<GLfloat>) == sizeof(float) * floatsInPoint); // if this fails, use contiguous raw float sequence instead
        fillBuffers({ reinterpret_cast<const GLfloat*> (vertices.data()), vertices.size() * floatsInPoint });
    }

    void fillBuffers(const std::span<const GLfloat>& vertices)
    {
        using namespace ::juce::gl;

        // Define that our vertices are laid out as groups of 3 GLfloats (2 for position (XY), 1 for brightness)
        const GLsizei positionDimension = 2;
        const GLsizei brightnessDimension = includeBrightnessAttribute ? 1 : 0;
        const GLsizei totalDimensions = positionDimension + brightnessDimension;
        const GLsizei vertexDataLength = totalDimensions * (GLsizei) sizeof(GLfloat);

        verticesCount = (GLsizei) vertices.size() / totalDimensions;

        glBindVertexArray(vao); // bind VAO to cache all VBO and vertex attribute state

        // Fill VBO buffer with vertices array
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(GLfloat) * vertices.size()), vertices.data(), GL_DYNAMIC_DRAW);

        // Position attribute (2 floats)
        const GLuint positionIndex = 0;
        glVertexAttribPointer(positionIndex, positionDimension, GL_FLOAT, GL_FALSE, vertexDataLength, nullptr);
        glEnableVertexAttribArray(positionIndex);

        if (includeBrightnessAttribute)
        {
            // Brightness attribute (1 float)
            const GLuint brightnessIndex = 1;
            glVertexAttribPointer(brightnessIndex, brightnessDimension, GL_FLOAT, GL_FALSE, vertexDataLength, (void*) (positionDimension * sizeof(GLfloat)));
            glEnableVertexAttribArray(brightnessIndex);
        }

        // Unbind buffers
        glBindVertexArray(0); // unbind VAO first since it remembers last bound VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    }

    void draw(GLenum glDrawMode) const
    {
        using namespace ::juce::gl;
        glBindVertexArray(vao);
        glDrawArrays(glDrawMode, 0, verticesCount);
        glBindVertexArray(0);
    }

private:
    GLuint vao {}; // vertex array object
    GLuint vbo {}; // vertex buffer object

    GLsizei verticesCount = 0;

    bool includeBrightnessAttribute = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LineBuffer)
};
