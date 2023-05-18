#include "GLResources.h"

GLResources::GLResources(juce::OpenGLContext& context_) : context(context_)
{
    auto unzipObjAndMtl = [](const char* data, const int size, const juce::String& internalFileName)
    {
        juce::MemoryInputStream stream(data, (size_t) size, false);
        auto zipFile = juce::ZipFile(stream);

        auto* objFile = zipFile.getEntry(internalFileName + ".obj");
        auto* mtlFile = zipFile.getEntry(internalFileName + ".mtl");

        juce::String objectString = objFile ? std::unique_ptr<juce::InputStream>(zipFile.createStreamForEntry(*objFile))->readEntireStreamAsString() : "";
        juce::String materialString = mtlFile ? std::unique_ptr<juce::InputStream>(zipFile.createStreamForEntry(*mtlFile))->readEntireStreamAsString() : "";

        return std::tuple<juce::String, juce::String> { objectString, materialString };
    };

    const auto& [boardObj, boardMtl] = unzipObjAndMtl(BinaryData::xIMU3_Board_zip, BinaryData::xIMU3_Board_zipSize, "x-IMU3 Board");
    board.setModel(boardObj, boardMtl);

    const auto& [housingObj, housingMtl] = unzipObjAndMtl(BinaryData::xIMU3_Housing_zip, BinaryData::xIMU3_Housing_zipSize, "x-IMU3 Housing");
    housing.setModel(housingObj, housingMtl);

    arrow.setModel(BinaryData::Arrow_obj, "");

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

    GLfloat UVs[] = { 0.0f, 0.0f,
                      1.0f, 0.0f,
                      1.0f, 1.0f,
                      0.0f, 1.0f };

    GLuint indices[] = { 0, 1, 3,
                         3, 1, 2 };

    textBuffer.create(6, true);
    textBuffer.fillEbo(indices, sizeof(indices), Buffer::multipleFill);
    textBuffer.fillVbo(Buffer::vertexBuffer, vertices, sizeof(vertices), Buffer::multipleFill);
    textBuffer.fillVbo(Buffer::textureBuffer, UVs, sizeof(UVs), Buffer::multipleFill);
}
