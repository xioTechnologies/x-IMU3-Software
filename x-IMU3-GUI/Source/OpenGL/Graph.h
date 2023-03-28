#pragma once

#include "GLRenderer.h"
#include "GraphDataBuffer.h"
#include "GridLines.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGLComponent.h"
#include "Text.h"

class Graph : public OpenGLComponent
{
public:
    struct Settings
    {
        Settings(const bool horizontalAutoscale = false, const float horizontalMin = -5.0f, const float horizontalMax = 0.0f,
                 const bool verticalAutoscale = true, const float verticalMin = -1.0f, const float verticalMax = 1.0f);

        Settings& operator=(const Settings& other);

        struct Axis
        {
            std::atomic<bool> autoscale;
            std::atomic<float> min;
            std::atomic<float> max;
        };

        Axis horizontal;
        Axis vertical;
    };

    struct LegendItem
    {
        juce::String label;
        juce::Colour colour;
    };

    Graph(GLRenderer& renderer_, const juce::String& yAxis_, const std::vector<LegendItem>& legend_, const Settings& settings_);

    ~Graph() override;

    void render() override;

    void update(const uint64_t timestamp, const std::vector<float>& values);

    void clear();

    juce::Rectangle<int> padded(juce::Rectangle<int> rectangle);

private:
    static constexpr int yAxisLabelWidth = 30;
    static constexpr int xAxisLabelHeight = 25;
    static constexpr int xAxisValuesHeight = 15;

    GLRenderer& renderer;
    const juce::String yAxis;
    std::vector<LegendItem> legend;
    const Settings& settings;

    GridLines gridLines;
    GraphDataBuffer graphDataBuffer { legend.size() };

    std::atomic<int> extraLeftPadding { 0 };

    static int getNumberOfDecimalPlaces(const float rangeMin, const float rangeMax);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
