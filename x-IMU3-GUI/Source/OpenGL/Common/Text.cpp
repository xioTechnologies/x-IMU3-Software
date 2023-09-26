#include "OpenGL/Common/GLResources.h"
#include "Text.h"

Text::Text(const std::unordered_set<unsigned char>& charactersToLoad_) : charactersToLoad(charactersToLoad_)
{
}

Text::~Text()
{
    unloadFont();
}

bool Text::loadFont(const char* data, size_t dataSize, int fontSizeJucePixels_)
{
    using namespace ::juce::gl;

    unloadFont();

    fontSizeJucePixels = fontSizeJucePixels_;
    fontSizeGLPixels = (GLuint) toGLPixels(fontSizeJucePixels);

    FT_Face face = nullptr;
    if (FT_New_Memory_Face(freeTypeLibrary->get(), reinterpret_cast<const FT_Byte*>(data), (FT_Long) dataSize, 0, &face))
    {
        return false;
    }

    FT_Set_Pixel_Sizes(face, fontSizeGLPixels, fontSizeGLPixels);

    descender = toPixels((float) face->size->metrics.descender);

    // Create OpenGL Textures for every font character that will be used
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable OpenGL default 4-byte alignment restriction since we store grayscale data with 1 byte per pixel
    glActiveTexture(GL_TEXTURE0); // use first texture unit for all textures bound below because shader only uses 1 texture at a time
    for (const auto& character : charactersToLoad)
    {
        if (FT_Load_Char(face, (FT_ULong) character, FT_LOAD_RENDER)) // if freetype fails to load the current glyph index
        {
            continue;
        }

        // Generate texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     (GLsizei) face->glyph->bitmap.width,
                     (GLsizei) face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        // Texture wrapping preferences
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Magnification/minification filters - GL_NEAREST for sharper text
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        const Glyph glyph = { textureID,
                              glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                              glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                              toPixels((float) face->glyph->advance.x) };
        glyphs[character] = glyph;
    }

    FT_Done_Face(face);
    return true;
}

void Text::unloadFont()
{
    for (auto& glyph : glyphs)
    {
        juce::gl::glDeleteTextures(1, &(glyph.second.textureID));
    }

    glyphs.clear();
}

int Text::getFontSizeGLPixels() const
{
    return (int) fontSizeGLPixels;
}

int Text::getFontSizeJucePixels() const
{
    return fontSizeJucePixels;
}

float Text::getStringWidthGLPixels(const juce::String& string) const
{
    float width = 0.0f;

    for (const auto& character : string)
    {
        auto glyphSearch = glyphs.find(static_cast<unsigned char>(character));
        if (glyphSearch == glyphs.end())
        {
            continue;
        }
        const Glyph glyph = glyphSearch->second;
        width += glyph.advance;
    }

    return width;
}

float Text::getStringWidthJucePixels(const juce::String& string) const
{
    auto context = juce::OpenGLContext::getCurrentContext();
    return (float) ((double) getStringWidthGLPixels(string) / (context ? context->getRenderingScale() : 1.0));
}

void Text::draw(GLResources* const resources, const juce::String& text, const juce::Colour& colour, juce::Justification justification, glm::vec2 screenPosition, juce::Rectangle<int> viewport) const
{
    if (justification.testFlags(juce::Justification::horizontallyCentred))
    {
        screenPosition.x -= getStringWidthGLPixels(text) / 2.0f;
    }
    else if (justification.testFlags(juce::Justification::right))
    {
        screenPosition.x -= getStringWidthGLPixels(text);
    }

    if (justification.testFlags(juce::Justification::verticallyCentred))
    {
        const auto offset = (GLfloat) getFontSizeGLPixels() / 2.0f + descender;
        screenPosition.y -= offset;
    }

    const auto projection = glm::ortho((float) viewport.getX(), (float) viewport.getRight(), (float) viewport.getY(), (float) viewport.getY() + (float) viewport.getHeight());

    const auto& textShader = resources->textShader;
    textShader.use();
    textShader.colour.setRGBA(colour);
    auto textOrigin = screenPosition;
    for (size_t index = 0; index < (size_t) text.length(); index++)
    {
        auto glyphSearch = glyphs.find(static_cast<unsigned char>(text[(int) index]));
        if (glyphSearch == glyphs.end())
        {
            continue;
        }
        const Glyph glyph = glyphSearch->second;

        const auto scale = glm::scale(glm::mat4(1.0), glm::vec3(glyph.size, 1.0f));
        const auto glyphCentre = glm::vec2((float) glyph.bearing.x + (0.5f * (float) glyph.size.x), (0.5f * (float) glyph.size.y) - ((float) glyph.size.y - (float) glyph.bearing.y));
        const auto translation = glm::translate(glm::mat4(1.0), glm::vec3(textOrigin + glyphCentre, 0.0f));
        const auto transform = projection * translation * scale;
        textShader.transform.set(transform);
        textShader.setTextureImage(juce::gl::GL_TEXTURE_2D, glyph.textureID);
        resources->textQuad.draw();

        textOrigin.x += glyph.advance; // move origin to next character
    }
}

void Text::drawChar3D(GLResources* const resources, unsigned char character, const juce::Colour& colour, const glm::mat4& transform, juce::Rectangle<int> viewportBounds) const
{
    auto glyphSearch = glyphs.find(character);
    if (glyphSearch == glyphs.end())
    {
        return;
    }
    const Glyph glyph = glyphSearch->second;

    // Calculate Normalized Device Coordinates (NDC) transformation to place text at position on screen with a constant size
    const auto ndcCoord = glm::vec2(transform[3][0], transform[3][1]) / transform[3][3]; // get x, y of matrix translation then divide by w of translation for constant size in pixels
    const auto zTranslation = transform[3][2]; // use z of matrix translation so 2D elements have proper layering
    const auto ndcMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(ndcCoord, zTranslation));

    const auto& textShader = resources->textShader;
    textShader.use();
    textShader.colour.setRGBA(colour);

    const auto scaleFactor = glm::vec2((float) glyph.size.x / (float) viewportBounds.getWidth(), (float) glyph.size.y / (float) viewportBounds.getHeight());
    const auto scale = glm::scale(glm::mat4(1.0), glm::vec3(scaleFactor, 1.0f));
    const auto ndcTransform = ndcMatrix * scale;
    textShader.transform.set(ndcTransform);
    textShader.setTextureImage(juce::gl::GL_TEXTURE_2D, glyph.textureID);
    resources->textQuad.draw();
}

int Text::toGLPixels(int jucePixels)
{
    auto context = juce::OpenGLContext::getCurrentContext();
    return juce::roundToInt((double) jucePixels * (context ? context->getRenderingScale() : 1.0));
}

Text::FreeTypeLibrary::FreeTypeLibrary()
{
    FT_Init_FreeType(&freetypeLibrary);
}

Text::FreeTypeLibrary::~FreeTypeLibrary()
{
    FT_Done_FreeType(freetypeLibrary);
}

FT_Library Text::FreeTypeLibrary::get() const
{
    return freetypeLibrary;
}
