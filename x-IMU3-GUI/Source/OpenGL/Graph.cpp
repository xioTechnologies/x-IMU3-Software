#include "Graph.h"

Graph::Graph(GLRenderer& renderer_, const std::vector<juce::Colour>& colours_, const int legendHeight_, const int rightMargin_)
        : OpenGLComponent(renderer_.getContext()),
          renderer(renderer_),
          colours(colours_),
          legendHeight(legendHeight_),
          rightMargin(rightMargin_)
{
    renderer.addComponent(*this);

    setSettings(settings);
}

Graph::~Graph()
{
    renderer.removeComponent(*this);
}

void Graph::render()
{
    std::scoped_lock _(settingsMutex);

    auto bounds = getBoundsInMainWindow();

    const auto channels = buffer.read();

    if (scaleToFitPending.exchange(false))
    {
        settings.axesLimits.autoscale(true, true, channels, settings.enabledChannels);
    }

    settings.axesLimits.autoscale(settings.horizontalAutoscale, settings.verticalAutoscale, channels, settings.enabledChannels);

    GLHelpers::clear(UIColours::backgroundLight, toOpenGLBounds(bounds)); // paint graph background color

    if (ticksEnabled)
    {
        // Remove top/right margins from graph plot
        bounds.removeFromRight(rightMargin);
        bounds.removeFromTop(legendHeight);

        static constexpr auto xTickMargin = 2;
        static constexpr auto yTickMargin = 5;

        auto xTicksBounds = bounds.removeFromBottom(renderer.getResources().getGraphTickText().getFontSizeJucePixels() + 1); // font height
        bounds.removeFromBottom(xTickMargin);

        const auto yTicks = createTicks(bounds.getHeight(), settings.axesLimits.y);

        const auto yTicksWidth = getMaximumStringWidth(yTicks, renderer.getResources().getGraphTickText());
        const auto yTicksBounds = bounds.removeFromLeft(yTicksWidth);
        bounds.removeFromLeft(yTickMargin);

        xTicksBounds.removeFromLeft(yTicksWidth);
        xTicksBounds.removeFromLeft(yTickMargin);

        const auto xTicks = createTicks(bounds.getWidth(), settings.axesLimits.x);

        drawXTicks(xTicksBounds, yTicksBounds.getX(), settings.axesLimits.x, xTicks);
        drawYTicks(yTicksBounds, settings.axesLimits.y, yTicks);
        drawPlot(bounds, settings.axesLimits, xTicks, yTicks, channels, settings.enabledChannels);
    }
    else
    {
        const auto xTicks = createTicks(bounds.getWidth(), settings.axesLimits.x);
        const auto yTicks = createTicks(bounds.getHeight(), settings.axesLimits.y);
        drawPlot(bounds, settings.axesLimits, xTicks, yTicks, channels, settings.enabledChannels);
    }

    plotWidthJUCEPixels = (float) bounds.getWidth();
    plotHeightJUCEPixels = (float) bounds.getHeight();
}

void Graph::setSettings(Settings settings_)
{
    std::scoped_lock _(settingsMutex);
    settings = settings_;
}

Graph::Settings Graph::getSettings() const
{
    std::scoped_lock _(settingsMutex);
    return settings;
}

void Graph::setTicksEnabled(const bool enabled)
{
    ticksEnabled = enabled;
}

void Graph::scaleToFit()
{
    scaleToFitPending = true;
}

void Graph::clear()
{
    buffer.clear();
}

void Graph::update(const uint64_t timestamp, const std::vector<float>& values)
{
    buffer.write(timestamp, values);
}

int Graph::getMaximumStringWidth(const std::vector<Tick>& ticks, const Text& text)
{
    int maxStringWidth = 0;
    for (const auto& tick : ticks)
    {
        maxStringWidth = std::max(maxStringWidth, (int) std::ceil(text.getStringWidthJucePixels(tick.label)));
    }
    return maxStringWidth;
}

void Graph::drawPlot(const juce::Rectangle<int>& bounds, const AxesLimits& limits, const std::vector<Tick>& xTicks, const std::vector<Tick>& yTicks, const std::vector<std::span<const juce::Point<GLfloat>>>& channels, const std::vector<bool>& enabledChannels)
{
    // Set rendering bounds
    const auto glBounds = toOpenGLBounds(bounds);
    GLHelpers::ScopedCapability scopedScissor(juce::gl::GL_SCISSOR_TEST, true);
    GLHelpers::viewportAndScissor(glBounds);

    // Setup OpenGL state
    GLHelpers::ScopedCapability scopedCull(juce::gl::GL_CULL_FACE, false);
    GLHelpers::ScopedCapability scopedDepth(juce::gl::GL_DEPTH_TEST, false);

    /*  NOTE: We are using OpenGL's built-in line rendering system, GL_LINES. By using this system, we are
     *  limited to drawing lines with a width of 1 pixel. The OpenGL spec only requires graphics drivers
     *  implement line widths of 1 pixel, allowing optional implementation of more widths. To have consistent
     *  line drawing behavior across devices, we can only really use glLineWidth of 1.0.
     *
     *  If we want to increase line width in the future, we will need to create our own line system using
     *  elongated quads.
     *
     *  Refs:
     *  - https://registry.khronos.org/OpenGL-Refpages/gl4/html/glLineWidth.xhtml "Only width 1 is guaranteed to be supported" and this is often the case.
     *  - https://mattdesl.svbtle.com/drawing-lines-is-hard
     */
    juce::gl::glLineWidth(1.0f);

    // Draw
    drawGrid(limits, xTicks, yTicks);
    drawData(limits, channels, enabledChannels);
}

void Graph::drawGrid(const AxesLimits& limits, const std::vector<Tick>& xTicks, const std::vector<Tick>& yTicks)
{
    auto addGridLines = [](std::vector<GLfloat>& linesToAddTo, bool areVertical, const std::vector<Tick>& ticks, const AxisLimits& axisLimits)
    {
        // Add line to grid based on position in graph units
        auto addLine = [&](float position, bool isMajorTick)
        {
            const float tickBrightness = isMajorTick ? majorTickBrightness : minorTickBrightness;
            const float ndcPosition = (((position - axisLimits.min) / axisLimits.getRange()) * 2.0f) - 1.0f;
            if (areVertical)
            {
                linesToAddTo.insert(linesToAddTo.end(), { ndcPosition, -1.0f, tickBrightness, ndcPosition, 1.0f, tickBrightness });
            }
            else
            {
                linesToAddTo.insert(linesToAddTo.end(), { -1.0f, ndcPosition, tickBrightness, 1.0f, ndcPosition, tickBrightness });
            }
        };

        for (auto& tick : ticks)
        {
            addLine(tick.value, tick.isMajor);
        }
    };

    // Compute line vertices for LineBuffer
    std::vector<GLfloat> lines;
    addGridLines(lines, true, xTicks, limits.x); // vertical x ticks
    addGridLines(lines, false, yTicks, limits.y); // horizontal y ticks

    if (ticksEnabled)
    {
        // Border lines
        lines.insert(lines.end(), {
                -1.0f, -1.0f, borderBrightness, -1.0f, 1.0f, borderBrightness, // left edge
                1.0f, -1.0f, borderBrightness, 1.0f, 1.0f, borderBrightness, // right edge
                -1.0f, 1.0f, borderBrightness, 1.0f, 1.0f, borderBrightness, // top edge
                -1.0f, -1.0f, borderBrightness, 1.0f, -1.0f, borderBrightness // bottom edge
        });
    }

    // Draw lines
    GLHelpers::ScopedCapability _(juce::gl::GL_LINE_SMOOTH, false); // provides sharper horizontal/vertical lines

    renderer.getResources().graphGridShader.use();
    auto& gridBuffer = renderer.getResources().graphGridBuffer;
    gridBuffer.fillBuffers(lines);
    gridBuffer.draw(juce::gl::GL_LINES);
}

void Graph::drawData(const AxesLimits& limits, const std::vector<std::span<const juce::Point<GLfloat>>>& channels, const std::vector<bool>& enabledChannels)
{
    if ((channels.size() != enabledChannels.size()) || (channels.size() != colours.size()))
    {
        jassertfalse;
        return;
    }

    const auto& graphDataShader = renderer.getResources().graphDataShader;
    auto& graphDataBuffer = renderer.getResources().graphDataBuffer;
    graphDataShader.use();
    graphDataShader.axisLimitsRange.set({ limits.x.getRange(), limits.y.getRange() });
    graphDataShader.axisLimitsMin.set({ limits.x.min, limits.y.min });

    for (size_t index = 0; index < channels.size(); index++)
    {
        if (enabledChannels[index] == false)
        {
            continue;
        }

        graphDataShader.colour.setRGBA(colours[index]);
        graphDataBuffer.fillBuffers(channels[index]);
        graphDataBuffer.draw(juce::gl::GL_LINE_STRIP);
    }
}

void Graph::drawXTicks(const juce::Rectangle<int>& bounds, int yTicksLeftEdge, const AxisLimits& limits, const std::vector<Tick>& ticks)
{
    // Expand drawing bounds to allow text to be drawn past the corners of the plot.
    const auto drawBounds = bounds.withRight(bounds.getRight() + rightMargin).withLeft(yTicksLeftEdge);
    drawTicks(true, bounds, drawBounds, limits, ticks);
}

void Graph::drawYTicks(const juce::Rectangle<int>& bounds, const AxisLimits& limits, const std::vector<Tick>& ticks)
{
    // Expand drawing bounds to allow text to be drawn past the corners of the plot.
    const auto drawBounds = bounds.expanded(0, legendHeight);
    drawTicks(false, bounds, drawBounds, limits, ticks);
}

void Graph::drawTicks(bool isXTicks, const juce::Rectangle<int>& plotBounds, const juce::Rectangle<int>& drawBounds, const AxisLimits& limits, const std::vector<Tick>& ticks)
{
    // Set rendering bounds, expanded to allow drawing past graph edges
    const auto glPlotBounds = toOpenGLBounds(plotBounds); // only plot area
    const auto glDrawBounds = toOpenGLBounds(drawBounds); // full area allowed to draw text
    GLHelpers::ScopedCapability scopedScissor(juce::gl::GL_SCISSOR_TEST, true);
    GLHelpers::viewportAndScissor(glDrawBounds);

    const auto& text = renderer.getResources().getGraphTickText();
    const int distanceOfPlotAxis = isXTicks ? glPlotBounds.getWidth() : glPlotBounds.getHeight();
    const int plotStartOffset = isXTicks ? (glPlotBounds.getX() - glDrawBounds.getX()) : (glPlotBounds.getY() - glDrawBounds.getY());

    // Collect only text labels from std::vector<Tick>
    auto labelsToDraw = ticks;
    labelsToDraw.erase(std::remove_if(labelsToDraw.begin(), labelsToDraw.end(), [&](auto& tick)
    {
        return tick.label.isEmpty();
    }), labelsToDraw.end());

    // For X-axis, hide tick labels that extend out of bounds or overlap
    if (isXTicks)
    {
        auto getLabelEdges = [&](const Tick& tick) -> std::tuple<float, float>
        {
            const auto centreX = mapRange(tick.value, limits.min, limits.max, 0.0f, (float) distanceOfPlotAxis) + (float) plotStartOffset + (float) glDrawBounds.getX();
            const auto leftEdgeX = centreX - (text.getStringWidthGLPixels(tick.label) / 2.0f);
            const auto rightEdgeX = centreX + (text.getStringWidthGLPixels(tick.label) / 2.0f);
            return { leftEdgeX, rightEdgeX };
        };

        // Remove any tick text which would extend past the edges of the drawing bounds
        labelsToDraw.erase(std::remove_if(labelsToDraw.begin(), labelsToDraw.end(), [&](auto& tick)
        {
            const auto [leftEdgeX, rightEdgeX] = getLabelEdges(tick);
            return leftEdgeX < (float) glDrawBounds.getX() || rightEdgeX > (float) glDrawBounds.getRight();
        }), labelsToDraw.end());

        auto areAnyLabelsTooClose = [&]
        {
            // Check each pair of labels to see if they are too close
            constexpr float minimumSpaceBetweenLabels = 8.0f;
            for (size_t index = 0; index < labelsToDraw.size() - 1; index++)
            {
                const auto [label1LeftEdge, label1RightEdge] = getLabelEdges(labelsToDraw[index]);
                const auto [label2LeftEdge, label2RightEdge] = getLabelEdges(labelsToDraw[index + 1]);
                if (label1RightEdge + minimumSpaceBetweenLabels > label2LeftEdge)
                {
                    return true;
                }
            }

            return false;
        };

        if (labelsToDraw.size() > 1)
        {
            // If labels are too close, remove every other label (halve the number of labels) until there are no overlaps
            const auto halvingCount = static_cast<int> (std::ceil(std::log2(labelsToDraw.size())));
            for (int i = 0; (i < halvingCount) && areAnyLabelsTooClose(); i++)
            {
                // Erase every other element (odd indices), except for "0" if it is displayed
                labelsToDraw.erase(std::remove_if(labelsToDraw.begin(), labelsToDraw.end(), [&](auto& tick)
                {
                    return ((&tick - &*labelsToDraw.begin()) % 2) && tick.label != "0";
                }), labelsToDraw.end());
            }
        }
    }

    // Draw each text string
    for (const auto& tick : labelsToDraw)
    {
        const auto offsetAlongAxis = mapRange(tick.value, limits.min, limits.max, 0.0f, (float) distanceOfPlotAxis) + (float) plotStartOffset;
        const auto offsetTowardsAxis = isXTicks ? (float) (glDrawBounds.getHeight() - (int) text.getFontSizeGLPixels()) : (float) glDrawBounds.getWidth();
        const auto positionRelative = isXTicks ? glm::vec2(offsetAlongAxis, offsetTowardsAxis) : glm::vec2(offsetTowardsAxis, offsetAlongAxis);
        const auto screenPosition = positionRelative + glm::vec2(glDrawBounds.getX(), glDrawBounds.getY());
        text.draw(renderer.getResources(), tick.label, juce::Colours::grey, isXTicks ? juce::Justification::horizontallyCentred : juce::Justification::centredRight, screenPosition, glDrawBounds);
    }
}
