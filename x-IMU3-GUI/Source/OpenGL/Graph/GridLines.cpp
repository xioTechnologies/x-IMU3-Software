#include "GridLines.h"

void GridLines::render(GLResources& resources, const juce::Rectangle<GLint>& viewport_, const AxesRange& axesRange_, juce::Point<GLfloat> pixelSize_)
{
    viewport = viewport_;
    axesRange = axesRange_;
    pixelSize = pixelSize_;

    juce::gl::glPointSize(2.0f);
    juce::gl::glViewport(viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight());
    resources.gridLinesShader.use();

    const auto setUniforms = [&shader = resources.gridLinesShader](const Vec4& colour, const Vec4& window, const Vec4& offsetAndScale)
    {
        shader.colour.set(colour.x, colour.y, colour.z, colour.w);
        shader.window.set(window.x, window.y, window.z, window.w);
        shader.offsetAndScale.set(offsetAndScale.x, offsetAndScale.y, offsetAndScale.z, offsetAndScale.w);
    };

    const auto renderVertices = [&shader = resources.gridLinesShader](auto& buffer, auto& lines, const auto isVertical, const auto isBorder)
    {
        if (lines.size() > 0)
        {
            buffer.linkVbo(shader.position.attributeID, Buffer::vertexBuffer, Buffer::Rgba, Buffer::floatingPoint);
            buffer.fillVbo(Buffer::vertexBuffer, &lines[0].x, GLsizeiptr(lines.size() * sizeof(Vec4)), Buffer::FillType::multipleFill);
            shader.isVertical.set(isVertical);
            shader.isBorder.set(isBorder);
            buffer.render(Buffer::lines, (int) lines.size());
        }
    };

    setUniforms({ 1.0f, 1.0f, 1.0f, 1.0f },
                { 0.0f, 0.0f, 1.0f, 1.0f },
                { (GLfloat) -axesRange.getXCenter(),
                  (GLfloat) -axesRange.getYCenter(),
                  (GLfloat) axesRange.getXScale(),
                  (GLfloat) axesRange.getYScale() });

    createVerticalLines();
    renderVertices(resources.gridVerticalBuffer, verticesVerticalLines, true, false);

    createHorizontalLines();
    renderVertices(resources.gridHorizontalBuffer, verticesHorizontalLines, false, false);

    createBorderLines();
    setUniforms({ 1.0f, 1.0f, 1.0f, 1.0f },
                { -1.0f, -1.0f, 1.0f, 1.0f },
                { 0.0f, 0.0f, 1.0f, 1.0f });
    renderVertices(resources.gridBorderBuffer, verticesBorderLines, false, true);
}

std::vector<float> GridLines::getMajorXPositions() const
{
    std::vector<float> positions;
    for (auto& line : verticesVerticalLines)
    {
        if ((line.w == majorTickColor) || (line.w == subMajorTickColor))
        {
            positions.push_back(line.x);
        }
    }
    return positions;
}

std::vector<float> GridLines::getMajorYPositions() const
{
    std::vector<float> positions;
    for (auto& line : verticesHorizontalLines)
    {
        if ((line.w == majorTickColor) || (line.w == subMajorTickColor))
        {
            positions.push_back(line.y);
        }
    }
    return positions;
}

void GridLines::getMajorMinorTicks(double& tickSize, int& majorEveryInt, int& labelEveryInt, double pixelsPerUnit, float pixelThreshold)
{
    if (pixelsPerUnit * tickSize < pixelThreshold)
    {
        tickSize *= 10;

        if (pixelsPerUnit * tickSize < pixelThreshold * 2)
        {
            majorEveryInt = 10;
            labelEveryInt = 10;
        }
        else if (pixelsPerUnit * tickSize < pixelThreshold * 4)
        {
            majorEveryInt = 5;
            labelEveryInt = 5;
        }
        else if (pixelsPerUnit * tickSize < pixelThreshold * 8)
        {
            majorEveryInt = 2;
            labelEveryInt = 2;
        }
        else
        {
            majorEveryInt = 1;
            labelEveryInt = 1;
        }
    }
    else if (pixelsPerUnit * tickSize < pixelThreshold * 2)
    {
        tickSize *= 5;
        majorEveryInt = 2;
        labelEveryInt = 2;
    }
    else if (pixelsPerUnit * tickSize < pixelThreshold * 4)
    {
        tickSize *= 2;
        majorEveryInt = 5;
        labelEveryInt = 5;
    }
    else if (pixelsPerUnit * tickSize > pixelThreshold * 20)
    {
        labelEveryInt = 1;
    }
    else if (pixelsPerUnit * tickSize > pixelThreshold * 10)
    {
        labelEveryInt = 2;
    }
    else if (pixelsPerUnit * tickSize > pixelThreshold * 4)
    {
        labelEveryInt = 5;
    }
}

void GridLines::createBorderLines()
{
    verticesBorderLines.clear();

    const float pixelsIn = 1.0f;
    for (int i = 0; i < GLResources::borderThickness; i++)
    {
        juce::Point<GLfloat> min(pixelsIn + i, pixelsIn + i);
        juce::Point<GLfloat> max(viewport.getWidth() - pixelsIn - i, viewport.getHeight() - pixelsIn - i);

        min *= pixelSize;
        max *= pixelSize;

        float lineColor = 0.75f;

        if (min.x < max.x && min.y < max.y)
        {
            verticesBorderLines.push_back({ min.x, min.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ max.x, min.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ min.x, min.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ min.x, max.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ max.x, max.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ min.x, max.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ max.x, max.y, 0.5f, lineColor });
            verticesBorderLines.push_back({ max.x, min.y, 0.5f, lineColor });
        }
    }
}

int GridLines::createHorizontalLines()
{
    int i = 0;

    double y;
    double tickSize;
    axesRange.getMostSignificantTickY(y, tickSize);

    int majorEveryInt = 10;
    int majorLineIndex = 0;

    int labelEveryInt = 10;
    int labelLineIndex = 0;

    double pixelsPerUnit = getPixelsPerUnit().y;

    getMajorMinorTicks(tickSize, majorEveryInt, labelEveryInt, pixelsPerUnit, 3);

    double yMax = axesRange.yMax;
    double yMin = axesRange.yMin;
    double xMin = axesRange.xMin;
    double xMax = axesRange.xMax;

    verticesHorizontalLines.clear();

    while (y <= yMax)
    {
        if (y >= yMin)
        {
            float colour = subTickColor;

            if (majorLineIndex == 0)
            {
                colour = majorTickColor;
            }
            else if (labelLineIndex == 0)
            {
                colour = subMajorTickColor;
            }

            verticesHorizontalLines.push_back({ (float) xMin, (float) y, 0.5f, colour });
            verticesHorizontalLines.push_back({ (float) xMax, (float) y, 0.5f, colour });

            i += 2;
        }

        majorLineIndex = (majorLineIndex + 1) % majorEveryInt;
        labelLineIndex = (labelLineIndex + 1) % labelEveryInt;

        y += tickSize;
    }

    return i;
}

int GridLines::createVerticalLines()
{
    int i = 0;

    double x;
    double tickSize;
    axesRange.getMostSignificantTickX(x, tickSize);

    int majorEveryInt = 10;
    int majorLineIndex = 0;

    int labelEveryInt = 10;
    int labelLineIndex = 0;

    double pixelsPerUnit = getPixelsPerUnit().x;

    getMajorMinorTicks(tickSize, majorEveryInt, labelEveryInt, pixelsPerUnit, 5);

    double yMax = axesRange.yMax;
    double yMin = axesRange.yMin;
    double xMin = axesRange.xMin;
    double xMax = axesRange.xMax;

    verticesVerticalLines.clear();

    while (x <= xMax)
    {
        if (x >= xMin)
        {
            float colour = subTickColor;

            if (majorLineIndex == 0)
            {
                colour = majorTickColor;
            }

            else if (labelLineIndex == 0)
            {
                colour = subMajorTickColor;
            }

            verticesVerticalLines.push_back({ (float) x, (float) yMin, 0.5f, colour });
            verticesVerticalLines.push_back({ (float) x, (float) yMax, 0.5f, colour });

            i += 2;
        }

        majorLineIndex = (majorLineIndex + 1) % majorEveryInt;
        labelLineIndex = (labelLineIndex + 1) % labelEveryInt;

        x += tickSize;
    }

    return i;
}

juce::Point<float> GridLines::getPixelsPerUnit()
{
    int graphSizeWidth = viewport.getWidth();
    int graphSizeHeight = viewport.getHeight();

    double pixelsPerUnitX = graphSizeWidth / axesRange.getXRange();
    double pixelsPerUnitY = graphSizeHeight / axesRange.getYRange();

    return { (float) pixelsPerUnitX, (float) pixelsPerUnitY };
}
