#include <algorithm>
#include "AxesRange.h"
#include <cmath>

double AxesRange::getXRange() const
{
    return xMax - xMin;
}

double AxesRange::getYRange() const
{
    return yMax - yMin;
}

double AxesRange::getXScale() const
{
    return 2.0 / getXRange();
}

double AxesRange::getYScale() const
{
    return 2.0 / getYRange();
}

double AxesRange::getXCenter() const
{
    return xMin + getXRange() * 0.5;
}

double AxesRange::getYCenter() const
{
    return yMin + getYRange() * 0.5;
}

void AxesRange::setXRange(double value)
{
    double center = getXCenter();

    double halfValue = value * 0.5;

    xMin = center - halfValue;
    xMax = center + halfValue;
}

void AxesRange::setYRange(double value)
{
    double center = getYCenter();

    double halfValue = value * 0.5;

    yMin = center - halfValue;
    yMax = center + halfValue;
}

void AxesRange::setXCenter(double value)
{
    double min = xMin;
    double max = xMax;

    min -= getXCenter();
    max -= getXCenter();

    min += value;
    max += value;

    xMin = min;
    xMax = max;
}

void AxesRange::setYCenter(double value)
{
    double min = yMin;
    double max = yMax;

    min -= getYCenter();
    max -= getYCenter();

    min += value;
    max += value;

    yMin = min;
    yMax = max;
}

void AxesRange::getMostSignificantTickX(double& minTick, double& tickSize)
{
    getMostSignificantTick(minTick, tickSize, getXRange(), xMin);
}

void AxesRange::getMostSignificantTickY(double& minTick, double& tickSize)
{
    getMostSignificantTick(minTick, tickSize, getYRange(), yMin);
}

void AxesRange::getMostSignificantTick(double& minTick, double& tickSize, double range, double min)
{
    range = std::clamp(range, rangeMin, rangeMax);

    int exponent = (int) std::ceil(std::log(range) / std::log(10));

    double orderOfMagnitude = std::pow(10, exponent);

    tickSize = orderOfMagnitude * 0.01f;

    double sign = (min > 0.0) ? 1.0 : (min < 0.0) ? -1.0 : 0.0;

    minTick = std::ceil(std::abs(min / orderOfMagnitude)) * orderOfMagnitude * sign;

    while (minTick > min)
    {
        minTick -= orderOfMagnitude;
    }
}
