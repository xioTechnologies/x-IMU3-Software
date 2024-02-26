// Used for objects not affected by lighting
#version 330 core

in vec2 textureCoord_frag;

out vec4 fragColour;

uniform vec4 colour;
uniform sampler2D textureImage;

void main()
{
    fragColour =  texture(textureImage, textureCoord_frag) * colour;
}
