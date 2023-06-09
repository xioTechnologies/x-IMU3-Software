#pragma once

#include "AxesRange.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGL/Common/GLResources.h"
#include <vector>

class GraphDataBuffer
{
    static constexpr size_t bufferSize = GLResources::graphBufferSize;

public:
    explicit GraphDataBuffer(const size_t numberOfLines);

    AxesRange update(AxesRange axesRange, const bool horizontalAutoscale, const bool verticalAutoscale, const std::array<std::atomic<bool>, 3>& visibleLines);

    void write(const uint64_t timestamp, const std::vector<float>& values);

    void clear();

    const std::vector<std::array<juce::Point<GLfloat>, bufferSize>>& getLineBuffers() const;

    size_t getNumberAvailable() const;

private:
    struct FifoDatum
    {
        uint64_t timestamp;
        std::vector<float> values;
    };
    std::array<FifoDatum, 1024> fifoData;
    juce::AbstractFifo fifo { (int) fifoData.size() };

    std::array<uint64_t, bufferSize> timestampBuffer;
    std::vector<std::array<juce::Point<GLfloat>, bufferSize>> lineBuffers;
    size_t numberAvailable = 0;

    std::atomic<uint64_t> mostRecentTimestamp { 0 };

    std::atomic<bool> clearPending = false;
};
