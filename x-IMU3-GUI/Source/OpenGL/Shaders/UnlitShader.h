#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/GLHelpers.h"
#include "OpenGL/Common/Shader.h"

class UnlitShader : public Shader
{
public:
    explicit UnlitShader(juce::OpenGLContext& context_) : Shader("Unlit", context_, BinaryData::Unlit_vert, BinaryData::Unlit_frag)
    {
        // Init texture uniform to always be associated with texture unit 0 (GL_TEXTURE0)
        use();
        textureImage.set(0);
    }

    void setTextureImage(const juce::OpenGLTexture& texture) const
    {
        juce::gl::glActiveTexture(juce::gl::GL_TEXTURE0);
        texture.bind();
    }

    GLHelpers::Uniform modelViewProjectionMatrix { *this, "modelViewProjectionMatrix" };
    GLHelpers::Uniform colour { *this, "colour" };

private:
    GLHelpers::Uniform textureImage { *this, "textureImage" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnlitShader)
};
