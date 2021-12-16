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
        Settings& operator=(const Settings& other);

        struct Axis
        {
            std::atomic<bool> autoscale;
            std::atomic<float> min;
            std::atomic<float> max;
        };

        Axis horizontal { false, -5.0f, 0.0f };
        Axis vertical { true, -1.0f, 1.0f };
    };

    struct LegendItem
    {
        juce::String label;
        juce::Colour colour;
    };

    Graph(GLRenderer& renderer_, const juce::String& yAxis_, const std::vector<LegendItem>& legend_);

    ~Graph() override;

    void render() override;

    void update(const uint64_t timestamp, const std::vector<float>& values);

    void setSettings(const Settings& settings_);

    void clear();

    juce::Rectangle<int> padded(juce::Rectangle<int> rectangle);

private:
    static constexpr int yAxisLabelWidth = 30;
    static constexpr int xAxisLabelHeight = 25;
    static constexpr int xAxisValuesHeight = 15;

    GLRenderer& renderer;
    const juce::String yAxis;

    Settings settings;
    std::vector<LegendItem> legend;

    GridLines gridLines;
    GraphDataBuffer graphDataBuffer { legend.size() };

    std::atomic<int> extraLeftPadding { 0 };

    static int getNumberOfDecimalPlaces(const float rangeMin, const float rangeMax);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
