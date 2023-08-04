#include <algorithm>
#include "GraphDataBuffer.h"

GraphDataBuffer::GraphDataBuffer(const size_t numberOfLines) : lineBuffers(numberOfLines)
{
}

AxesRange GraphDataBuffer::update(AxesRange axesRange, const bool horizontalAutoscale, const bool verticalAutoscale, const std::array<std::atomic<bool>, 3>& visibleLines)
{
    if (clearPending.exchange(false) == true)
    {
        numberAvailable = 0;
    }

    // Get number available in FIFO
    const auto fifoNumReady = fifo.getNumReady();

    // Discard old data from buffer
    if (fifoNumReady > 0)
    {
        int destinationIndex = (int) bufferSize - 1;
        int sourceIndex = (int) bufferSize - 1 - fifoNumReady;

        while (sourceIndex >= 0)
        {
            timestampBuffer[(size_t) destinationIndex] = timestampBuffer[(size_t) sourceIndex];

            for (auto& lineBuffer : lineBuffers)
            {
                lineBuffer[(size_t) destinationIndex] = lineBuffer[(size_t) sourceIndex];
            }

            destinationIndex--;
            sourceIndex--;
        }

        // Write new data to buffer
        size_t bufferIndex = (size_t) fifoNumReady - 1;
        juce::AbstractFifo::ScopedRead(fifo, fifoNumReady).forEach([&](auto fifoIndex)
                                                                   {
                                                                       timestampBuffer[bufferIndex] = fifoData[(size_t) fifoIndex].timestamp;

                                                                       for (size_t lineIndex = 0; lineIndex < lineBuffers.size(); lineIndex++)
                                                                       {
                                                                           lineBuffers[lineIndex][bufferIndex].y = fifoData[(size_t) fifoIndex].values[lineIndex];
                                                                       }

                                                                       bufferIndex--;
                                                                       numberAvailable = std::min(bufferSize, numberAvailable + 1);
                                                                   });
    }

    // Recalculate timestamps and max/min
    auto yMin = std::numeric_limits<float>::max();
    auto yMax = std::numeric_limits<float>::lowest();

    for (size_t lineIndex = 0; lineIndex < lineBuffers.size(); lineIndex++)
    {
        auto& lineBuffer = lineBuffers[lineIndex];

        for (size_t dataIndex = 0; dataIndex < numberAvailable; dataIndex++)
        {
            const auto relativeTimestamp = -1E-6f * (float) (timestampBuffer[0] - timestampBuffer[dataIndex]);

            lineBuffer[dataIndex].x = relativeTimestamp;

            if (visibleLines[lineIndex] && (horizontalAutoscale || (relativeTimestamp >= axesRange.xMin && relativeTimestamp <= axesRange.xMax)))
            {
                yMin = std::min(yMin, lineBuffer[dataIndex].y);
                yMax = std::max(yMax, lineBuffer[dataIndex].y);
            }
        }
    }

    // Update axis range
    const auto xMin = lineBuffers[0][(size_t) std::max((int) numberAvailable - 1, 0)].x;
    const auto xMax = lineBuffers[0][0].x;
    if (horizontalAutoscale && numberAvailable > 1)
    {
        axesRange.xMin = xMin;
        axesRange.xMax = xMax;
    }

    if (verticalAutoscale)
    {
        if ((yMin - yMax) > 0.0f)
        {
            yMin -= 1.0f;
            yMax += 1.0f;
        }

        axesRange.yMin = yMin;
        axesRange.yMax = yMax;
    }

    return axesRange;
}

void GraphDataBuffer::write(const uint64_t timestamp, const std::vector<float>& values)
{
    if (timestamp < mostRecentTimestamp)
    {
        clear();
    }
    mostRecentTimestamp = timestamp;

    juce::AbstractFifo::ScopedWrite(fifo, 1).forEach([&](auto index)
                                                     {
                                                         fifoData[(size_t) index] = { timestamp, values };
                                                     });
}

void GraphDataBuffer::clear()
{
    clearPending = true;
    fifo.reset();
}

const std::vector<std::array<juce::Point<GLfloat>, GraphDataBuffer::bufferSize>>& GraphDataBuffer::getLineBuffers() const
{
    return lineBuffers;
}

size_t GraphDataBuffer::getNumberAvailable() const
{
    return numberAvailable;
}
