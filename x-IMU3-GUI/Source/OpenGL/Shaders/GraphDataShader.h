#pragma once

#include <BinaryData.h>
#include "OpenGL/Common/OpenGLHelpers.h"
#include "OpenGL/Common/Shader.h"

class GraphDataShader : public Shader {
public:
    explicit GraphDataShader(juce::OpenGLContext &context_) : Shader("GraphData", context_, BinaryData::GraphData_vert, BinaryData::GraphData_frag) {
    }

    OpenGLHelpers::Uniform axisLimitsRange{*this, "axisLimitsRange"};
    OpenGLHelpers::Uniform axisLimitsMin{*this, "axisLimitsMin"};
    OpenGLHelpers::Uniform colour{*this, "colour"};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphDataShader)
};
