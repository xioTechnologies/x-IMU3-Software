#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/Shader.h"

class GraphDataShader : public Shader
{
public:
    explicit GraphDataShader(juce::OpenGLContext& context_) : Shader("GraphData", context_, BinaryData::GraphData_vert, BinaryData::GraphData_frag)
    {
    }

    // TODO: Attribute variables can be removed in refactor
    Attribute position { *this, "position" };
    Uniform window { *this, "window" };
    Uniform offsetAndScale { *this, "offsetAndScale" };
    Uniform colour { *this, "colour" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphDataShader)
};
