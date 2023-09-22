#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLHelpers.h"
#include "OpenGL/Common/Shader.h"

class LitShader : public Shader
{
public:
    explicit LitShader(juce::OpenGLContext& context_) : Shader("Lit", context_, BinaryData::Lit_vert, BinaryData::Lit_frag)
    {
    }

    GLHelpers::Uniform modelMatrix { *this, "modelMatrix" };
    GLHelpers::Uniform modelMatrixInverseTranspose { *this, "modelMatrixInverseTranspose" };
    GLHelpers::Uniform viewMatrix { *this, "viewMatrix" };
    GLHelpers::Uniform projectionMatrix { *this, "projectionMatrix" };
    GLHelpers::Uniform cameraPosition { *this, "cameraPosition" };

    GLHelpers::Uniform lightColour { *this, "light.colour" };
    GLHelpers::Uniform lightPosition { *this, "light.position" };
    GLHelpers::Uniform lightIntensity { *this, "light.intensity" };

    GLHelpers::Uniform materialColour { *this, "materialColour" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LitShader)
};
