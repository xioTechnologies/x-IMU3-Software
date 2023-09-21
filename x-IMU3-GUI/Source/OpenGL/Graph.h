#pragma once

#include "../CustomLookAndFeel.h"
#include "Common/GLHelpers.h"
#include "Common/GLRenderer.h"
#include "Graph/AxesLimits.h"
#include "Graph/ChannelBuffers.h"
#include "Graph/Ticks.h"
#include "OpenGLComponent.h"

class Graph : public OpenGLComponent
{
public:
    struct Settings
    {
        AxesLimits axesLimits;
        bool horizontalAutoscale = false;
        bool verticalAutoscale = true;
        std::vector<bool> enabledChannels;
    };

    Graph(GLRenderer& renderer_, const std::vector<juce::Colour>& colours_, const int legendHeight_, const int rightMargin_);

    ~Graph() override;

    void render() override;

    void setSettings(Settings settings_);

    Settings getSettings() const;

    void setTicksEnabled(const bool enabled);

    void scaleToFit();

    void clear();

    void write(const uint64_t timestamp, const std::vector<float>& values);

    std::atomic<float> plotWidthJUCEPixels = 0.0f;
    std::atomic<float> plotHeightJUCEPixels = 0.0f;

private:
    GLRenderer& renderer;
    const std::vector<juce::Colour> colours;
    const int legendHeight;
    const int rightMargin;

    mutable std::mutex settingsMutex;
    Settings settings;

    std::atomic<bool> scaleToFitPending { false };

    ChannelBuffers buffer { (int) colours.size() };

    std::atomic<bool> ticksEnabled { false };

    static constexpr GLfloat majorTickBrightness = 0.65f;
    static constexpr GLfloat minorTickBrightness = 0.45f;
    static constexpr GLfloat borderBrightness = 0.65f;

    static float engineeringValueToNDC(float value, const AxisLimits& axisLimits); // map to OpenGL Normalized Device Coordinates (NDC) with values ranging from -1.0 to 1.0

    static int getMaximumStringWidth(const std::vector<Tick>& ticks, const Text& text);

    void drawPlot(const juce::Rectangle<int>& bounds, const AxesLimits& limits, const std::vector<Tick>& xTicks, const std::vector<Tick>& yTicks, const std::vector<std::span<const juce::Point<GLfloat>>>& channels, const std::vector<bool>& enabledChannels);

    void drawGrid(const AxesLimits& limits, const std::vector<Tick>& xTicks, const std::vector<Tick>& yTicks);

    void drawData(const AxesLimits& limits, const std::vector<std::span<const juce::Point<GLfloat>>>& channels, const std::vector<bool>& enabledChannels);

    void drawXTicks(const juce::Rectangle<int>& bounds, int yTicksLeftEdge, const AxisLimits& limits, const std::vector<Tick>& ticks);

    void drawYTicks(const juce::Rectangle<int>& bounds, const AxisLimits& limits, const std::vector<Tick>& ticks);

    void drawTicks(bool isXTicks, const juce::Rectangle<int>& plotBounds, const juce::Rectangle<int>& drawBounds, const AxisLimits& limits, const std::vector<Tick>& ticks);

    // Copy of juce::jmap with assertion for exactlyEqual instead of approximatelyEqual
    template<typename Type>
    Type mapRange(Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
    {
        jassert (!juce::exactlyEqual(sourceRangeMax, sourceRangeMin)); // mapping from a range of zero will produce NaN!
        return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Graph)
};
