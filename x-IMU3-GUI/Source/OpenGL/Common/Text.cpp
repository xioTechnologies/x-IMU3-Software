#include "freetype/freetype.h"
#include "OpenGL/Common/GLResources.h"
#include "Text.h"

static FT_Library freetypeLibrary;

Text::Initializer::Initializer()
{
    FT_Init_FreeType(&freetypeLibrary);
}

Text::Initializer::~Initializer()
{
    FT_Done_FreeType(freetypeLibrary);
}

Text::Text(const bool isFirstLetterCentered_) : isFirstLetterCentered(isFirstLetterCentered_)
{
}

bool Text::loadFont(const char* data, size_t dataSize, GLuint fontSize_)
{
    FT_Face freetypeFace = nullptr;

    if (FT_New_Memory_Face(freetypeLibrary, reinterpret_cast<const FT_Byte*>(data), (FT_Long) dataSize, 0, &freetypeFace))
    {
        return false;
    }

    FT_Set_Pixel_Sizes(freetypeFace, (FT_UInt) fontSize_, (FT_UInt) fontSize_);

    juce::gl::glPixelStorei(juce::gl::GL_UNPACK_ALIGNMENT, 1);

    for (int i = 0; i < 256; i++)
    {
        if (FT_Load_Char(freetypeFace, (FT_ULong) i, FT_LOAD_RENDER))
        {
            continue;
        }

        GLuint freetypeTextureID;
        juce::gl::glGenTextures(1, &freetypeTextureID);
        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, freetypeTextureID);

        juce::gl::glTexParameterf(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MAG_FILTER, juce::gl::GL_LINEAR);
        juce::gl::glTexParameterf(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MIN_FILTER, juce::gl::GL_LINEAR);

        juce::gl::glTexParameterf(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_WRAP_S, juce::gl::GL_CLAMP_TO_EDGE);
        juce::gl::glTexParameterf(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_WRAP_T, juce::gl::GL_CLAMP_TO_EDGE);

        juce::gl::glTexImage2D(juce::gl::GL_TEXTURE_2D,
                               0,
                               juce::gl::GL_RED,
                               (GLsizei) freetypeFace->glyph->bitmap.width,
                               (GLsizei) freetypeFace->glyph->bitmap.rows,
                               0,
                               juce::gl::GL_RED,
                               juce::gl::GL_UNSIGNED_BYTE,
                               freetypeFace->glyph->bitmap.buffer);

        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, 0);

        Glyph glyph = { freetypeTextureID,
                        freetypeFace->glyph->bitmap.width,
                        freetypeFace->glyph->bitmap.rows,
                        freetypeFace->glyph->bitmap_left,
                        freetypeFace->glyph->bitmap_top,
                        (GLint) freetypeFace->glyph->advance.x };

        alphabet[(GLchar) i] = glyph;
    }

    fontSize = fontSize_;
    descender = freetypeFace->size->metrics.descender / 64.0f;

    FT_Done_Face(freetypeFace);
    return true;
}

void Text::unloadFont()
{
    for (auto& glyph : alphabet)
    {
        juce::gl::glDeleteTextures(1, &(glyph.second.textureID));
    }

    alphabet.clear();
}

GLuint Text::getFontSize()
{
    return fontSize;
}

GLuint Text::getTotalWidth()
{
    totalWidth = 0;

    for (int i = 0; i < text.length(); i++)
    {
        Glyph glyph = alphabet[(GLchar) text[i]];
        totalWidth += (GLuint) (glyph.advance / 64.0f);
    }

    return totalWidth;
}

float Text::getDescender() const
{
    return descender;
}

const juce::String& Text::getText() const
{
    return text;
}

void Text::setText(const juce::String& text_)
{
    text = text_;
}

void Text::setScale(const juce::Point<GLfloat>& scale_)
{
    scale = scale_;
}

void Text::setPosition(const juce::Vector3D<GLfloat>& position_)
{
    position = position_;
}

void Text::renderScreenSpace(GLResources& resources, const juce::String& label, const juce::Colour& colour, const glm::mat4& transform)
{
    // Calculate Normalized Device Coordinates (NDC) transformation to place text at position on screen with a constant size
    glm::vec2 ndcCoord = glm::vec2(transform[3][0], transform[3][1]) / transform[3][3]; // get x, y of translation portion then divide by w of translation
    const auto ndcMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(ndcCoord, 0.0f));

    resources.textShader.colour.setRGBA(colour);
    resources.textShader.transformation.set(ndcMatrix);

    setText(label);
    render(resources);
}

void Text::render(GLResources& resources)
{
    auto textOrigin = position;

    for (size_t index = 0; index < (size_t) text.length(); index++)
    {
        Glyph glyph = alphabet[(GLchar) text[(int) index]];

        auto halfWidth = glyph.width * 0.5f;
        auto halfBearingY = glyph.bearingY * 0.5f;

        if (isFirstLetterCentered)
        {
            GLfloat vertices[] = { textOrigin.x - (halfWidth * scale.x), textOrigin.y + (halfBearingY * scale.y), 0.0f,
                                   textOrigin.x + (halfWidth * scale.x), textOrigin.y + (halfBearingY * scale.y), 0.0f,
                                   textOrigin.x + (halfWidth * scale.x), textOrigin.y + (halfBearingY * scale.y) - (glyph.height * scale.y), 0.0f,
                                   textOrigin.x - (halfWidth * scale.x), textOrigin.y + (halfBearingY * scale.y) - (glyph.height * scale.y), 0.0f };

            resources.textBuffer.fillVbo(Buffer::vertexBuffer, vertices, sizeof(vertices), Buffer::multipleFill);
        }

        else
        {
            GLfloat vertices[] = { textOrigin.x + (glyph.bearingX * scale.x), textOrigin.y + (glyph.bearingY * scale.y), 0.0f,
                                   textOrigin.x + (glyph.bearingX * scale.x) + (glyph.width * scale.x), textOrigin.y + (glyph.bearingY * scale.y), 0.0f,
                                   textOrigin.x + (glyph.bearingX * scale.x) + (glyph.width * scale.x), textOrigin.y + (glyph.bearingY * scale.y) - (glyph.height * scale.y), 0.0f,
                                   textOrigin.x + (glyph.bearingX * scale.x), textOrigin.y + (glyph.bearingY * scale.y) - (glyph.height * scale.y), 0.0f };

            resources.textBuffer.fillVbo(Buffer::vertexBuffer, vertices, sizeof(vertices), Buffer::multipleFill);
        }

        GLfloat UVs[] = { 0.0f, 0.0f,
                          1.0f, 0.0f,
                          1.0f, 1.0f,
                          0.0f, 1.0f };

        GLuint indices[] = { 0, 1, 3,
                             3, 1, 2 };

        resources.textBuffer.linkEbo();
        resources.textBuffer.linkVbo(resources.textShader.vertexIn.attributeID, Buffer::vertexBuffer, Buffer::Xyz, Buffer::floatingPoint);
        resources.textBuffer.linkVbo(resources.textShader.textureIn.attributeID, Buffer::textureBuffer, Buffer::UV, Buffer::floatingPoint);

        resources.textBuffer.fillEbo(indices, sizeof(indices), Buffer::multipleFill);
        resources.textBuffer.fillVbo(Buffer::textureBuffer, UVs, sizeof(UVs), Buffer::multipleFill);

        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, glyph.textureID);

        resources.textBuffer.render(Buffer::triangles);

        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, 0);

        textOrigin.x += (glyph.advance * scale.x) / 64.0f;
    }
}
