#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/OpenGLHelpers.h"
#include "OpenGL/Common/Shader.h"

class TextShader : public Shader
{
public:
    explicit TextShader(juce::OpenGLContext& context_) : Shader("Text", context_, BinaryData::Text_vert, BinaryData::Text_frag)
    {
        // Init texture uniform to always be associated with texture unit 0 (GL_TEXTURE0)
        use();
        textureImage.set(0);
    }

    void setTextureImage(GLenum target, GLuint texture) const
    {
        using namespace ::juce::gl;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(target, texture);
    }

    OpenGLHelpers::Uniform colour { *this, "colour" };
    OpenGLHelpers::Uniform transform { *this, "transform" };

private:
    OpenGLHelpers::Uniform textureImage { *this, "textureImage" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextShader)
};
