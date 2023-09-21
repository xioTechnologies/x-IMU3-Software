#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>
#include "OpenGL/Graph/LineBuffer.h"
#include "OpenGL/Shaders/GraphDataShader.h"
#include "OpenGL/Shaders/GraphDataShader.h"
#include "OpenGL/Shaders/GraphGridShader.h"
#include "OpenGL/Shaders/GraphGridShader.h"
#include "OpenGL/Shaders/LitShader.h"
#include "OpenGL/Shaders/ScreenSpaceLitShader.h"
#include "OpenGL/Shaders/TextShader.h"
#include "OpenGL/Shaders/UnlitShader.h"
#include "OpenGL/Shaders/WorldGridShader.h"
#include "OpenGL/ThreeDView/Model.h"
#include "OpenGL/ThreeDView/OrbitCamera.h"
#include "OpenGL/ThreeDView/PlaneModel.h"
#include "Text.h"
#include "TextQuad.h"

class GLResources
{
    juce::OpenGLContext& context;

public:
    static constexpr int graphBufferSize = 1 << 16;

    explicit GLResources(juce::OpenGLContext& context_);

    // Text
    Text& getGraphTickText();

    Text& get3DViewAxesText();

    // Geometry
    Model arrow { context };
    Model board { context };
    Model housing { context };
    Model custom { context };
    PlaneModel plane;
    TextQuad textQuad;
    LineBuffer graphGridBuffer { true };
    LineBuffer graphDataBuffer { false };

    // Shaders
    const GraphDataShader graphDataShader { context };
    const GraphGridShader graphGridShader { context };
    const GraphGridShader gridLinesShader { context };
    const WorldGridShader grid3DShader { context };
    const TextShader textShader { context };
    const LitShader threeDViewShader { context };
    const UnlitShader unlitShader { context };
    const ScreenSpaceLitShader screenSpaceLitShader { context };

    // Textures
    juce::OpenGLTexture compassTexture;

    OrbitCamera orbitCamera;

private:
    std::unique_ptr<Text> graphTickText;
    std::unique_ptr<Text> threeDViewAxesText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GLResources)
};
