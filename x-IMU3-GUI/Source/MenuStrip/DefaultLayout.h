#pragma once

#include "../Windows/WindowIDs.h"

namespace DefaultLayout
{
    static const juce::ValueTree single = { WindowIDs::Row, {},
                                            {
                                                    { WindowIDs::DeviceSettings, {{ WindowIDs::size, 0.4 }}},
                                                    { WindowIDs::Column, {}, {
                                                            { WindowIDs::Row, {}, {
                                                                    { WindowIDs::Gyroscope, {}},
                                                                    { WindowIDs::Accelerometer, {}},
                                                                    { WindowIDs::Magnetometer, {}},
                                                                    { WindowIDs::HighGAccelerometer, {}},
                                                            }},
                                                            { WindowIDs::Row, {}, {
                                                                    { WindowIDs::EulerAngles, {{ WindowIDs::size, 1 }}},
                                                                    { WindowIDs::ThreeDView, {{ WindowIDs::size, 3 }}},
                                                            }}
                                                    }}
                                            }};

    static const juce::ValueTree rows = { WindowIDs::Row, {},
                                          {
                                                  { WindowIDs::Gyroscope, {}},
                                                  { WindowIDs::Accelerometer, {}},
                                                  { WindowIDs::ThreeDView, {}},
                                          }};

    static const juce::ValueTree columns = { WindowIDs::Row, {},
                                             {
                                                     { WindowIDs::Column, {}, {
                                                             { WindowIDs::Row, {}, {
                                                                     { WindowIDs::Gyroscope, {}},
                                                                     { WindowIDs::Accelerometer, {}},
                                                             }},
                                                             { WindowIDs::Row, {}, {
                                                                     { WindowIDs::ThreeDView, {}},
                                                             }}
                                                     }}
                                             }};

    static const juce::ValueTree grid = { WindowIDs::Row, {},
                                          {
                                                  { WindowIDs::ThreeDView, {}}
                                          }};

    static const juce::ValueTree accordion = single;
}
