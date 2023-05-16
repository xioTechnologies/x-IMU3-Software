#version 330 core

layout (location = 0) in vec3 position_vert;
layout (location = 1) in vec3 normal_vert;
layout (location = 2) in vec2 textureCoord_vert;

out vec3 position_frag;
out vec3 normal_frag;
out vec2 textureCoord_frag;

uniform mat4 modelMatrix;
uniform mat3 modelMatrixInverseTranspose;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    position_frag = vec3(modelMatrix * vec4(position_vert, 1.0));
    normal_frag = normalize(modelMatrixInverseTranspose * normal_vert);
    textureCoord_frag = textureCoord_vert;
    gl_Position = projectionMatrix * viewMatrix * vec4(position_frag, 1.0);
}
