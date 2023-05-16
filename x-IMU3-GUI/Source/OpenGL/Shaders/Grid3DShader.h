#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLUtil.hpp"
#include "OpenGL/Common/Shader.h"

class Grid3DShader : public Shader
{
public:
    explicit Grid3DShader(juce::OpenGLContext& context_) : Shader("Grid3D", context_, BinaryData::Grid3D_vert, BinaryData::Grid3D_frag)
    {
    }

    GLUtil::Uniform modelViewProjectionMatrix { *this, "modelViewProjectionMatrix" };
};
