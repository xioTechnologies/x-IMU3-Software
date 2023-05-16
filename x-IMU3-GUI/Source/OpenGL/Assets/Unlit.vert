// Used for objects not affected by lighting
#version 330 core

layout (location = 0) in vec3 position_vert;
// vertex attribute (location = 1) is reserved for normals
layout (location = 2) in vec2 textureCoord_vert;

out vec2 textureCoord_frag;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    textureCoord_frag = textureCoord_vert;
    gl_Position = modelViewProjectionMatrix * vec4 (position_vert, 1.0);
}
