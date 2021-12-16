#pragma once

#include "AxesRange.h"
#include "Buffer.h"
#include "GLResources.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Shader.h"
#include "Text.h"
#include <vector>

class GridLines
{
public:
    GridLines()
    {
    }

    void render(GLResources& resources, const juce::Rectangle<GLint>& viewport_, const AxesRange& axesRange_, juce::Point<GLfloat> pixelSize_);

    std::vector<float> getMajorXPositions() const;

    std::vector<float> getMajorYPositions() const;

private:
    static constexpr float subTickColor = 0.45f;
    static constexpr float subMajorTickColor = 0.65f;
    static constexpr float majorTickColor = 0.85f;

    void createBorderLines();

    int createVerticalLines();

    int createHorizontalLines();

    juce::Point<float> getPixelsPerUnit();

    void getMajorMinorTicks(double& tickSize, int& majorEveryInt, int& labelEveryInt, double pixelsPerUnit, float pixelThreshold);

    std::vector<Vec4> verticesBorderLines;
    std::vector<Vec4> verticesVerticalLines;
    std::vector<Vec4> verticesHorizontalLines;

    juce::Rectangle<GLint> viewport;
    AxesRange axesRange;
    juce::Point<GLfloat> pixelSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridLines)
};
