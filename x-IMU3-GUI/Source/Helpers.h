#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>

namespace Helpers
{
    inline juce::Quaternion<float> toQuaternion(const float xx, const float xy, const float xz,
                                                const float yx, const float yy, const float yz,
                                                const float zx, const float zy, const float zz)
    {
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
        if (const auto tr = xx + yy + zz; tr > 0)
        {
            const auto s = std::sqrt(tr + 1.0f) * 2.0f; // s=4*w
            return juce::Quaternion<float>((zy - yz) / s,
                                           (xz - zx) / s,
                                           (yx - xy) / s,
                                           0.25f * s);
        }
        else if ((xx > yy) & (xx > zz))
        {
            const auto s = std::sqrt(1.0f + xx - yy - zz) * 2.0f; // s=4*x
            return juce::Quaternion<float>(0.25f * s,
                                           (xy + yx) / s,
                                           (xz + zx) / s,
                                           (zy - yz) / s);
        }
        else if (yy > zz)
        {
            const auto s = std::sqrt(1.0f + yy - xx - zz) * 2.0f; // s=4*y
            return juce::Quaternion<float>((xy + yx) / s,
                                           0.25f * s,
                                           (yz + zy) / s,
                                           (xz - zx) / s);
        }
        else
        {
            const auto s = std::sqrt(1.0f + zz - xx - yy) * 2.0f; // s=4*z
            return juce::Quaternion<float>((xz + zx) / s,
                                           (yz + zy) / s,
                                           0.25f * s,
                                           (yx - xy) / s);
        }
    }

    inline juce::Quaternion<float> toQuaternion(const float roll, const float pitch, const float yaw)
    {
        // Quaternions and Rotation Sequence by Jack B. Kuipers, ISBN 0-691-10298-8, Page 167
        const auto psi = juce::degreesToRadians(yaw);
        const auto theta = juce::degreesToRadians(pitch);
        const auto phi = juce::degreesToRadians(roll);

        const auto cosHalfPsi = std::cos(psi * 0.5f);
        const auto sinHalfPsi = std::sin(psi * 0.5f);

        const auto cosHalfTheta = std::cos(theta * 0.5f);
        const auto sinHalfTheta = std::sin(theta * 0.5f);

        const auto cosHalfPhi = std::cos(phi * 0.5f);
        const auto sinHalfPhi = std::sin(phi * 0.5f);

        return juce::Quaternion<float>(cosHalfPsi * cosHalfTheta * sinHalfPhi - sinHalfPsi * sinHalfTheta * cosHalfPhi,
                                       cosHalfPsi * sinHalfTheta * cosHalfPhi + sinHalfPsi * cosHalfTheta * sinHalfPhi,
                                       sinHalfPsi * cosHalfTheta * cosHalfPhi - cosHalfPsi * sinHalfTheta * sinHalfPhi,
                                       cosHalfPsi * cosHalfTheta * cosHalfPhi + sinHalfPsi * sinHalfTheta * sinHalfPhi);
    }

    inline float asin(const float value)
    {
        if (value <= -1.0f)
        {
            return -1.0f * juce::MathConstants<float>::halfPi;
        }
        if (value >= 1.0f)
        {
            return juce::MathConstants<float>::halfPi;
        }
        return std::asin(value);
    }

    inline juce::Vector3D<float> toEulerAngles(const float x, const float y, const float z, const float w)
    {
        // Quaternions and Rotation Sequence by Jack B. Kuipers, ISBN 0-691-10298-8, Page 168
        const auto roll = juce::radiansToDegrees(std::atan2(2.0f * (y * z + w * x), 2.0f * (w * w - 0.5f + z * z)));
        const auto pitch = juce::radiansToDegrees(-1.0f * asin(2.0f * (x * z - w * y)));
        const auto yaw = juce::radiansToDegrees(std::atan2(2.0f * (x * y + w * z), 2.0f * (w * w - 0.5f + x * x)));

        return juce::Vector3D<float> { roll, pitch, yaw };
    }

    inline juce::String formatTimestamp(const uint64_t timestamp)
    {
        return juce::String(1E-6f * (float) timestamp, 3);
    }

    inline juce::String removeEscapeCharacters(const juce::String& input)
    {
        juce::String output;

        for (int index = 0; index < input.length(); index++)
        {
            if (input[index] != '\\')
            {
                output += input[index];
                continue;
            }

            if (++index >= input.length())
            {
                return output; // invalid escape sequence
            }

            switch (input[index])
            {
                case '\\':
                    output += '\\';
                    break;

                case 'n':
                    output += '\n';
                    break;

                case 'r':
                    output += '\r';
                    break;

                case 'x':
                {
                    if (index >= input.length() - 2)
                    {
                        return output; // invalid escape sequence
                    }

                    const auto upperNibble = juce::CharacterFunctions::getHexDigitValue((juce::juce_wchar) (juce::uint8) input[++index]);
                    const auto lowerNibble = juce::CharacterFunctions::getHexDigitValue((juce::juce_wchar) (juce::uint8) input[++index]);

                    if (upperNibble == -1 || lowerNibble == -1)
                    {
                        break; // invalid escape sequence
                    }

                    output += (char) ((upperNibble << 4) + lowerNibble);
                    break;
                }

                default:
                    break; // invalid escape sequence
            }
        }

        return output;
    }

    inline std::vector<juce::String> addEscapeCharacters(const juce::String& input)
    {
        std::vector<juce::String> output(1);

        for (const auto character : input)
        {
            if (juce::CharacterFunctions::isPrintable(character))
            {
                if (output.back()[0] == '\\')
                {
                    output.push_back({});
                }

                output.back() += character;
                continue;
            }

            if (output.back().isNotEmpty())
            {
                output.push_back({});
            }

            switch (character)
            {
                case '\\':
                    output.back() += "\\\\";
                    break;

                case '\n':
                    output.back() += "\\n";
                    break;

                case '\r':
                    output.back() += "\\r";
                    break;

                default:
                    output.back() += "\\x" + juce::String::toHexString(character).paddedLeft('0', 2);
                    break;
            }
        }

        return output;
    }
}
