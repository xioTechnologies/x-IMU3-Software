#pragma once

// TODO: Keep as is, or put createTicks inside for consistency within project?

struct Tick
{
    bool isMajor;
    float value;
    juce::String label;
};

static inline std::vector<Tick> createTicks(const int numberOfPixels, const AxisLimits& limits)
{
    const float range = limits.getRange();

    const int oomExponent = (int) std::floor(std::log10(range)); // order of magnitude's exponent ( 10^-1 [0.1s], 10^0 [1s], 10^1 [10s], 10^2 [100s], etc)

    // Calculate possible divisions of order of magnitude to display as major grid divisions.
    const auto previousOomDouble = std::pow(10.0f, (float) (oomExponent - 1)) * 2.0f;
    const auto oomFull = std::pow(10.0f, (float) oomExponent);
    const auto oomHalf = oomFull / 2.0f;
    const auto oomDouble = oomFull * 2.0f;
    const auto nextOomHalf = std::pow(10.0f, (float) (oomExponent + 1)) / 2.0f;

    // Set major tick distance by finding smallest oom division which is at least a minimum pixel length.
    constexpr auto minimumMajorDistancePixels = 25.0f;
    auto majorDistance = nextOomHalf;
    bool minorDivisionsUsesFourths = false;

    auto toPixels = [=](float dataUnits)
    {
        return (dataUnits / range) * (float) numberOfPixels;
    };

    if (toPixels(previousOomDouble) >= minimumMajorDistancePixels)
    {
        majorDistance = previousOomDouble;
        minorDivisionsUsesFourths = true;
    }
    else if (toPixels(oomHalf) >= minimumMajorDistancePixels)
    {
        majorDistance = oomHalf;
    }
    else if (toPixels(oomFull) >= minimumMajorDistancePixels)
    {
        majorDistance = oomFull;
    }
    else if (toPixels(oomDouble) >= minimumMajorDistancePixels)
    {
        majorDistance = oomDouble;
        minorDivisionsUsesFourths = true;
    }

    const unsigned int minorPerMajorDivisions = minorDivisionsUsesFourths ? 4 : 5;
    const float minorDistance = majorDistance / (float) minorPerMajorDivisions;

    // Prevent divide by 0 when adding text labels
    if (majorDistance <= 0.0 || minorPerMajorDivisions <= 0)
    {
        jassertfalse;
        return {};
    }

    // Create tick data from determined major and minor division distances
    std::vector<Tick> ticks;

    auto roundUpToNearestMultiple = [](float valueToRound, float multiple)
    {
        float remainderToClosestMultiple = std::fmod(std::abs(valueToRound), multiple);
        bool nearestMultipleIsLessThanValue = valueToRound > 0.0f;
        return valueToRound + ((nearestMultipleIsLessThanValue) ? (multiple - remainderToClosestMultiple) : remainderToClosestMultiple);
    };

    const float firstMajorPosition = roundUpToNearestMultiple(limits.min, majorDistance);
    const auto maxPossibleMajorTickCount = static_cast<unsigned int> (std::floor(limits.getRange() / majorDistance)) + 1;
    for (unsigned int majorTickIndex = 0; majorTickIndex < maxPossibleMajorTickCount; majorTickIndex++)
    {
        const float majorPosition = firstMajorPosition + (float) majorTickIndex * majorDistance;

        if (majorPosition > limits.max)
        {
            break;
        }

        auto approximatelyEqual = [](float a, float b, float epsilon)
        {
            return std::fabs(a - b) <= epsilon;
        };

        // Major ticks
        if (approximatelyEqual(majorPosition, 0.0f, majorDistance / (float) minorPerMajorDivisions))
        {
            ticks.push_back({ true, 0.0f, "0" }); // ensure 0 is written properly with no rounding error
        }
        else
        {
            ticks.push_back({ true, majorPosition, juce::String(majorPosition) });
        }

        // Minor ticks
        for (unsigned int minorTickIndex = 1; minorTickIndex < minorPerMajorDivisions; minorTickIndex++)
        {
            const float minorPosition = majorPosition + (float) minorTickIndex * minorDistance;
            if (minorPosition > limits.max)
            {
                break;
            }
            ticks.push_back({ false, minorPosition, {}});
        }
    }

    // Minor ticks prior to first major position
    for (unsigned int minorTickIndex = 1; minorTickIndex < minorPerMajorDivisions; minorTickIndex++)
    {
        const float minorPosition = firstMajorPosition - (float) minorTickIndex * minorDistance;
        if (minorPosition < limits.min)
        {
            break;
        }
        ticks.insert(ticks.begin(), { false, minorPosition, {}});
    }

    return ticks;
}
