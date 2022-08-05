#pragma once

#include <BinaryData.h>
#include "Shader.h"

class GraphDataShader : public Shader
{
public:
    explicit GraphDataShader(juce::OpenGLContext& context_) : Shader(context_, BinaryData::GraphData_vert, BinaryData::GraphData_frag)
    {
    }

    Attribute position { *this, "position" };
    Uniform window { *this, "window" };
    Uniform offsetAndScale { *this, "offsetAndScale" };
    Uniform colour { *this, "colour" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphDataShader)
};
