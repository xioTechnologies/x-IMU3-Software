#include "Buffer.h"

using namespace ::juce::gl;

Buffer::Buffer()
{
    vao = 0;
    ebo = 0;
    hasEbo = false;
    totalVertices = 0;
    vbos[vertexBuffer] = 0;
    vbos[textureBuffer] = 0;
    vbos[normalBuffer] = 0;
}

Buffer::~Buffer()
{
    glDeleteVertexArrays(1, &vao);

    if (hasEbo)
    {
        glDeleteBuffers(1, &ebo);
    }

    glDeleteBuffers(totalBuffers, vbos);
}

void Buffer::create(GLuint totalVertices_, bool hasEbo_)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(totalBuffers, vbos);

    if (hasEbo_)
    {
        glGenBuffers(1, &ebo);
    }

    totalVertices = totalVertices_;
    hasEbo = hasEbo_;
}

void Buffer::fillVbo(Buffer::VboType vboType, const GLfloat* data, GLsizeiptr bufferSize, FillType fillType)
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vboType]);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, data, (GLenum) fillType);

    glBindVertexArray(0); // unbind VAO
}

void Buffer::fillEbo(const GLuint* data, GLsizeiptr bufferSize, FillType fillType)
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, data, (GLenum) fillType);

    glBindVertexArray(0); // unbind VAO
}

void Buffer::appendVbo(Buffer::VboType vboType, const GLfloat* data, GLsizeiptr size, GLuint offset)
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vboType]);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

    glBindVertexArray(0); // unbind VAO
}

void Buffer::appendEbo(const GLuint* data, GLsizeiptr size, GLuint offset)
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);

    glBindVertexArray(0); // unbind VAO
}

void Buffer::linkVbo(GLuint attributeID, Buffer::VboType vboType, Buffer::ComponentType componentType, Buffer::DataType dataType)
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[vboType]);
    glVertexAttribPointer(attributeID, componentType, dataType, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attributeID);

    glBindVertexArray(0); // unbind VAO
}

void Buffer::disableAttribute(GLuint attributeID)
{
    glBindVertexArray(vao);

    glDisableVertexAttribArray(attributeID);

    glBindVertexArray(0); // unbind VAO
}

void Buffer::render(Buffer::DrawType drawType, const int numberOfVertices)
{
    glBindVertexArray(vao);

    if (hasEbo)
    {
        glDrawElements(drawType, numberOfVertices == -1 ? (GLsizei) totalVertices : (GLsizei) numberOfVertices, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(drawType, 0, numberOfVertices == -1 ? (GLsizei) totalVertices : (GLsizei) numberOfVertices);
    }

    glBindVertexArray(0); // unbind VAO
}
