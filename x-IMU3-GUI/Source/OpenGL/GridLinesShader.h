#pragma once

#include "Shader.h"

class GridLinesShader : public Shader
{
public:
    explicit GridLinesShader(juce::OpenGLContext& context_) : Shader(context_, BinaryData::GridLines_vert, BinaryData::GridLines_frag)
    {
    }

    Attribute position { *this, "position" };
    Uniform window { *this, "window" };
    Uniform offsetAndScale { *this, "offsetAndScale" };
    Uniform colour { *this, "colour" };
    Uniform isBorder { *this, "isBorder" };
    Uniform isVertical { *this, "isVertical" };
};
