#pragma once

#include "OpenGL/Graph.h"
#include "Widgets/SimpleLabel.h"
#include "Windows/Window.h"
#include "Ximu3.hpp"

class GraphWindow : public Window, private juce::AsyncUpdater
{
public:
    GraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_,
                GLRenderer& glRenderer,
                const juce::String& yAxis,
                std::vector<juce::String> legendStrings_,
                std::vector<juce::Colour> legendColours_,
                bool defaultHorizontalAutoscale_);

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

    void mouseDown([[maybe_unused]] const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseDoubleClick(const juce::MouseEvent& mouseEvent) override;

protected:
    void update(const uint64_t timestamp, const std::vector<float>& arguments);

    static const juce::String degreeSymbol;

private:
    static constexpr int labelHeight = 22;
    static constexpr int rightMargin = 10;

    const std::vector<juce::String> legendStrings;
    const std::vector<juce::Colour> legendColours;
    const bool defaultHorizontalAutoscale;
    const int numberOfChannels = (int) legendStrings.size();

    Graph graph;

    SimpleLabel xLabel;
    SimpleLabel yLabel;

    bool paused = settingsTree["paused"];

    bool compactView = false;

    // Graph state for mouse dragging
    Graph::Settings graphSettingsMouseCache;
    float plotWidthJUCEPixelsMouseCache = 1.0f;
    float plotHeightJUCEPixelsMouseCache = 1.0f;

    void writeToValueTree(const Graph::Settings& settings);

    Graph::Settings readFromValueTree() const;

    void zoomHorizontal(const float multiplier);

    void zoomVertical(const float multiplier);

    juce::PopupMenu getMenu() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    void handleAsyncUpdate() override;
};
