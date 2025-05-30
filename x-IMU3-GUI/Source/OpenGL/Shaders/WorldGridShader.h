#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/OpenGLHelpers.h"
#include "OpenGL/Common/Shader.h"

class WorldGridShader : public Shader
{
public:
    explicit WorldGridShader(juce::OpenGLContext& context_) : Shader("WorldGrid", context_, BinaryData::WorldGrid_vert, BinaryData::WorldGrid_frag)
    {
    }

    OpenGLHelpers::Uniform modelViewProjectionMatrix { *this, "modelViewProjectionMatrix" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WorldGridShader)
};
