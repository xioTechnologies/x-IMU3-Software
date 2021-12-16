in vec3 vertexIn;
in vec4 colourIn;
in vec2 textureIn;
in vec3 normalIn;

out vec3 vertexOut;
out vec4 colourOut;
out vec2 textureOut;
out vec3 normalOut;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main()
{
    colourOut = colourIn;
    textureOut = textureIn;
    normalOut = normalize((modelMatrix * vec4(normalIn, 0.0)).xyz);
    vertexOut = (modelMatrix * vec4(vertexIn, 1.0)).xyz;
    gl_Position = projectionMatrix * modelMatrix * vec4(vertexIn, 1.0);
}
