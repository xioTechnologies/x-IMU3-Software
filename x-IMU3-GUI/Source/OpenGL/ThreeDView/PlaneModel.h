#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <juce_opengl/juce_opengl.h>

class PlaneModel
{
public:
    PlaneModel();

    ~PlaneModel();

    void render() const;

private:
    void initGLData();

    void deinitGLData();

    static void makePlaneGeometry(const float extent, std::vector<GLfloat>& verticesOut, std::vector<GLuint>& indicesOut);

    // OpenGL vertex data buffers
    GLuint VAO = 0; // vertex array object
    GLuint VBO = 0; // vertex buffer object
    GLuint EBO = 0; // element buffer object
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};
