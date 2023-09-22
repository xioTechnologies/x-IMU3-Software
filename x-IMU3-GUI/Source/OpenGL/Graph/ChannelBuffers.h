#pragma once

#include <span>

class ChannelBuffers
{
public:
    ChannelBuffers(const int numberOfChannels) : channelBuffers((size_t) numberOfChannels)
    {

    }

    void clear()
    {
        clearPending = true;
    }

    std::vector<std::span<const juce::Point<GLfloat>>> read()
    {
        updateChannelBuffers();

        std::vector<std::span<const juce::Point<GLfloat>>> channels;
        for (auto& channel : channelBuffers)
        {
            channels.push_back({ channel.cbegin(), channel.cbegin() + numberAvailable });
        }
        return channels;
    }

    void write(const uint64_t timestamp, const std::vector<float>& values)
    {
        if (timestamp < mostRecentTimestamp)
        {
            fifo.reset();
            clearPending = true;
        }
        mostRecentTimestamp = timestamp;

        juce::AbstractFifo::ScopedWrite(fifo, 1).forEach([&](auto index)
                                                         {
                                                             fifoData[(size_t) index] = { timestamp, values };
                                                             fifoData[(size_t) index].values.resize(channelBuffers.size());
                                                         });
    }

private:
    struct FifoDatum
    {
        uint64_t timestamp;
        std::vector<float> values;
    };
    std::array<FifoDatum, 1 << 10> fifoData;
    juce::AbstractFifo fifo { (int) fifoData.size() };

    std::array<uint64_t, GLResources::graphBufferSize> timestamps;
    std::vector<std::array<juce::Point<GLfloat>, GLResources::graphBufferSize>> channelBuffers;

    int numberAvailable = 0;

    std::atomic<bool> clearPending { false };

    uint64_t mostRecentTimestamp = 0;

    void updateChannelBuffers()
    {
        if (clearPending.exchange(false))
        {
            numberAvailable = 0;
        }

        // Get number of data available in fifo
        const auto fifoNumReady = fifo.getNumReady();

        // Return if fifo is empty
        if (fifoNumReady == 0)
        {
            return;
        }

        // Shift buffers to make space
        if (const auto shiftAmount = (numberAvailable + fifoNumReady - GLResources::graphBufferSize); shiftAmount > 0)
        {
            std::copy(timestamps.begin() + shiftAmount, timestamps.end(), timestamps.begin());
            for (auto& channelBuffer : channelBuffers)
            {
                std::copy(channelBuffer.begin() + shiftAmount, channelBuffer.end(), channelBuffer.begin());
            }
        }

        // Copy new data
        auto writeIndex = std::min(GLResources::graphBufferSize - fifoNumReady, numberAvailable);
        juce::AbstractFifo::ScopedRead(fifo, (int) fifoNumReady).forEach([&](auto fifoIndex)
                                                                         {
                                                                             timestamps[(size_t) writeIndex] = fifoData[(size_t) fifoIndex].timestamp;

                                                                             for (size_t lineIndex = 0; lineIndex < channelBuffers.size(); lineIndex++)
                                                                             {
                                                                                 channelBuffers[lineIndex][(size_t) writeIndex].y = fifoData[(size_t) fifoIndex].values[lineIndex];
                                                                             }

                                                                             writeIndex++;
                                                                         });

        numberAvailable = std::min(GLResources::graphBufferSize, numberAvailable + fifoNumReady);

        // Update timestamps
        for (size_t lineIndex = 0; lineIndex < channelBuffers.size(); lineIndex++)
        {
            auto& channelBuffer = channelBuffers[lineIndex];
            for (int dataIndex = 0; dataIndex < numberAvailable; dataIndex++)
            {
                channelBuffer[(size_t) dataIndex].x = -1E-6f * (float) (timestamps[(size_t) (numberAvailable - 1)] - timestamps[(size_t) dataIndex]); // convert micro seconds to seconds
            }
        }
    }
};
