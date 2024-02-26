#version 330 core

in vec2 textureCoord_frag;
out vec4 fragColour;

uniform vec4 colour;
uniform sampler2D textureImage;

void main()
{
    float glyphOpacity = texture(textureImage, textureCoord_frag).r; // texture data is stored grayscale in GL_RED, interpret as opacity
    fragColour = colour * vec4(1.0, 1.0, 1.0, glyphOpacity);
}
