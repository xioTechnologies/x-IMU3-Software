// Used for objects not affected by lighting
#version 330 core

layout (location = 0) in vec3 position_vert;
// vertex attribute (location = 1) is reserved for normals
layout (location = 2) in vec2 textureCoord_vert;

out vec2 textureCoord_frag;

uniform mat4 modelViewProjectionMatrix;

uniform float inverseScreenScale = 1.0f;

void main()
{
    textureCoord_frag = textureCoord_vert;

    // Render at constant size in screen pixels by applying an inverse of the screen scale
    // Ref: https://community.khronos.org/t/draw-an-object-that-looks-the-same-size-regarles-the-distance-in-perspective-view/67804/6
    gl_Position = modelViewProjectionMatrix * vec4 (position_vert * inverseScreenScale, 1.0);
}
