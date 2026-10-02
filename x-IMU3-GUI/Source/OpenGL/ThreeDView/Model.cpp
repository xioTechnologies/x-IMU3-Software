#include "Model.h"

Model::Model(juce::ThreadPool &threadPool, const juce::String &objFileContent, const juce::String &mtlFileContent) {
    threadPool.addJob([state_ = state, objFileContent, mtlFileContent] {
        auto object = std::make_unique<WavefrontObjFile>();
        object->load(objFileContent, mtlFileContent);

        state_->object = std::move(object);
        state_->loaded = true;
    });
}

Model::Model(juce::ThreadPool &threadPool, const juce::File &objFile) {
    threadPool.addJob([state_ = state, objFile] {
        auto object = std::make_unique<WavefrontObjFile>();
        object->load(objFile);

        state_->object = std::move(object);
        state_->loaded = true;
    });
}

void Model::render() {
    if (state->loaded == false) {
        return;
    }

    if (glBuffers.empty()) {
        for (const auto &shape: state->object->shapes) {
            glBuffers.push_back(std::make_unique<GLBuffer>(*shape));
        }
    }

    // Render all OpenGL buffers
    for (const auto &glBuffer: glBuffers) {
        using namespace ::juce::gl;
        glBindVertexArray(glBuffer->vao); // Bind VAO for all vertex data (VBO, EBO, attributes)
        glDrawElements(GL_TRIANGLES, (GLsizei) glBuffer->indicesSize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0); // unbind VAO to prevent modification to the VAO by subsequent GL calls
    }
}

void Model::renderWithMaterials(const LitShader &shader) {
    if (state->loaded == false) {
        return;
    }

    if (glBuffers.empty()) {
        for (const auto &shape: state->object->shapes) {
            glBuffers.push_back(std::make_unique<GLBuffer>(*shape));
        }
    }

    // Render all OpenGL buffers
    for (const auto &glBuffer: glBuffers) {
        using namespace ::juce::gl;

        const auto &material = glBuffer->associatedShape.material;
        shader.materialColour.set(glm::vec4(material.diffuse, 1.0f));

        glBindVertexArray(glBuffer->vao); // bind VAO for all vertex data (VBO, EBO, attributes)
        glDrawElements(GL_TRIANGLES, (GLsizei) glBuffer->indicesSize, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0); // unbind VAO to prevent modification to the VAO by subsequent GL calls
    }
}

bool Model::isLoading() const {
    return state->loaded == false;
}

Model::GLBuffer::GLBuffer(const WavefrontObjFile::Shape &shape) : associatedShape(shape) {
    using namespace ::juce::gl;

    // Generate IDs for OpenGL vertex objects
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    fillBuffers();
}

Model::GLBuffer::~GLBuffer() {
    using namespace ::juce::gl;

    // Delete OpenGL buffer objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
}

void Model::GLBuffer::fillBuffers() {
    using namespace ::juce::gl;

    const auto &mesh = associatedShape.mesh;

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
    glVertexAttribPointer(normalIndex, normalDimension, GL_FLOAT, GL_FALSE, vertexDataLength, (void *) (positionDimension * sizeof(GLfloat)));
    glEnableVertexAttribArray(normalIndex);

    // Texture coordinate attribute (2 floats)
    const GLuint textureCoordinateIndex = 2;
    glVertexAttribPointer(textureCoordinateIndex, textureCoordinateDimension, GL_FLOAT, GL_FALSE, vertexDataLength, (void *) ((positionDimension + normalDimension) * sizeof(GLfloat)));
    glEnableVertexAttribArray(textureCoordinateIndex);

    // Unbind buffers
    glBindVertexArray(0); // unbind VAO first since it remembers last bound VBO & EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO
}
