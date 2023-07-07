#pragma once

#include "Common/GLRenderer.h"
#include "Common/Text.h"
#include "Graph/GraphDataBuffer.h"
#include "Graph/GridLines.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGLComponent.h"

class Graph : public OpenGLComponent
{
public:
    struct Settings
    {
        Settings(const bool horizontalAutoscale = false, const float horizontalMin = -5.0f, const float horizontalMax = 0.0f,
                 const bool verticalAutoscale = true, const float verticalMin = -1.0f, const float verticalMax = 1.0f);

        Settings(const Settings& other);

        Settings& operator=(const Settings& other);

        std::atomic<int> clearCounter { 0 };
        std::atomic<bool> paused { false };

        struct Axis
        {
            std::atomic<bool> autoscale;
            std::atomic<float> min;
            std::atomic<float> max;
        };

        Axis horizontal;
        Axis vertical;

        std::array<std::atomic<bool>, 3> visibleLines { true, true, true };
    };

    struct LegendItem
    {
        juce::String label;
        juce::Colour colour;
    };

    Graph(GLRenderer& renderer_, const juce::String& yAxis_, const std::vector<LegendItem>& legend_, const Settings& settings_);

    ~Graph() override;

    void render() override;

    const std::vector<LegendItem>& getLegend() const;

    void update(const uint64_t timestamp, const std::vector<float>& values);

    juce::Rectangle<int> padded(juce::Rectangle<int> rectangle);

private:
    static constexpr int yAxisLabelWidth = 30;
    static constexpr int xAxisLabelHeight = 25;
    static constexpr int xAxisValuesHeight = 15;

    GLRenderer& renderer;
    const juce::String yAxis;
    std::vector<LegendItem> legend;
    const Settings& settings;

    int clearCounter = 0;

    GridLines gridLines;
    GraphDataBuffer graphDataBuffer { legend.size() };

    std::atomic<int> extraLeftPadding { 0 };

    static int getNumberOfDecimalPlaces(const float rangeMin, const float rangeMax);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
