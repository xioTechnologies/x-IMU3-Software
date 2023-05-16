#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/Shader.h"

class GridLinesShader : public Shader
{
public:
    explicit GridLinesShader(juce::OpenGLContext& context_) : Shader("GridLines", context_, BinaryData::GridLines_vert, BinaryData::GridLines_frag)
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
