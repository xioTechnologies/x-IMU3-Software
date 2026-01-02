#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/OpenGLHelpers.h"
#include "OpenGL/Common/Shader.h"

class ScreenSpaceLitShader : public Shader {
public:
    explicit ScreenSpaceLitShader(juce::OpenGLContext &context_) : Shader("ScreenSpaceLit", context_, BinaryData::ScreenSpaceLit_vert, BinaryData::Lit_frag) {
    }

    OpenGLHelpers::Uniform inverseScreenScale{*this, "inverseScreenScale"};

    OpenGLHelpers::Uniform modelMatrix{*this, "modelMatrix"};
    OpenGLHelpers::Uniform modelMatrixInverseTranspose{*this, "modelMatrixInverseTranspose"};
    OpenGLHelpers::Uniform viewMatrix{*this, "viewMatrix"};
    OpenGLHelpers::Uniform projectionMatrix{*this, "projectionMatrix"};
    OpenGLHelpers::Uniform cameraPosition{*this, "cameraPosition"};

    OpenGLHelpers::Uniform lightColour{*this, "light.colour"};
    OpenGLHelpers::Uniform lightPosition{*this, "light.position"};
    OpenGLHelpers::Uniform lightIntensity{*this, "light.intensity"};

    OpenGLHelpers::Uniform materialColour{*this, "materialColour"};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScreenSpaceLitShader)
};
