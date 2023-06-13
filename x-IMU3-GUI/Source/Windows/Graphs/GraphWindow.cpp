#include "../WindowIDs.h"
#include "GraphWindow.h"
#include "../../Widgets/PopupMenuHeader.h"

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
    if (mouseEvent.mods.isAltDown())
    {
        zoomVertical(wheel.deltaY > 0);
    }
    else
    {
        zoomHorizontal(wheel.deltaY > 0);
    }
}

void GraphWindow::update(const uint64_t timestamp, const std::vector<float>& arguments)
{
    if (paused)
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
        paused = false;
    });
    menu.addItem("Clear", true, false, [&]
    {
        graph.clear();
    });
    menu.addItem("Pause", true, paused, [&]
    {
        paused = !paused;
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("HORIZONTAL"), nullptr);
    menu.addItem("Autoscale", true, settings.horizontal.autoscale, [&]
    {
        settings.horizontal.autoscale = !settings.horizontal.autoscale;
    });
    menu.addItem("Zoom In (Scroll)", settings.horizontal.autoscale == false, false, [&]
    {
        zoomHorizontal(true);
    });
    menu.addItem("Zoom Out (Scroll)", settings.horizontal.autoscale == false, false, [&]
    {
        zoomHorizontal(false);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VERTICAL"), nullptr);
    menu.addItem("Autoscale", true, settings.vertical.autoscale, [&]
    {
        settings.vertical.autoscale = !settings.vertical.autoscale;
    });
    menu.addItem("Zoom In (Alt+Scroll)", settings.vertical.autoscale == false, false, [&]
    {
        zoomVertical(true);
    });
    menu.addItem("Zoom Out (Alt+Scroll)", settings.vertical.autoscale == false, false, [&]
    {
        zoomVertical(false);
    });

    if (graph.getLegend().size() > 1)
    {
        menu.addSeparator();
        menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);

        bool allTicked = true;
        for (auto& line : settings.visibleLines)
        {
            allTicked = allTicked && line;
        }
        menu.addItem("All", true, allTicked, [&, allTicked]
        {
            for (auto& line : settings.visibleLines)
            {
                line = !allTicked;
            }
        });

        for (size_t index = 0; index < graph.getLegend().size(); index++)
        {
            menu.addItem(graph.getLegend()[index].label, true, settings.visibleLines[index], [&, index]
            {
                settings.visibleLines[index] = settings.visibleLines[index] == false;
            });
        }
    }

    return menu;
}

void GraphWindow::zoomHorizontal(const bool direction)
{
    const auto amount = (settings.horizontal.max - settings.horizontal.min) * 0.125f;
    settings.horizontal.min = settings.horizontal.min + (direction ? amount : -amount);
}

void GraphWindow::zoomVertical(const bool direction)
{
    const auto amount = (settings.vertical.max - settings.vertical.min) * 0.125f;
    settings.vertical.max = settings.vertical.max - (direction ? amount : -amount);
    settings.vertical.min = settings.vertical.min + (direction ? amount : -amount);
}
