in vec4 colourOut;
in vec2 textureOut;
out vec4 pixelColour;
uniform sampler2D textureImage;

void main(void)
{
    pixelColour = colourOut * texture(textureImage, textureOut).r;
}