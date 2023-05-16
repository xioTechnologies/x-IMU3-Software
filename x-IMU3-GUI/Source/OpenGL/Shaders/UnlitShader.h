#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLUtil.hpp"
#include "OpenGL/Common/Shader.h"

class UnlitShader : public Shader
{
public:
    explicit UnlitShader(juce::OpenGLContext& context_) : Shader("Unlit", context_, BinaryData::Unlit_vert, BinaryData::Unlit_frag)
    {
    }

    GLUtil::Uniform modelViewProjectionMatrix { *this, "modelViewProjectionMatrix" };

    GLUtil::Uniform colour { *this, "colour" };
    GLUtil::Uniform isTextured { *this, "isTextured" };
    GLUtil::Uniform textureImage { *this, "textureImage" };
};
