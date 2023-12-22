#include "Model.h"
#include "OpenGL/Common/GLResources.h"

Model::Model(juce::OpenGLContext& context_) : context(context_)
{
}

void Model::render()
{
    // Initialize buffers if they have not yet been initialized
    std::lock_guard _(objectLock);
    if (object != nullptr && fillBuffersPending)
    {
        fillBuffers();
        fillBuffersPending = false;
    }

    // Render all OpenGL buffers
    for (const auto& glBuffer : glBuffers)
    {
        using namespace ::juce::gl;
        glBindVertexArray(glBuffer->vao); // Bind VAO for all vertex data (VBO, EBO, attributes)
        glDrawElements(GL_TRIANGLES, (GLsizei) glBuffer->indicesSize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0); // unbind VAO to prevent modification to the VAO by subsequent GL calls
    }
}

void Model::renderWithMaterials(const LitShader& shader)
{
    // Initialize buffers if they have not yet been initialized
    std::lock_guard _(objectLock);
    if (object != nullptr && fillBuffersPending)
    {
        fillBuffers();
        fillBuffersPending = false;
    }

    // Render all OpenGL buffers
    for (const auto& glBuffer : glBuffers)
    {
        using namespace ::juce::gl;

        const auto& material = glBuffer->associatedShape.material;
        shader.materialColour.set(glm::vec4(material.diffuse, 1.0f));

        glBindVertexArray(glBuffer->vao); // bind VAO for all vertex data (VBO, EBO, attributes)
        glDrawElements(GL_TRIANGLES, (GLsizei) glBuffer->indicesSize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0); // unbind VAO to prevent modification to the VAO by subsequent GL calls
    }
}

void Model::setModel(const juce::String& objFileContent, const juce::String& mtlFileContent)
{
    loading = true;
    juce::Thread::launch([&, self = juce::WeakReference(this), objFileContent, mtlFileContent]
                         {
                             auto newObject = std::make_shared<WavefrontObjFile>();
                             newObject->load(objFileContent, mtlFileContent);

                             juce::MessageManager::callAsync([&, self, newObject]() mutable
                                                             {
                                                                 if (self == nullptr)
                                                                 {
                                                                     return;
                                                                 }

                                                                 std::lock_guard _(objectLock);
                                                                 std::swap(object, newObject);
                                                                 fillBuffersPending = true;
                                                                 loading = false;
                                                             });
                         });
}

void Model::setModel(const juce::File& objFile_)
{
    if (objFile_ == juce::File() || objFile == objFile_)
    {
        return;
    }
    objFile = objFile_;

    loading = true;
    juce::Thread::launch([&, self = juce::WeakReference(this), objFile_]
                         {
                             auto newObject = std::make_shared<WavefrontObjFile>();
                             newObject->load(objFile_);

                             juce::MessageManager::callAsync([&, self, newObject]() mutable
                                                             {
                                                                 if (self == nullptr)
                                                                 {
                                                                     return;
                                                                 }

                                                                 std::lock_guard _(objectLock);
                                                                 std::swap(object, newObject);
                                                                 fillBuffersPending = true;
                                                                 loading = false;
                                                             });
                         });
}

bool Model::isLoading() const
{
    return loading;
}

void Model::fillBuffers()
{
    glBuffers.clear();

    for (const auto& shape : object->shapes)
    {
        auto glBuffer = std::make_unique<GLBuffer>(*shape);
        glBuffers.push_back(std::move(glBuffer));
    }
}

Model::GLBuffer::GLBuffer(const WavefrontObjFile::Shape& shape) : associatedShape(shape)
{
    using namespace ::juce::gl;

    // Generate IDs for OpenGL vertex objects
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    fillBuffers();
}

Model::GLBuffer::~GLBuffer()
{
    using namespace ::juce::gl;

    // Delete OpenGL buffer objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
}

void Model::GLBuffer::fillBuffers()
{
    using namespace ::juce::gl;

    const auto& mesh = associatedShape.mesh;

    glBindVertexArray(vao); // bind VAO to cache all VBO, EBO, and vertex attribute state

    // Fill EBO buffer with indices array
    indicesSize = (GLuint) mesh.indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(sizeof(GLuint) * indicesSize), mesh.indices.data(), GL_STATIC_DRAW);

    // Fill VBO buffer with vertices array
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(GLfloat) * mesh.vertices.size()), mesh.vertices.data(), GL_STATIC_DRAW);

    // Define that our vertices are laid out as groups of 7 GLfloats (3 for position, 3 for normal, 2 for texture coord)
    const GLsizei positionDimension = 3;
    const GLsizei normalDimension = 3;
    const GLsizei textureCoordinateDimension = 2;
    const GLsizei vertexDataLength = (positionDimension + normalDimension + textureCoordinateDimension) * sizeof(GLfloat);

    // Position attribute (3 floats)
    const GLuint positionIndex = 0;
    glVertexAttribPointer(positionIndex, positionDimension, GL_FLOAT, GL_FALSE, vertexDataLength, nullptr);
    glEnableVertexAttribArray(positionIndex);

    // Normal attribute (3 floats)
    const GLuint normalIndex = 1;
    glVertexAttribPointer(normalIndex, normalDimension, GL_FLOAT, GL_FALSE, vertexDataLength, (void*) (positionDimension * sizeof(GLfloat)));
    glEnableVertexAttribArray(normalIndex);

    // Texture coordinate attribute (2 floats)
    const GLuint textureCoordinateIndex = 2;
    glVertexAttribPointer(textureCoordinateIndex, textureCoordinateDimension, GL_FLOAT, GL_FALSE, vertexDataLength, (void*) ((positionDimension + normalDimension) * sizeof(GLfloat)));
    glEnableVertexAttribArray(textureCoordinateIndex);

    // Unbind buffers
    glBindVertexArray(0); // unbind VAO first since it remembers last bound VBO & EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO
}
