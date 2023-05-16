#version 330 core

in vec2 textureOut;
out vec4 pixelColour;

uniform vec4 colour;
uniform sampler2D textureImage;

void main(void)
{
    pixelColour = colour * texture(textureImage, textureOut).r;
}