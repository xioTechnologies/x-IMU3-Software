#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/Shader.h"

class GraphGridShader : public Shader
{
public:
    explicit GraphGridShader(juce::OpenGLContext& context_) : Shader("GraphGrid", context_, BinaryData::GraphGrid_vert, BinaryData::GraphGrid_frag)
    {
    }

    // TODO: Attribute variables can be removed in refactor
    Attribute position { *this, "position" };
    Uniform window { *this, "window" };
    Uniform offsetAndScale { *this, "offsetAndScale" };
    Uniform colour { *this, "colour" };
    Uniform isBorder { *this, "isBorder" };
    Uniform isVertical { *this, "isVertical" };
};
