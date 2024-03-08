#include "GLResources.h"

GLResources::GLResources(juce::OpenGLContext& context_, juce::ThreadPool& threadPool) :
        context(context_),
        arrow(context, threadPool),
        board(context, threadPool),
        housing(context, threadPool),
        user(context, threadPool)
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

    const std::unordered_set<unsigned char> charactersToLoad = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '+', 'e', 'X', 'Y', 'Z' };
    graphTickText = std::make_unique<Text>(charactersToLoad);
    threeDViewAxesText = std::make_unique<Text>(charactersToLoad);
}

Text& GLResources::getGraphTickText()
{
    // Handles font reload if window moved between low and high DPI monitors because GL pixel size will differ
    const auto fontSizeJucePixels = 12;
    if (graphTickText->getFontSizeGLPixels() != Text::toGLPixels(fontSizeJucePixels))
    {
        graphTickText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSizeJucePixels);
    }
    return *graphTickText;
}

Text& GLResources::get3DViewAxesText()
{
    // Handles font reload if window moved between low and high DPI monitors because GL pixel size will differ
    const auto fontSizeJucePixels = 30;
    if (threeDViewAxesText->getFontSizeGLPixels() != Text::toGLPixels(fontSizeJucePixels))
    {
        threeDViewAxesText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSizeJucePixels);
    }
    return *threeDViewAxesText;
}
