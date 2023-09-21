#version 330 core

layout (location = 0) in vec3 position_vert;
layout (location = 1) in vec2 textureCoord_vert;

out vec2 textureCoord_frag;

uniform mat4 transform;

void main()
{
    textureCoord_frag = textureCoord_vert;
    gl_Position = transform * vec4(position_vert, 1.0);
}