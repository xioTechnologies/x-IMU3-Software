#version 330 core

in vec3 vertexIn;
in vec2 textureIn;

out vec2 textureOut;

uniform mat4 transformation;

void main(void)
{
    textureOut = textureIn;
    gl_Position = transformation * vec4(vertexIn, 1.0);
}