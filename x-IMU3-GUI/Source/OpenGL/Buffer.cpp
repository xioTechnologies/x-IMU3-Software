#include "Buffer.h"

Buffer::Buffer(juce::OpenGLContext& context) : glContext(context)
{
    ebo = 0;
    hasEbo = false;
    totalVertices = 0;
    vbos[vertexBuffer] = 0;
    vbos[colourBuffer] = 0;
    vbos[textureBuffer] = 0;
    vbos[normalBuffer] = 0;
}

Buffer::~Buffer()
{
    if (hasEbo)
    {
        glContext.extensions.glDeleteBuffers(1, &ebo);
    }

    glContext.extensions.glDeleteBuffers(totalBuffers, vbos);
}

void Buffer::create(GLuint totalVertices_, bool hasEbo_)
{
    glContext.extensions.glGenBuffers(totalBuffers, vbos);

    if (hasEbo_)
    {
        glContext.extensions.glGenBuffers(1, &ebo);
    }

    totalVertices = totalVertices_;
    hasEbo = hasEbo_;
}

void Buffer::fillVbo(Buffer::VboType vboType, const GLfloat* data, GLsizeiptr bufferSize, FillType fillType)
{
    glContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbos[vboType]);
    glContext.extensions.glBufferData(juce::gl::GL_ARRAY_BUFFER, bufferSize, data, (GLenum) fillType);
}

void Buffer::fillEbo(const GLuint* data, GLsizeiptr bufferSize, FillType fillType)
{
    glContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ebo);
    glContext.extensions.glBufferData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, bufferSize, data, (GLenum) fillType);
}

void Buffer::appendVbo(Buffer::VboType vboType, const GLfloat* data, GLsizeiptr size, GLuint offset)
{
    glContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbos[vboType]);
    glContext.extensions.glBufferSubData(juce::gl::GL_ARRAY_BUFFER, offset, size, data);
}

void Buffer::appendEbo(const GLuint* data, GLsizeiptr size, GLuint offset)
{
    glContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ebo);
    glContext.extensions.glBufferSubData(juce::gl::GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void Buffer::linkVbo(GLuint attributeID, Buffer::VboType vboType, Buffer::ComponentType componentType, Buffer::DataType dataType)
{
    glContext.extensions.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbos[vboType]);
    glContext.extensions.glVertexAttribPointer(attributeID, componentType, dataType, juce::gl::GL_FALSE, 0, nullptr);
    glContext.extensions.glEnableVertexAttribArray(attributeID);
}

void Buffer::disableAttribute(GLuint attributeID)
{
    glContext.extensions.glDisableVertexAttribArray(attributeID);
}

void Buffer::render(Buffer::DrawType drawType, const int numberOfVertices)
{
    if (hasEbo)
    {
        glContext.extensions.glBindBuffer(juce::gl::GL_ELEMENT_ARRAY_BUFFER, ebo);
        juce::gl::glDrawElements(drawType, numberOfVertices == -1 ? (GLsizei) totalVertices : numberOfVertices, juce::gl::GL_UNSIGNED_INT, nullptr);
    }

    else
    {
        juce::gl::glDrawArrays(drawType, 0, numberOfVertices == -1 ? (GLsizei) totalVertices : numberOfVertices);
    }
}
