#pragma once

#include "Buffer.h"
#include "GraphDataShader.h"
#include "GridLinesShader.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include "Model.h"
#include "Text.h"
#include "TextShader.h"
#include "ThreeDViewShader.h"
#include "ThreeDViewShaderWithMaterial.h"

struct Vec4
{
    GLfloat x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
};

class GLResources
{
    juce::OpenGLContext& context;

public:
    static constexpr int borderThickness = 1;
    static constexpr int graphBufferSize = 1 << 16;

    explicit GLResources(juce::OpenGLContext& context_);

    Text& getGraphLegendText();

    Text& getGraphAxisValuesText();

    Text& getGraphAxisLabelText();

    Text& get3DViewLoadingText();

    Text& get3DViewAxisText();

    Buffer textBuffer { context };

    Buffer gridBorderBuffer { context };
    Buffer gridVerticalBuffer { context };
    Buffer gridHorizontalBuffer { context };

    Buffer graphDataBuffer { context };

    Model arrow { context };
    Model board { context };
    Model housing { context };
    Model stage { context };
    Model teapot { context };
    Model custom { context };

    const GraphDataShader graphDataShader { context };
    const GridLinesShader gridLinesShader { context };
    const TextShader textShader { context };
    const ThreeDViewShaderWithMaterial threeDViewShaderWithMaterial { context };
    const ThreeDViewShader threeDViewShader { context };

    juce::OpenGLTexture compassTexture;

private:
    void createGraphDataBuffer();

    void createGridBuffers();

    void createTextBuffer();

    std::unique_ptr<Text> infoText;
    std::unique_ptr<Text> legendText;
    std::unique_ptr<Text> axisValuesText;
    std::unique_ptr<Text> axisLabelText;
    std::unique_ptr<Text> axisMarkerText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GLResources)
};
