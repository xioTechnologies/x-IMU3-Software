#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include <juce_opengl/juce_opengl.h>

namespace OpenGLHelpers {
    /** Represents an OpenGL uniform value.
        After a juce::OpenGLShaderProgram has been linked, you can create Uniform objects to
        let you set the uniforms that your shaders use.

        Be careful not to call the set() functions unless the appropriate program
        is loaded into the current context.

        Based on JUCE's juce::OpenGLShaderProgram::Uniform with extended set functions for
        validity checking, colours, and GLM structures.
    */
    class Uniform {
    public:
        /** Initialises a uniform.
            The program must have been successfully linked when this
            constructor is called.
        */
        Uniform(const juce::OpenGLShaderProgram &program, const char *uniformName_) : uniformId(juce::gl::glGetUniformLocation(program.getProgramID(), uniformName_))
#if JUCE_DEBUG
                                                                                      , uniformName(uniformName_)
#endif
        {
            assertUniformExistsInShaderProgram();
        }

        bool isValid() const noexcept {
            return uniformId >= 0;
        }

        explicit operator bool() const noexcept {
            return isValid();
        }

        GLint getUniformId() const {
            return uniformId;
        }

        /** Sets a float uniform. */
        void set(GLfloat n1) const noexcept {
            juce::gl::glUniform1f(uniformId, n1);
        }

        /** Sets an int uniform. */
        void set(GLint n1) const noexcept {
            juce::gl::glUniform1i(uniformId, n1);
        }

        /** Sets a boolean into an int uniform. */
        void set(bool data) const noexcept {
            set((GLint) data);
        }

        /** Sets a vec2 uniform. */
        void set(GLfloat n1, GLfloat n2) const noexcept {
            juce::gl::glUniform2f(uniformId, n1, n2);
        }

        /** Sets a vec3 uniform. */
        void set(GLfloat n1, GLfloat n2, GLfloat n3) const noexcept {
            juce::gl::glUniform3f(uniformId, n1, n2, n3);
        }

        /** Sets a vec4 uniform. */
        void set(GLfloat n1, GLfloat n2, GLfloat n3, GLfloat n4) const noexcept {
            juce::gl::glUniform4f(uniformId, n1, n2, n3, n4);
        }

        /** Sets an ivec4 uniform. */
        void set(GLint n1, GLint n2, GLint n3, GLint n4) const noexcept {
            juce::gl::glUniform4i(uniformId, n1, n2, n3, n4);
        }

        /** Sets a vector float uniform. */
        void set(const GLfloat *values, int numValues) const noexcept {
            juce::gl::glUniform1fv(uniformId, numValues, values);
        }

        /** Sets a 2x2 matrix float uniform. */
        void setMatrix2(const GLfloat *values, GLint count, GLboolean transpose) const noexcept {
            juce::gl::glUniformMatrix2fv(uniformId, count, transpose, values);
        }

        /** Sets a 3x3 matrix float uniform. */
        void setMatrix3(const GLfloat *values, GLint count, GLboolean transpose) const noexcept {
            juce::gl::glUniformMatrix3fv(uniformId, count, transpose, values);
        }

        /** Sets a 4x4 matrix float uniform. */
        void setMatrix4(const GLfloat *values, GLint count, GLboolean transpose) const noexcept {
            juce::gl::glUniformMatrix4fv(uniformId, count, transpose, values);
        }

        /** Sets the RGB portion of a JUCE colour to a vec3 uniform. */
        void setRGB(juce::Colour colour) const noexcept {
            set((GLfloat) colour.getFloatRed(), (GLfloat) colour.getFloatGreen(), (GLfloat) colour.getFloatBlue());
        }

        /** Sets the RGBA of a JUCE colour to a vec4 uniform. */
        void setRGBA(juce::Colour colour) const noexcept {
            set((GLfloat) colour.getFloatRed(), (GLfloat) colour.getFloatGreen(), (GLfloat) colour.getFloatBlue(), (GLfloat) colour.getFloatAlpha());
        }

        /** Sets a vec2 uniform. */
        void set(glm::vec2 data) const noexcept {
            set(data.x, data.y);
        }

        /** Sets a vec3 uniform. */
        void set(glm::vec3 data) const noexcept {
            juce::gl::glUniform3fv(uniformId, 1, glm::value_ptr(data));
        }

        /** Sets a vec4 uniform. */
        void set(glm::vec4 data) const noexcept {
            juce::gl::glUniform4fv(uniformId, 1, glm::value_ptr(data));
        }

        /** Sets a mat3 uniform. */
        void set(glm::mat3 matrix) const noexcept {
            juce::gl::glUniformMatrix3fv(uniformId, 1, juce::gl::GL_FALSE, glm::value_ptr(matrix));
        }

        /** Sets a mat4 uniform. */
        void set(glm::mat4 matrix) const noexcept {
            juce::gl::glUniformMatrix4fv(uniformId, 1, juce::gl::GL_FALSE, glm::value_ptr(matrix));
        }

    private:
        void assertUniformExistsInShaderProgram() const {
            if (!isValid()) {
#if JUCE_DEBUG
                std::cout << "OpenGL Error: Uniform name \"" << uniformName << "\" does not exist in the compiled shader program." << std::endl;
#endif

                /*  If you hit this assertion, then you have attempted to create a GLUtil::Uniform
                    with a name that is not found in the OpenGLShaderProgram it is linked to. Make sure your
                    GLUtil::Uniform's name matches the uniform names in your GLSL shader programs.
                    Additionally, make sure any of your changes to shader files have been updated in the
                    binary data by saving Projucer or refreshing CMake.
                */
                jassertfalse;
            }
        }

        /** The uniform's ID number.
            If the uniform couldn't be found, this value will be < 0.
        */
        GLint uniformId;

#if JUCE_DEBUG
        juce::String uniformName;
#endif

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Uniform)
    };

    static void setCapabilityEnabled(GLenum capability, GLboolean shouldBeEnabled) {
        if (shouldBeEnabled) {
            juce::gl::glEnable(capability);
        } else {
            juce::gl::glDisable(capability);
        }
    }

    class ScopedCapability {
    public:
        ScopedCapability(GLenum capability_, GLboolean shouldBeEnabled) {
            capability = capability_;
            wasEnabled = juce::gl::glIsEnabled(capability);
            setCapabilityEnabled(capability, shouldBeEnabled);
        }

        ~ScopedCapability() {
            setCapabilityEnabled(capability, wasEnabled);
        }

        GLenum capability;
        GLboolean wasEnabled;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopedCapability)
    };

    static inline void viewport(const juce::Rectangle<int> &bounds) {
        juce::gl::glViewport((GLint) bounds.getX(), (GLint) bounds.getY(), (GLsizei) bounds.getWidth(), (GLsizei) bounds.getHeight());
    }

    static inline void scissor(const juce::Rectangle<int> &bounds) {
        juce::gl::glScissor((GLint) bounds.getX(), (GLint) bounds.getY(), (GLsizei) bounds.getWidth(), (GLsizei) bounds.getHeight());
    }

    static inline void viewportAndScissor(const juce::Rectangle<int> &bounds) {
        OpenGLHelpers::viewport(bounds);
        OpenGLHelpers::scissor(bounds);
    }

    static inline void clear(const juce::Colour &colour, const juce::Rectangle<int> &bounds) {
        OpenGLHelpers::ScopedCapability _(juce::gl::GL_SCISSOR_TEST, true);
        OpenGLHelpers::viewportAndScissor(bounds);
        juce::OpenGLHelpers::clear(colour);
    }
} // GLUtil
