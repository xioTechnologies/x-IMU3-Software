#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLHelpers.h"
#include "OpenGL/Common/Shader.h"

class ScreenSpaceLitShader : public Shader
{
public:
    explicit ScreenSpaceLitShader(juce::OpenGLContext& context_) : Shader("ScreenSpaceLit", context_, BinaryData::ScreenSpaceLit_vert, BinaryData::Lit_frag)
    {
    }

    GLHelpers::Uniform inverseScreenScale { *this, "inverseScreenScale" };

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScreenSpaceLitShader)
};
