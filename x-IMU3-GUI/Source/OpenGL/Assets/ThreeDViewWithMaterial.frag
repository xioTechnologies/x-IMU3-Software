struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 vertexOut;
in vec4 colourOut;
in vec2 textureOut;
in vec3 normalOut;

uniform Light light;
uniform bool isTextured;
uniform Material material;
uniform sampler2D textureImage;

void main (void)
{
    vec3 normal = normalize(normalOut);
    vec3 ambientColour = light.ambient * material.ambient;

    vec3 lightDirection = normalize(light.position - vertexOut);
    float lightIntensity = max(dot(lightDirection, normal), 0.0);
    vec3 diffuseColour = light.diffuse * material.diffuse * lightIntensity;

    vec3 viewDirection = normalize(vertexOut);
    vec3 reflection = reflect(-lightDirection, normal);
    float specularTerm = pow(max(dot(-viewDirection, reflection), 0.0), material.shininess);
    vec3 specularColour = light.specular * material.specular * specularTerm;

    vec4 finalColour = colourOut * vec4((ambientColour + diffuseColour + specularColour), 1.0);
    fragColor = finalColour;

    if (isTextured)
    {
        fragColor = finalColour * texture(textureImage, textureOut);
    }
}
