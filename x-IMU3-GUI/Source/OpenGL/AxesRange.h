#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class AxesRange
{
public:
    static constexpr double rangeMin = 0.001, rangeMax = 1000000.0;

    double xMin, xMax, yMin, yMax;

    double getXRange() const;

    double getYRange() const;

    double getXScale() const;

    double getYScale() const;

    double getXCenter() const;

    double getYCenter() const;

    void setXRange(double value);

    void setYRange(double value);

    void setXCenter(double value);

    void setYCenter(double value);

    void getMostSignificantTickX(double& minTick, double& tickSize);

    void getMostSignificantTickY(double& minTick, double& tickSize);

private:
    static void getMostSignificantTick(double& minTick, double& tickSize, double range, double min);
};
