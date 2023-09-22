#pragma once

#include <span>

class AxisLimits
{
public:
    static constexpr auto maxValue = 1e15f;
    static constexpr auto minValue = -maxValue;

    float min = 0.0f;
    float max = 1.0f;

    float getRange() const
    {
        return max - min;
    }

    void limitRange()
    {
        min = juce::jlimit(minValue, maxValue, min);
        max = juce::jlimit(minValue, maxValue, max);

        if (juce::exactlyEqual(min, max))
        {
            if (juce::exactlyEqual(min, minValue))
            {
                max = std::nextafter(max, std::numeric_limits<float>::max()); // increase max to prevent min from exceeding minValue
            }
            else
            {
                min = std::nextafter(min, std::numeric_limits<float>::lowest()); // decrease min to prevent min from exceeding minValue and avoid max exceeding 0 for x-axis
            }
        }

        jassert(max > min);
    }
};

class AxesLimits
{
public:
    AxisLimits x;
    AxisLimits y;

    AxesLimits()
    {
        setDefault();
    }

    void setDefault()
    {
        x.min = -5.0f;
        x.max = 0.0f;

        y.min = -1.0f;
        y.max = 1.0f;
    }

    void autoscale(const bool horizontal, const bool vertical, const std::vector<std::span<const juce::Point<GLfloat>>>& channels, const std::vector<bool>& enabledChannels)
    {
        x.limitRange();
        y.limitRange();

        if (channels.empty())
        {
            return;
        }

        if (channels[0].size() < 2)
        {
            return;
        }

        if (channels.size() != enabledChannels.size())
        {
            jassertfalse;
            return;
        }

        // Return if no channels enabled
        bool anyChannelEnabled = false;
        for (bool enabled : enabledChannels)
        {
            anyChannelEnabled |= enabled;
        }
        if (anyChannelEnabled == false)
        {
            return;
        }

        // Horizontal autoscale
        const float oldestTimestamp = channels[0].front().x;
        const float newestTimestamp = channels[0].back().x;
        if (horizontal)
        {
            x.min = oldestTimestamp;
            x.max = newestTimestamp;
        }

        // Vertical autoscale
        if (vertical)
        {
            AxisLimits newY { std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest() };

            for (size_t index = 0; index < channels.size(); index++)
            {
                if (enabledChannels[index] == false)
                {
                    continue;
                }
                for (const auto& point : channels[index])
                {
                    if (point.x < x.min)
                    {
                        continue;
                    }
                    if (point.x > x.max)
                    {
                        break;
                    }
                    newY.min = std::min(newY.min, point.y);
                    newY.max = std::max(newY.max, point.y);
                }
            }

            if (juce::exactlyEqual(newY.min, std::numeric_limits<float>::max()) == false)
            {
                const auto margin = juce::exactlyEqual(newY.min, newY.max) ? 1.0f : (0.02f * y.getRange()); // add 5% margin
                y.min = newY.min - margin;
                y.max = newY.max + margin;
            }
        }

        x.limitRange();
        y.limitRange();
    }
};
