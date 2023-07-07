#pragma once

#include "../../DevicePanel/DevicePanel.h"
#include "../../OpenGL/Common/GLRenderer.h"
#include "../../OpenGL/Graph.h"
#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>

class GraphWindow : public Window
{
public:
    GraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer, const juce::String& yAxis, const std::vector<Graph::LegendItem>& legend, Graph::Settings& settings_, const Graph::Settings& defaultSettings_);

    void resized() override;

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

protected:
    void update(const uint64_t timestamp, const std::vector<float>& arguments);

    static const juce::String degreeSymbol;

private:
    Graph::Settings& settings;
    const Graph::Settings defaultSettings;

    Graph graph;

    juce::PopupMenu getMenu();

    void zoomHorizontal(const float multiplier);

    void zoomVertical(const float multiplier);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWindow)
};
