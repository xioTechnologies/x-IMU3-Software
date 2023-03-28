#pragma once

#include "../../DevicePanel/DevicePanel.h"
#include "../../OpenGL/GLRenderer.h"
#include "../../OpenGL/Graph.h"
#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>

class GraphWindow : public Window
{
public:
    GraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer, const juce::String& yAxis, const std::vector<Graph::LegendItem>& legend, Graph::Settings& settings_);

    void resized() override;

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

protected:
    void update(const uint64_t timestamp, const std::vector<float>& arguments);

    static const juce::String degreeSymbol;

private:
    Graph::Settings& settings;

    Graph graph;
    std::atomic<bool> isPaused = false;

    juce::PopupMenu getMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWindow)
};
