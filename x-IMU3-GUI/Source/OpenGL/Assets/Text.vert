in vec3 vertexIn;
in vec4 colourIn;
in vec2 textureIn;

out vec4 colourOut;
out vec2 textureOut;

uniform mat4 transformation;

void main(void)
{
    colourOut = colourIn;
    textureOut = textureIn;
    gl_Position = transformation * vec4(vertexIn, 1.0);
}