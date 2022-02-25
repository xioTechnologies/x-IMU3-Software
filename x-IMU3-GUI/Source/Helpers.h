#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>

namespace Helpers
{
    inline juce::Quaternion<float> ToQuaternion(const float xx, const float xy, const float xz,
                                                const float yx, const float yy, const float yz,
                                                const float zx, const float zy, const float zz)
    {
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
        if (const auto tr = xx + yy + zz; tr > 0)
        {
            const auto s = std::sqrt(tr + 1.0f) * 2.0f; // s=4*w
            return juce::Quaternion<float>(-1.0f * (zy - yz) / s,
                                           -1.0f * (xz - zx) / s,
                                           -1.0f * (yx - xy) / s,
                                           0.25f * s);
        }
        else if ((xx > yy) & (xx > zz))
        {
            const auto s = std::sqrt(1.0f + xx - yy - zz) * 2.0f; // s=4*x
            return juce::Quaternion<float>(-1.0f * 0.25f * s,
                                           -1.0f * (xy + yx) / s,
                                           -1.0f * (xz + zx) / s,
                                           (zy - yz) / s);
        }
        else if (yy > zz)
        {
            const auto s = std::sqrt(1.0f + yy - xx - zz) * 2.0f; // s=4*y
            return juce::Quaternion<float>(-1.0f * (xy + yx) / s,
                                           -1.0f * 0.25f * s,
                                           -1.0f * (yz + zy) / s,
                                           (xz - zx) / s);
        }
        else
        {
            const auto s = std::sqrt(1.0f + zz - xx - yy) * 2.0f; // s=4*z
            return juce::Quaternion<float>(-1.0f * (xz + zx) / s,
                                           -1.0f * (yz + zy) / s,
                                           -1.0f * 0.25f * s,
                                           (yx - xy) / s);
        }
    }

    inline juce::Quaternion<float> ToQuaternion(const float roll, const float pitch, const float yaw)
    {
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
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

    inline juce::Vector3D<float> ToEulerAngles(const float x, const float y, const float z, const float w)
    {
        const auto roll = juce::radiansToDegrees(std::atan2(2.0f * (y * z - w * x), 2.0f * (w * w - 0.5f + z * z)));
        const auto pitch = juce::radiansToDegrees(-1.0f * std::asin(2.0f * (x * z + w * y)));
        const auto yaw = juce::radiansToDegrees(std::atan2(2.0f * (x * y - w * z), 2.0f * (w * w - 0.5f + x * x)));

        return juce::Vector3D<float> { roll, pitch, yaw };
    }

    inline juce::String formatTimestamp(const uint64_t timestamp)
    {
        return juce::String(1E-6f * (float) timestamp, 3);
    }
}
