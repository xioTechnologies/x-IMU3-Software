#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLUtil.hpp"
#include "OpenGL/Common/Shader.h"

class ScreenSpaceUnlitShader : public Shader
{
public:
    explicit ScreenSpaceUnlitShader(juce::OpenGLContext& context_) : Shader("ScreenSpaceUnlit", context_, BinaryData::ScreenSpaceUnlit_vert, BinaryData::Unlit_frag)
    {
    }

    GLUtil::Uniform modelViewProjectionMatrix { *this, "modelViewProjectionMatrix" };

    GLUtil::Uniform colour { *this, "colour" };
    GLUtil::Uniform isTextured { *this, "isTextured" };
    GLUtil::Uniform textureImage { *this, "textureImage" };

    GLUtil::Uniform inverseScreenScale { *this, "inverseScreenScale"};
};
