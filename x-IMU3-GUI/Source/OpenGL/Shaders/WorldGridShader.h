#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLUtil.hpp"
#include "OpenGL/Common/Shader.h"

class WorldGridShader : public Shader
{
public:
    explicit WorldGridShader(juce::OpenGLContext& context_) : Shader("WorldGrid", context_, BinaryData::WorldGrid_vert, BinaryData::WorldGrid_frag)
    {
    }

    GLUtil::Uniform modelViewProjectionMatrix { *this, "modelViewProjectionMatrix" };
};
