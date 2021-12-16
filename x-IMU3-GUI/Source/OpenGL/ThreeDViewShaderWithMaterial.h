#include "Shader.h"

class ThreeDViewShaderWithMaterial : public Shader
{
public:
    explicit ThreeDViewShaderWithMaterial(juce::OpenGLContext& context) : Shader(context, BinaryData::ThreeDViewWithMaterial_vert, BinaryData::ThreeDViewWithMaterial_frag)
    {
    }

    Uniform isTextured { *this, "isTextured" };
    Uniform textureImage { *this, "textureImage" };
    Uniform modelMatrix { *this, "modelMatrix" };
    Uniform projectionMatrix { *this, "projectionMatrix" };

    Uniform lightPosition { *this, "light.position" };
    Uniform lightAmbient { *this, "light.ambient" };
    Uniform lightDiffuse { *this, "light.diffuse" };
    Uniform lightSpecular { *this, "light.specular" };

    Uniform materialAmbient { *this, "material.ambient" };
    Uniform materialDiffuse { *this, "material.diffuse" };
    Uniform materialSpecular { *this, "material.specular" };
    Uniform materialShininess { *this, "material.shininess" };

    Attribute vertexIn { *this, "vertexIn" };
    Attribute colourIn { *this, "colourIn" };
    Attribute textureIn { *this, "textureIn" };
    Attribute normalIn { *this, "normalIn" };
};
