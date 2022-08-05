#pragma once

#include "Shader.h"

class TextShader : public Shader
{
public:
    TextShader(juce::OpenGLContext& context_) : Shader(context_, BinaryData::Text_vert, BinaryData::Text_frag)
    {
    }

    Uniform textureImage { *this, "textureImage" };
    Attribute vertexIn { *this, "vertexIn" };
    Attribute colourIn { *this, "colourIn" };
    Attribute textureIn { *this, "textureIn" };
    Uniform transformation { *this, "transformation" };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextShader)
};
