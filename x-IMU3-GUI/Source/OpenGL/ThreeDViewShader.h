#include "Shader.h"

class ThreeDViewShader : public Shader
{
public:
    explicit ThreeDViewShader(juce::OpenGLContext& context) : Shader(context, BinaryData::ThreeDView_vert, BinaryData::ThreeDView_frag)
    {
    }

    Uniform modelMatrix { *this, "modelMatrix" };
    Uniform projectionMatrix { *this, "projectionMatrix" };

    Uniform lightColour { *this, "light.colour" };
    Uniform lightPosition { *this, "light.position" };
    Uniform lightIntensity { *this, "light.intensity" };

    Uniform emissivity { *this, "emissivity" };
    Uniform isTextured { *this, "isTextured" };
    Uniform textureImage { *this, "textureImage" };
    Uniform specularFactor { *this, "specularFactor" };
    Uniform cameraPosition { *this, "cameraPosition" };
    Uniform maxSpecularRange { *this, "maxSpecularRange" };

    Attribute vertexIn { *this, "vertexIn" };
    Attribute colourIn { *this, "colourIn" };
    Attribute textureIn { *this, "textureIn" };
    Attribute normalIn { *this, "normalIn" };
};
