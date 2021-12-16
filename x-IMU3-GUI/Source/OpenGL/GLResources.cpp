#include "GLResources.h"

GLResources::GLResources(juce::OpenGLContext& context_) : context(context_)
{
    auto unzip = [](const char* data, const int size)
    {
        juce::MemoryInputStream stream(data, (size_t) size, false);
        return std::unique_ptr<juce::InputStream>(juce::ZipFile(stream).createStreamForEntry(0))->readEntireStreamAsString();
    };

    arrow.setModel(BinaryData::Arrow_obj, "");
    board.setModel(unzip(BinaryData::Board_zip, BinaryData::Board_zipSize), "");
    housing.setModel(unzip(BinaryData::Housing_zip, BinaryData::Housing_zipSize), "");
    stage.setModel(BinaryData::Stage_obj, "");
    teapot.setModel(BinaryData::Teapot_obj, "");

    compassTexture.loadImage(juce::ImageFileFormat::loadFrom(BinaryData::Compass_png, BinaryData::Compass_pngSize));

    createGraphDataBuffer();
    createGridBuffers();
    createTextBuffer();
}

Text& GLResources::getGraphLegendText()
{
    const auto fontSize = (GLuint) juce::roundToInt(13 * context.getRenderingScale());
    if (legendText == nullptr || legendText->getFontSize() != fontSize)
    {
        legendText = std::make_unique<Text>(false);
        legendText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSize);
    }
    return *legendText;
}

Text& GLResources::getGraphAxisValuesText()
{
    const auto fontSize = (GLuint) juce::roundToInt(12 * context.getRenderingScale());
    if (axisValuesText == nullptr || axisValuesText->getFontSize() != fontSize)
    {
        axisValuesText = std::make_unique<Text>(false);
        axisValuesText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSize);
    }
    return *axisValuesText;
}

Text& GLResources::getGraphAxisLabelText()
{
    const auto fontSize = (GLuint) juce::roundToInt(13 * context.getRenderingScale());
    if (axisLabelText == nullptr || axisLabelText->getFontSize() != fontSize)
    {
        axisLabelText = std::make_unique<Text>(false);
        axisLabelText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSize);
    }
    return *axisLabelText;
}

Text& GLResources::get3DViewLoadingText()
{
    const auto fontSize = (GLuint) juce::roundToInt(24 * context.getRenderingScale());
    if (infoText == nullptr || infoText->getFontSize() != fontSize)
    {
        infoText = std::make_unique<Text>(false);
        infoText->loadFont(BinaryData::MontserratBold_ttf, BinaryData::MontserratBold_ttfSize, fontSize);
    }
    return *infoText;
}

Text& GLResources::get3DViewAxisText()
{
    const auto fontSize = (GLuint) juce::roundToInt(30 * context.getRenderingScale());
    if (axisMarkerText == nullptr || axisMarkerText->getFontSize() != fontSize)
    {
        axisMarkerText = std::make_unique<Text>(true);
        axisMarkerText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSize);
    }
    return *axisMarkerText;
}

void GLResources::createGraphDataBuffer()
{
    graphDataBuffer.create(graphBufferSize);
    graphDataBuffer.fillVbo(Buffer::vertexBuffer, nullptr, graphBufferSize * sizeof(juce::Point<GLfloat>), Buffer::multipleFill);
}

void GLResources::createGridBuffers()
{
    const int TOTAL_BYTES_VBO = 1024 * sizeof(Vec4);  //xyzw (w is for scalar color)
    const int TOTAL_BYTES_VBO_BORDER = 12 * borderThickness * sizeof(Vec4);  //xyzw (w is for scalar color)

    gridBorderBuffer.create(12 * borderThickness);
    gridVerticalBuffer.create(1024);
    gridHorizontalBuffer.create(1024);

    gridBorderBuffer.fillVbo(Buffer::vertexBuffer, nullptr, TOTAL_BYTES_VBO_BORDER, Buffer::multipleFill);
    gridVerticalBuffer.fillVbo(Buffer::vertexBuffer, nullptr, TOTAL_BYTES_VBO, Buffer::multipleFill);
    gridHorizontalBuffer.fillVbo(Buffer::vertexBuffer, nullptr, TOTAL_BYTES_VBO, Buffer::multipleFill);
}

void GLResources::createTextBuffer()
{
    GLfloat vertices[] = { -0.5f, -0.5f, 0.0f,
                           0.5f, -0.5f, 0.0f,
                           0.5f, 0.5f, 0.0f,
                           -0.5f, 0.5f, 0.0f };

    GLfloat colours[] = { 1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 1.0f };

    GLfloat UVs[] = { 0.0f, 0.0f,
                      1.0f, 0.0f,
                      1.0f, 1.0f,
                      0.0f, 1.0f };

    GLuint indices[] = { 0, 1, 3,
                         3, 1, 2 };

    textBuffer.create(6, true);
    textBuffer.fillEbo(indices, sizeof(indices), Buffer::multipleFill);
    textBuffer.fillVbo(Buffer::vertexBuffer, vertices, sizeof(vertices), Buffer::multipleFill);
    textBuffer.fillVbo(Buffer::colourBuffer, colours, sizeof(colours), Buffer::multipleFill);
    textBuffer.fillVbo(Buffer::textureBuffer, UVs, sizeof(UVs), Buffer::multipleFill);
}
