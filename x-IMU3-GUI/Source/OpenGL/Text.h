#pragma once

#include "Buffer.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include "Shader.h"
#include <string>

class GLResources;

class Text
{
public:
    explicit Text(const bool isFirstLetterCentered_);

    bool loadFont(const char* data, size_t dataSize, GLuint fontSize_);

    void unloadFont();

    GLuint getFontSize();

    GLuint getTotalWidth();

    float getDescender() const;

    const juce::String& getText() const;

    void setText(const juce::String& text_);

    void setScale(const juce::Point<GLfloat>& scale_);

    void setPosition(const juce::Vector3D<GLfloat>& position_);

    void setColour(const juce::Colour& colour_);

    void render(GLResources& resources);

private:

    class Initializer
    {
    public:
        Initializer();

        ~Initializer();

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Initializer)
    };

    juce::SharedResourcePointer<Initializer> initializer;

    struct Glyph
    {
        GLuint textureID; //texture freetypeTextureID for each letter
        GLuint width; //width of the letter
        GLuint height; //height of the letter
        GLint bearingX; //distance from the y-axis origin
        GLint bearingY; //distance from the x-axis baseline
        GLint advance; //offset to advance to next glyph
    };

    juce::Colour colour = juce::Colours::white;
    juce::Vector3D<GLfloat> position;

    juce::String text;

    juce::Point<GLfloat> scale = juce::Point<GLfloat>(1.0f, 1.0f);

    GLuint fontSize = 0;
    GLuint totalWidth = 0;

    GLfloat descender = 0;

    bool isFirstLetterCentered = false;

    std::map<GLchar, Glyph> alphabet;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Text)
};
