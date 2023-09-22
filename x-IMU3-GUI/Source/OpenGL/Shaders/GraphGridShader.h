#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLHelpers.h"
#include "OpenGL/Common/Shader.h"

class GraphGridShader : public Shader
{
public:
    explicit GraphGridShader(juce::OpenGLContext& context_) : Shader("GraphGrid", context_, BinaryData::GraphGrid_vert, BinaryData::GraphGrid_frag)
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphGridShader)
};
