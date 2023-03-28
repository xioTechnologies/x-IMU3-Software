#include "../Convert.h"
#include "../CustomLookAndFeel.h"
#include "AxesRange.h"
#include "Graph.h"

Graph::Settings::Settings(const bool horizontalAutoscale, const float horizontalMin, const float horizontalMax,
                          const bool verticalAutoscale, const float verticalMin, const float verticalMax)
{
    horizontal.autoscale = horizontalAutoscale;
    horizontal.min = horizontalMin;
    horizontal.max = horizontalMax;
    vertical.autoscale = verticalAutoscale;
    vertical.min = verticalMin;
    vertical.max = verticalMax;
}

Graph::Settings& Graph::Settings::operator=(const Graph::Settings& other)
{
    horizontal.autoscale = other.horizontal.autoscale.load();
    horizontal.min = other.horizontal.min.load();
    horizontal.max = other.horizontal.max.load();
    vertical.autoscale = other.vertical.autoscale.load();
    vertical.min = other.vertical.min.load();
    vertical.max = other.vertical.max.load();
    return *this;
}

Graph::Graph(GLRenderer& renderer_, const juce::String& yAxis_, const std::vector<LegendItem>& legend_, const Settings& settings_)
        : OpenGLComponent(renderer_.getContext()),
          renderer(renderer_),
          yAxis(yAxis_),
          legend(legend_),
          settings(settings_)
{
    renderer.addComponent(*this);
}

Graph::~Graph()
{
    renderer.removeComponent(*this);
}

void Graph::render()
{
    const auto bounds = toOpenGLBounds(getBoundsInMainWindow());

    renderer.refreshScreen(UIColours::backgroundLight, bounds);

    AxesRange axesRange;
    axesRange.xMin = settings.horizontal.min;
    axesRange.xMax = settings.horizontal.max;
    axesRange.yMin = settings.vertical.min;
    axesRange.yMax = settings.vertical.max;

    axesRange = graphDataBuffer.update(axesRange, settings.horizontal.autoscale, settings.vertical.autoscale);

    const auto numberOfDecimalPlacesX = getNumberOfDecimalPlaces((float) axesRange.xMin, (float) axesRange.xMax);
    const auto numberOfDecimalPlacesY = getNumberOfDecimalPlaces((float) axesRange.yMin, (float) axesRange.yMax);
    const auto majorYPositions = gridLines.getMajorYPositions();

    const auto getAxisValueAsString = [](auto value, auto numberOfDecimalPlaces)
    {
        if (std::fabs(value) < 1E-6f) // TODO: Redesign so that axis value is precisely zero
        {
            return juce::String("0");
        }
        return numberOfDecimalPlaces == 0 ? juce::String(juce::roundToInt(value)) : juce::String(value, numberOfDecimalPlaces);
    };

    extraLeftPadding = [&]
    {
        int padding = 0;

        for (const auto value : majorYPositions)
        {
            auto& text = renderer.getResources().getGraphAxisValuesText();
            text.setText(getAxisValueAsString(value, numberOfDecimalPlacesY));
            padding = std::max(padding, (int) text.getTotalWidth());
        }

        return juce::roundToInt(padding / context.getRenderingScale());
    }();

    const auto innerBounds = toOpenGLBounds(padded(getBoundsInMainWindow()));

    renderer.turnCullingOff();
    renderer.turnDepthTestOff();

    juce::gl::glDisable(juce::gl::GL_LINE_SMOOTH);
    gridLines.render(renderer.getResources(), innerBounds, axesRange, juce::Point<GLfloat>(2.0f / innerBounds.getWidth(), 2.0f / innerBounds.getHeight()));
    juce::gl::glEnable(juce::gl::GL_LINE_SMOOTH);

    // Render graph
    juce::gl::glViewport(innerBounds.getX(), innerBounds.getY(), innerBounds.getWidth(), innerBounds.getHeight());
    renderer.getResources().graphDataShader.use();

    const auto setUniforms = [&](const juce::Colour& colour, const Vec4& window, const Vec4& offsetAndScale)
    {
        renderer.getResources().graphDataShader.colour.set(colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha());
        renderer.getResources().graphDataShader.window.set(window.x, window.y, window.z, window.w);
        renderer.getResources().graphDataShader.offsetAndScale.set(offsetAndScale.x, offsetAndScale.y, offsetAndScale.z, offsetAndScale.w);
    };

    for (size_t index = 0; index < graphDataBuffer.getLineBuffers().size(); index++)
    {
        setUniforms(legend[index].colour,
                    { 0.0f, 0.0f, 1.0f, 1.0f },
                    { (GLfloat) -axesRange.getXCenter(),
                      (GLfloat) -axesRange.getYCenter(),
                      (GLfloat) axesRange.getXScale(),
                      (GLfloat) axesRange.getYScale() });

        renderer.getResources().graphDataBuffer.linkVbo(renderer.getResources().graphDataShader.position.attributeID, Buffer::vertexBuffer, Buffer::XY, Buffer::floatingPoint);
        renderer.getResources().graphDataBuffer.fillVbo(Buffer::vertexBuffer, &graphDataBuffer.getLineBuffers()[index][0].x, static_cast<GLsizeiptr>(graphDataBuffer.getNumberAvailable() * sizeof(juce::Point<GLfloat>)));
        renderer.getResources().graphDataBuffer.render(Buffer::lineStrip, (int) graphDataBuffer.getNumberAvailable());
    }

    const auto renderText = [&resources = renderer.getResources(), bounds](Text& text, const juce::String& label, const juce::Colour& colour, float x, float y, const juce::Justification justification, bool rotated = false)
    {
        juce::gl::glViewport(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
        resources.textShader.use();

        text.setText(label);
        text.setColour(colour);

        const juce::Point<GLfloat> pixelSize(2.0f / bounds.getWidth(), 2.0f / bounds.getHeight());
        text.setScale({ rotated ? pixelSize.y : pixelSize.x, rotated ? pixelSize.x : pixelSize.y });

        if (justification.testFlags(juce::Justification::horizontallyCentred))
        {
            (rotated ? y : x) -= text.getTotalWidth() / 2;
        }
        else if (justification.testFlags(juce::Justification::right))
        {
            (rotated ? y : x) -= text.getTotalWidth();
        }

        if (justification.testFlags(juce::Justification::verticallyCentred))
        {
            const auto offset = text.getFontSize() / 2.0f + text.getDescender();
            if (rotated)
            {
                x += offset;
            }
            else
            {
                y -= offset;
            }
        }

        auto translation = juce::Matrix3D<float>::fromTranslation(juce::Vector3D<float>(-1 + (x * pixelSize.x), -1 + (y * pixelSize.y), 0.0f));
        auto rotation = juce::Matrix3D<float>::rotation({ 0.0f, 0.0f, rotated ? juce::degreesToRadians(90.0f) : 0.0f });

        auto transformation = translation * rotation;

        resources.textShader.transformation.setMatrix4(transformation.mat, 1, false);

        text.render(resources);
    };

    // Render x axis label
    const auto timestamp = "Timestamp (" + juce::String(1E-6f * (float) graphDataBuffer.getMostRecentTimestamp(), 3) + ")";
    renderer.getResources().getGraphAxisLabelText().setText(timestamp.replaceCharacters("123456789", "000000000"));
    const auto secondsTextWidth = renderer.getResources().getGraphAxisLabelText().getTotalWidth();
    renderText(renderer.getResources().getGraphAxisLabelText(), timestamp, juce::Colours::white, (float) (bounds.getWidth() / 2) - secondsTextWidth / 2, (xAxisLabelHeight / 2.0f) * (float) context.getRenderingScale(), juce::Justification::centredLeft, false);

    // Render y axis label
    renderText(renderer.getResources().getGraphAxisLabelText(), yAxis, juce::Colours::white, (yAxisLabelWidth / 2.0f) * (float) context.getRenderingScale(), (float) (innerBounds.getCentreY() - bounds.getY()), juce::Justification::centred, true);

    // Render x axis values
    for (auto& seconds : gridLines.getMajorXPositions())
    {
        const auto x = juce::jmap<float>(seconds, (float) axesRange.xMin, (float) axesRange.xMax, (float) (innerBounds.getX() - bounds.getX()), (float) (innerBounds.getX() - bounds.getX() + innerBounds.getWidth()));
        const auto y = innerBounds.getY() - bounds.getY() - (int) renderer.getResources().getGraphAxisValuesText().getFontSize() - 5;
        renderText(renderer.getResources().getGraphAxisValuesText(), getAxisValueAsString(seconds, numberOfDecimalPlacesX), juce::Colours::grey, x, (float) y, juce::Justification::horizontallyCentred);
    }

    // Render y axis values
    for (auto& value : majorYPositions)
    {
        const auto x = innerBounds.getX() - bounds.getX() - 10;
        const auto y = juce::jmap<float>(value, (float) axesRange.yMin, (float) axesRange.yMax, (float) (innerBounds.getY() - bounds.getY()), (float) (innerBounds.getY() - bounds.getY() + innerBounds.getHeight()));
        renderText(renderer.getResources().getGraphAxisValuesText(), getAxisValueAsString(value, numberOfDecimalPlacesY), juce::Colours::grey, (float) x, y, juce::Justification::centredRight);
    }

    // Render legend
    auto x = innerBounds.getRight() - bounds.getX();
    for (auto it = legend.rbegin(); it != legend.rend(); it++)
    {
        auto topPadding = bounds.getBottom() - innerBounds.getBottom();
        auto y = innerBounds.getY() - bounds.getY() + innerBounds.getHeight() + topPadding / 2 - (int) renderer.getResources().getGraphLegendText().getFontSize() / 2;
        renderText(renderer.getResources().getGraphLegendText(), it->label, it->colour, (float) x, (float) y, juce::Justification::right);
        x -= (int) renderer.getResources().getGraphLegendText().getTotalWidth() + 15;
    }
}

void Graph::update(const uint64_t timestamp, const std::vector<float>& values)
{
    graphDataBuffer.write(timestamp, values);
}

void Graph::clear()
{
    graphDataBuffer.clear();
}

juce::Rectangle<int> Graph::padded(juce::Rectangle<int> rectangle)
{
    rectangle.removeFromLeft(yAxisLabelWidth + extraLeftPadding);
    rectangle.removeFromTop(25);
    rectangle.removeFromRight(10);
    rectangle.removeFromBottom(xAxisLabelHeight + xAxisValuesHeight);
    return rectangle;
}

int Graph::getNumberOfDecimalPlaces(const float rangeMin, const float rangeMax)
{
    return std::abs(std::min((int) std::ceil(std::log((rangeMax - rangeMin) * 0.01f) / std::log(10)), 0));
}
