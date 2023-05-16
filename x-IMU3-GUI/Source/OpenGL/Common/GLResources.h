#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include "OpenGL/Common/Text.h"
#include "OpenGL/Graph/Buffer.h"
#include "OpenGL/Shaders/GraphDataShader.h"
#include "OpenGL/Shaders/Grid3DShader.h"
#include "OpenGL/Shaders/GridLinesShader.h"
#include "OpenGL/Shaders/LitShader.h"
#include "OpenGL/Shaders/ScreenSpaceLitShader.h"
#include "OpenGL/Shaders/TextShader.h"
#include "OpenGL/Shaders/UnlitShader.h"
#include "OpenGL/ThreeDView/Model.h"
#include "OpenGL/ThreeDView/OrbitCamera.h"
#include "OpenGL/ThreeDView/PlaneModel.h"

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

    // Text
    Text& getGraphLegendText();

    Text& getGraphAxisValuesText();

    Text& getGraphAxisLabelText();

    Text& get3DViewLoadingText();

    Text& get3DViewAxisText();

    // Buffers
    Buffer textBuffer { context };
    Buffer gridBorderBuffer { context };
    Buffer gridVerticalBuffer { context };
    Buffer gridHorizontalBuffer { context };
    Buffer graphDataBuffer { context };

    // Models
    Model arrow { context };
    Model board { context };
    Model housing { context };
    Model teapot { context };
    Model custom { context };
    PlaneModel plane;

    // Shaders
    const GraphDataShader graphDataShader { context };
    const GridLinesShader gridLinesShader { context };
    const Grid3DShader grid3DShader { context };
    const TextShader textShader { context };
    const LitShader threeDViewShader { context };
    const UnlitShader unlitShader { context };
    const ScreenSpaceLitShader screenSpaceLitShader { context };

    // Textures
    juce::OpenGLTexture compassTexture;

    OrbitCamera orbitCamera;

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
