#include "../WindowIDs.h"
#include "GraphWindow.h"

const juce::String GraphWindow::degreeSymbol = (juce::CharPointer_UTF8("\xc2\xba"));

GraphWindow::GraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer, const juce::String& yAxis, const std::vector<Graph::LegendItem>& legend, Graph::Settings& settings_)
        : Window(devicePanel_, windowLayout_, type_, getWindowTitle(type_) + " Graph Menu", std::bind(&GraphWindow::getMenu, this)),
          settings(settings_),
          graph(glRenderer, yAxis, legend, settings)
{
    addAndMakeVisible(graph);
}

void GraphWindow::resized()
{
    Window::resized();

    graph.setBounds(getContentBounds());
}

void GraphWindow::mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel)
{
    static constexpr auto deltaScale = 2.0f;

    if (mouseEvent.mods.isAltDown())
    {
        const auto delta = juce::jmap<float>(wheel.deltaY, 0.0f, deltaScale, 0.0f, settings.vertical.max - settings.vertical.min);
        settings.vertical.max = settings.vertical.max - delta;
        settings.vertical.min = settings.vertical.min + delta;
    }
    else
    {
        const auto delta = juce::jmap<float>(wheel.deltaY, 0.0f, deltaScale, 0.0f, settings.horizontal.max - settings.horizontal.min);
        settings.horizontal.min = std::min(0.0f, settings.horizontal.min + delta);
    }
}

void GraphWindow::update(const uint64_t timestamp, const std::vector<float>& arguments)
{
    if (isPaused)
    {
        return;
    }

    graph.update(timestamp, arguments);
}

juce::PopupMenu GraphWindow::getMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Restore Defaults", true, false, [&]
    {
        settings = {};
        isPaused = false;
    });
    menu.addItem("Clear", true, false, [&]
    {
        graph.clear();
    });
    menu.addItem("Pause", true, isPaused, [&]
    {
        isPaused = !isPaused;
    });
    menu.addItem("Horizontal Autoscale", true, settings.horizontal.autoscale, [&]
    {
        settings.horizontal.autoscale = !settings.horizontal.autoscale;
    });
    menu.addItem("Vertical Autoscale", true, settings.vertical.autoscale, [&]
    {
        settings.vertical.autoscale = !settings.vertical.autoscale;
    });

    return menu;
}
