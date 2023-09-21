// Phong lighting model based on https://learnopengl.com/Lighting/Basic-Lighting
// with addition of lighting intensity, lighting attenuation, gamma correction, and HDR tonemap
// Note: could add additional exposure tone mapping control: https://learnopengl.com/Advanced-Lighting/HDR
#version 330 core

struct Light
{
    vec3 colour;
    vec3 position;
    float intensity;
};

in vec3 position_frag;
in vec3 normal_frag;
in vec2 textureCoord_frag;

out vec4 fragColour;

uniform vec4 materialColour;
uniform Light light;
uniform vec3 cameraPosition;

const float ambientStrength = 0.1;// base world lighting, ranges from 0 to 1
const float specularStrength = 0.53;// highlights, ranges from 0 to 1, Blender default lighting is close to 0.53
const float shininess = 2.0;// ranges from around 2 (rough) to 256 (shiny), a decent halfway default 32.0

// Prevent material colour from being too dark or bright since real life materials are rarely pure black or pure white
const float maxMaterialBrightness = 0.99;
const float minMaterialBrightness = 0.008;

const float gamma = 1.7;// gamma correction, 1.7 for more contrast than default 2.2, ref: https://learnopengl.com/Advanced-Lighting/Gamma-Correction
const float exposure = 1.2;

vec3 calculateLight (const Light light)
{
    // Ambient lighting
    vec3 ambient = ambientStrength * light.colour;

    // Diffuse lighting
    vec3 lightDirection = normalize(light.position - position_frag);
    float diffuseFragment = max(dot(normal_frag, lightDirection), 0.0);
    vec3 diffuse = diffuseFragment * light.colour * light.intensity;

    // Specular lighting
    vec3 viewDirection = normalize(cameraPosition - position_frag);
    vec3 reflectDirection = reflect(-lightDirection, normal_frag);
    float specularFragment = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
    vec3 specular = specularStrength * specularFragment * light.colour * light.intensity;

    // Clamp material colour to prevent pure black or pure white materials so shadows/lights still appear on surface
    vec3 materialColourClamped = clamp(materialColour.rgb, vec3(minMaterialBrightness), vec3(maxMaterialBrightness));

    // Light attenuation, ref: https://learnopengl.com/Lighting/Light-casters
    float distance = length(light.position - position_frag);
    float attenuation = 1.0 / distance * distance;// quadratic attenuation, works well with gamma correction

    vec3 lightingColour = vec3 (ambient + diffuse + specular) * attenuation * materialColourClamped;
    return lightingColour;
}

void main()
{
    vec3 color = vec3(0.0);
    color += calculateLight(light);

    color = vec3(1.0) - exp(-color * exposure);// HDR tone mapping with exposure, ref: https://learnopengl.com/Advanced-Lighting/HDR
    color = pow (color, vec3(1.0 / gamma));// gamma correction

    fragColour = vec4(color, materialColour.a);
}