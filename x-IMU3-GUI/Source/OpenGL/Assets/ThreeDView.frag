struct Light
{
    vec4 colour;
    vec3 position;
    float intensity;
};

in vec4 colourOut;
in vec3 vertexOut;
in vec3 normalOut;
in vec2 textureOut;

uniform Light light;
uniform bool isTextured;
uniform vec3 cameraPosition;
uniform sampler2D textureImage;

uniform float emissivity;// higher values = darker
uniform float maxSpecularRange;// higher values = matte
uniform float specularFactor;// higher values = matte surfaces and corners

vec4 GetDiffuseSpecular()
{
    vec3 lightDirection = normalize(light.position - vertexOut);
    vec3 viewDirection = normalize(cameraPosition - vertexOut);
    vec3 reflection = normalize(reflect(-lightDirection, normalOut));

    float intensity = max(dot(normalOut, lightDirection), 0.0);
    float specularScale = smoothstep(0.0, maxSpecularRange, intensity);
    float specular = pow(max(dot(-reflection, viewDirection), 0.0), specularFactor) * specularScale;

    return vec4(intensity * light.colour.xyz, specular) * light.intensity;
}

void main()
{
    vec4 lightAccum = vec4(0.0);
    vec3 ambientColour = vec3(0.1, 0.1, 0.1);

    vec4 shadowlightTemp = GetDiffuseSpecular();
    vec4 lightingShadow = vec4(shadowlightTemp.rgb * colourOut.rgb, shadowlightTemp.w);
    vec4 lightingPoints = lightAccum + lightingShadow;

    lightAccum.rgb = mix(colourOut.rgb, (lightAccum.rgb + lightingPoints.rgb) * colourOut.rgb, emissivity);
    lightAccum.rgb = max(lightAccum.rgb, ambientColour * colourOut.rgb);
    lightAccum.a += lightingPoints.a;

    fragColor = vec4(lightAccum.rgb + vec3(lightAccum.a), colourOut.a);

    if (isTextured)
    {
        fragColor *= texture(textureImage, textureOut);
    }
}
