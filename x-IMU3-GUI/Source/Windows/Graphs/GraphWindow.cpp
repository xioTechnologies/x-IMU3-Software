#include "DevicePanel/DevicePanel.h"
#include "GraphWindow.h"
#include "Widgets/PopupMenuHeader.h"

const juce::String GraphWindow::degreeSymbol = (juce::CharPointer_UTF8("\xc2\xba"));

GraphWindow::GraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_,
                         GLRenderer& glRenderer,
                         const juce::String& yAxis,
                         std::vector<juce::String> legendStrings_,
                         std::vector<juce::Colour> legendColours_,
                         juce::ValueTree settingsTree_,
                         bool defaultHorizontalAutoscale_)
        : Window(windowLayout_, type_, devicePanel_, "", std::bind(&GraphWindow::getMenu, this)),
          legendStrings(legendStrings_),
          legendColours(legendColours_),
          defaultHorizontalAutoscale(defaultHorizontalAutoscale_),
          settingsTree(settingsTree_),
          graph(glRenderer, legendColours_, labelHeight, rightMargin),
          xLabel("Time (s)", UIFonts::getDefaultFont(), juce::Justification::centred),
          yLabel(yAxis, UIFonts::getDefaultFont(), juce::Justification::centred)
{
    jassert(legendStrings.size() == legendColours.size());

    addAndMakeVisible(graph);
    addAndMakeVisible(xLabel);
    addAndMakeVisible(yLabel);

    settingsTree.addListener(this);
    graph.setSettings(readFromValueTree());
}

void GraphWindow::paint(juce::Graphics& g)
{
    if (compactView)
    {
        return;
    }

    // Draw background
    juce::RectangleList<int> backgroundBounds(getLocalBounds());
    backgroundBounds.subtract(graph.getBounds());
    g.setColour(UIColours::backgroundLight);
    g.fillRectList(backgroundBounds);

    // Draw legend
    static constexpr int margin = 10;

    const auto settings = readFromValueTree();
    auto legendBounds = getContentBounds().removeFromTop(labelHeight).reduced(rightMargin, 0);
    const auto font = UIFonts::getDefaultFont();
    g.setFont(font);

    for (int index = numberOfChannels - 1; index >= 0; index--)
    {
        const auto legendString = legendStrings[(size_t) index];
        g.setColour(settings.enabledChannels[(size_t) index] ? legendColours[(size_t) index] : juce::Colours::grey);
        g.drawText(legendString, legendBounds.removeFromRight(font.getStringWidth(legendString) + margin), juce::Justification::centredRight);
    }
}

void GraphWindow::resized()
{
    Window::resized();

    auto graphArea = getContentBounds();

    compactView = std::min(getWidth(), getHeight()) < 200;

    xLabel.setVisible(compactView == false);
    yLabel.setVisible(compactView == false);

    if (compactView == false)
    {
        xLabel.setBounds(graphArea.removeFromBottom(labelHeight).withTrimmedLeft(labelHeight));

        // Rotate Y label vertical
        const auto yLabelBounds = graphArea.removeFromLeft(labelHeight);
        yLabel.setTransform({}); // prevent glitch on subsequent resize
        yLabel.setSize(yLabelBounds.getHeight(), yLabelBounds.getWidth()); // invert width/height
        yLabel.setCentrePosition(0, 0);
        yLabel.setTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi).translated((float) yLabelBounds.getCentreX(), (float) yLabelBounds.getCentreY()));
    }

    graph.setBounds(graphArea);
    graph.setTicksEnabled(compactView == false);
}

void GraphWindow::mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel)
{
    if (mouseEvent.mods.isAltDown())
    {
        zoomVertical(std::exp2(-wheel.deltaY));
    }
    else
    {
        zoomHorizontal(std::exp2(-wheel.deltaY));
    }
}

void GraphWindow::mouseDown([[maybe_unused]] const juce::MouseEvent& mouseEvent)
{
    // Cache graph state for mouseDrag
    graphSettingsMouseCache = graph.getSettings();
    plotWidthJUCEPixelsMouseCache = graph.plotWidthJUCEPixels;
    plotHeightJUCEPixelsMouseCache = graph.plotHeightJUCEPixels;

    if (mouseEvent.mods.isPopupMenu())
    {
        getMenu().show();
    }
}

void GraphWindow::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    auto settings = graph.getSettings();
    if (settings.horizontalAutoscale && settings.verticalAutoscale)
    {
        return;
    }

    if (plotWidthJUCEPixelsMouseCache <= 0.0f || plotHeightJUCEPixelsMouseCache <= 0.0f)
    {
        jassertfalse;
        return;
    }

    auto dragOffsetPixels = mouseEvent.position - mouseEvent.mouseDownPosition;

    if (settings.horizontalAutoscale == false)
    {
        auto limits = graphSettingsMouseCache.axesLimits.x;
        auto offset = -1.0f * (dragOffsetPixels.x * (limits.getRange() / plotWidthJUCEPixelsMouseCache));
        offset = juce::jlimit(AxisLimits::minValue - limits.min, -limits.max, offset); // constrain drag from AxisLimits::minValue to 0 but keep range
        limits.min += offset;
        limits.max += offset;
        settings.axesLimits.x = limits;
    }
    if (settings.verticalAutoscale == false)
    {
        auto limits = graphSettingsMouseCache.axesLimits.y;
        auto offset = dragOffsetPixels.y * (limits.getRange() / plotHeightJUCEPixelsMouseCache);
        offset = juce::jlimit(AxisLimits::minValue - limits.min, AxisLimits::maxValue - limits.max, offset); // constrain drag from AxisLimits::minValue to +AxisLimits::maxValue but keep range
        limits.min += offset;
        limits.max += offset;
        settings.axesLimits.y = limits;
    }

    writeToValueTree(settings);
}

void GraphWindow::mouseDoubleClick([[maybe_unused]] const juce::MouseEvent& mouseEvent)
{
    settingsTree.setProperty("scaleToFit", (bool) settingsTree["scaleToFit"] == false, nullptr);
}

void GraphWindow::update(const uint64_t timestamp, const std::vector<float>& arguments)
{
    if (paused)
    {
        return;
    }
    graph.update(timestamp, arguments);
}

void GraphWindow::writeToValueTree(const Graph::Settings& settings)
{
    settingsTree.setProperty("axesLimitsXMin", settings.axesLimits.x.min, nullptr);
    settingsTree.setProperty("axesLimitsXMax", settings.axesLimits.x.max, nullptr);
    settingsTree.setProperty("axesLimitsYMin", settings.axesLimits.y.min, nullptr);
    settingsTree.setProperty("axesLimitsYMax", settings.axesLimits.y.max, nullptr);
    settingsTree.setProperty("horizontalAutoscale", settings.horizontalAutoscale, nullptr);
    settingsTree.setProperty("verticalAutoscale", settings.verticalAutoscale, nullptr);
    for (int index = 0; index < numberOfChannels; index++)
    {
        settingsTree.setProperty("enabledChannels" + juce::String(index), false, nullptr);
    }
    for (size_t index = 0; index < settings.enabledChannels.size(); index++)
    {
        settingsTree.setProperty("enabledChannels" + juce::String(index), (bool) settings.enabledChannels[index], nullptr);
    }
}

Graph::Settings GraphWindow::readFromValueTree() const
{
    Graph::Settings settings;
    settings.axesLimits.x.min = settingsTree.getProperty("axesLimitsXMin", settings.axesLimits.x.min);
    settings.axesLimits.x.max = settingsTree.getProperty("axesLimitsXMax", settings.axesLimits.x.max);
    settings.axesLimits.y.min = settingsTree.getProperty("axesLimitsYMin", settings.axesLimits.y.min);
    settings.axesLimits.y.max = settingsTree.getProperty("axesLimitsYMax", settings.axesLimits.y.max);
    settings.horizontalAutoscale = settingsTree.getProperty("horizontalAutoscale", defaultHorizontalAutoscale);
    settings.verticalAutoscale = settingsTree.getProperty("verticalAutoscale", settings.verticalAutoscale);
    for (size_t index = 0; index < (size_t) numberOfChannels; index++)
    {
        settings.enabledChannels.push_back(settingsTree.getProperty("enabledChannels" + juce::String(index), true));
    }
    return settings;
}

void GraphWindow::zoomHorizontal(const float multiplier)
{
    auto settings = graph.getSettings();
    auto limits = settings.axesLimits.x;
    settings.axesLimits.x = { limits.max - (limits.getRange() * multiplier), limits.max };

    writeToValueTree(settings);
}

void GraphWindow::zoomVertical(const float multiplier)
{
    auto settings = graph.getSettings();
    const auto yLimits = settings.axesLimits.y;
    const auto range = yLimits.getRange();
    const auto offset = (multiplier * range - range) / 2;
    settings.axesLimits.y = { yLimits.min - offset, yLimits.max + offset };

    writeToValueTree(settings);
}

juce::PopupMenu GraphWindow::getMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Restore Defaults", true, false, [this]
    {
        settingsTree.removeProperty("paused", nullptr);

        Graph::Settings settings;
        settings.horizontalAutoscale = defaultHorizontalAutoscale;
        settings.enabledChannels.resize((size_t) numberOfChannels, true);
        writeToValueTree(settings);
    });
    menu.addItem("Scale to Fit (Double Click)", (graph.getSettings().horizontalAutoscale == false) || (graph.getSettings().verticalAutoscale == false), false, [this]
    {
        settingsTree.setProperty("scaleToFit", (bool) settingsTree["scaleToFit"] == false, nullptr);
    });
    menu.addItem("Clear", true, false, [this]
    {
        settingsTree.setProperty("clear", (bool) settingsTree["clear"] == false, nullptr);
    });
    menu.addItem("Pause", true, paused, [this]
    {
        settingsTree.setProperty("paused", (bool) settingsTree["paused"] == false, nullptr);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("HORIZONTAL"), nullptr);
    menu.addItem("Autoscale", true, graph.getSettings().horizontalAutoscale, [this]
    {
        auto settings = graph.getSettings();
        settings.horizontalAutoscale = !settings.horizontalAutoscale;
        writeToValueTree(settings);
    });
    menu.addItem("Move Left (Drag)", graph.getSettings().horizontalAutoscale == false, false, [this]
    {
        auto settings = graph.getSettings();
        const auto offset = settings.axesLimits.x.getRange() / 2;
        settings.axesLimits.x.min -= offset;
        settings.axesLimits.x.max -= offset;
        writeToValueTree(settings);
    });
    menu.addItem("Move Right (Drag)", graph.getSettings().horizontalAutoscale == false, false, [this]
    {
        auto settings = graph.getSettings();
        const auto offset = settings.axesLimits.x.getRange() / 2;
        settings.axesLimits.x.min += offset;
        settings.axesLimits.x.max += offset;
        writeToValueTree(settings);
    });
    menu.addItem("Zoom In (Scroll)", graph.getSettings().horizontalAutoscale == false, false, [this]
    {
        zoomHorizontal(0.5f);
    });
    menu.addItem("Zoom Out (Scroll)", graph.getSettings().horizontalAutoscale == false, false, [this]
    {
        zoomHorizontal(2.0f);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VERTICAL"), nullptr);
    menu.addItem("Autoscale", true, graph.getSettings().verticalAutoscale, [this]
    {
        auto settings = graph.getSettings();
        settings.verticalAutoscale = !settings.verticalAutoscale;
        writeToValueTree(settings);
    });
    menu.addItem("Move Up (Drag)", graph.getSettings().verticalAutoscale == false, false, [this]
    {
        auto settings = graph.getSettings();
        const auto offset = settings.axesLimits.y.getRange() / 2;
        settings.axesLimits.y.min += offset;
        settings.axesLimits.y.max += offset;
        writeToValueTree(settings);
    });
    menu.addItem("Move Down (Drag)", graph.getSettings().verticalAutoscale == false, false, [this]
    {
        auto settings = graph.getSettings();
        const auto offset = settings.axesLimits.y.getRange() / 2;
        settings.axesLimits.y.min -= offset;
        settings.axesLimits.y.max -= offset;
        writeToValueTree(settings);
    });
    menu.addItem("Zoom In (Alt+Scroll)", graph.getSettings().verticalAutoscale == false, false, [this]
    {
        zoomVertical(0.5f);
    });
    menu.addItem("Zoom Out (Alt+Scroll)", graph.getSettings().verticalAutoscale == false, false, [this]
    {
        zoomVertical(2.0f);
    });

    if (numberOfChannels > 1)
    {
        menu.addSeparator();
        menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);

        auto settings = graph.getSettings();

        bool allTicked = true;
        for (auto const line : settings.enabledChannels)
        {
            allTicked = allTicked && line;
        }
        menu.addItem("All", true, allTicked, [this, allTicked]
        {
            auto settings_ = graph.getSettings();
            for (auto&& line : settings_.enabledChannels)
            {
                line = !allTicked;
            }
            writeToValueTree(settings_);
            repaint(); // refresh legend text color
        });

        for (size_t index = 0; index < (size_t) numberOfChannels; index++)
        {
            menu.addItem(legendStrings[index], true, settings.enabledChannels[index], [this, index]
            {
                auto settings_ = graph.getSettings();
                settings_.enabledChannels[index] = settings_.enabledChannels[index] == false;
                writeToValueTree(settings_);
            });
        }
    }

    return menu;
}

void GraphWindow::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (property.toString() == "scaleToFit")
    {
        graph.scaleToFit();
        return;
    }

    if (property.toString() == "clear")
    {
        graph.clear();
        return;
    }

    if (property.toString() == "paused")
    {
        paused = treeWhosePropertyHasChanged["paused"];
        return;
    }

    if (property.toString().startsWith("enabledChannels"))
    {
        repaint();
    }

    graph.setSettings(readFromValueTree());
}
