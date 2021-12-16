#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Shader : public juce::OpenGLShaderProgram
{
public:
    Shader(juce::OpenGLContext& context, const juce::String& vertexShader, const juce::String& fragmentShader) : juce::OpenGLShaderProgram(context)
    {
        if (addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader)) == false ||
            addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader)) == false ||
            link() == false)
        {
            std::cout << getLastError() << std::endl;
            jassertfalse;
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Shader)
};
