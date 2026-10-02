#include "OpenGLResources.h"

OpenGLResources::OpenGLResources(juce::OpenGLContext &context_, juce::ThreadPool &threadPool_) : context(context_),
                                                                                                 threadPool(threadPool_),
                                                                                                 arrow(context, threadPool) {
    arrow.setModel(BinaryData::Arrow_obj, "");

    compassTexture.loadImage(juce::ImageFileFormat::loadFrom(BinaryData::Compass_png, BinaryData::Compass_pngSize));

    const std::unordered_set<unsigned char> charactersToLoad = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '+', 'e', 'X', 'Y', 'Z', 't'};
    graphTickText = std::make_unique<Text>(charactersToLoad);
    threeDViewAxesText = std::make_unique<Text>(charactersToLoad);
}

Model &OpenGLResources::getModel(const juce::File &file) {
    std::lock_guard _(modelsLock);

    auto &model = models[file];

    if (model == nullptr) {
        model = std::make_unique<Model>(context, threadPool);
        model->setModel(file);
    }

    return *model;
}

Text &OpenGLResources::getGraphTickText() {
    // Handles font reload if window moved between low and high DPI monitors because GL pixel size will differ
    const auto fontSizeJucePixels = 12;
    if (graphTickText->getFontSizeGLPixels() != Text::toGLPixels(fontSizeJucePixels)) {
        graphTickText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSizeJucePixels);
    }
    return *graphTickText;
}

Text &OpenGLResources::get3DViewAxesText() {
    // Handles font reload if window moved between low and high DPI monitors because GL pixel size will differ
    const auto fontSizeJucePixels = 30;
    if (threeDViewAxesText->getFontSizeGLPixels() != Text::toGLPixels(fontSizeJucePixels)) {
        threeDViewAxesText->loadFont(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize, fontSizeJucePixels);
    }
    return *threeDViewAxesText;
}
