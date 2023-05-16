#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLUtil.hpp"
#include "OpenGL/Common/Shader.h"

class ScreenSpaceLitShader : public Shader
{
public:
    explicit ScreenSpaceLitShader(juce::OpenGLContext& context_) : Shader("ScreenSpaceLit", context_, BinaryData::ScreenSpaceLit_vert, BinaryData::Lit_frag)
    {
    }

    GLUtil::Uniform inverseScreenScale { *this, "inverseScreenScale" };

    GLUtil::Uniform modelMatrix { *this, "modelMatrix" };
    GLUtil::Uniform modelMatrixInverseTranspose { *this, "modelMatrixInverseTranspose" };
    GLUtil::Uniform viewMatrix { *this, "viewMatrix" };
    GLUtil::Uniform projectionMatrix { *this, "projectionMatrix" };
    GLUtil::Uniform cameraPosition { *this, "cameraPosition" };

    GLUtil::Uniform lightColour { *this, "light.colour" };
    GLUtil::Uniform lightPosition { *this, "light.position" };
    GLUtil::Uniform lightIntensity { *this, "light.intensity" };

    GLUtil::Uniform materialColour { *this, "materialColour" };
    GLUtil::Uniform isTextured { *this, "isTextured" };
    GLUtil::Uniform textureImage { *this, "textureImage" };
};
