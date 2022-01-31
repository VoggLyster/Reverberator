/*
  ==============================================================================

    LFO.cpp
    Created: 20 Jan 2022 10:39:02pm
    Author:  svkly

  ==============================================================================
*/

#include "LFO.h"
#include <math.h>

LFO::LFO()
{
    for (int i = 0; i < length; i++) {
        sineTable[i] = 1 + cosf(float(i) / float(length) * (2.0 * juce::MathConstants<float>::pi));
    }
}

LFO::~LFO()
{

}

void LFO::prepare(double _sampleRate)
{
    sampleRate = int(_sampleRate);
}

void LFO::setFrequency(float freq)
{
    incStep = float(length) * (freq / float(sampleRate));
}

float LFO::getValue()
{
    pReadFloat += incStep;
    if (pReadFloat > length)
        pReadFloat -= length;
    int pRead = int(pReadFloat);
    return sineTable[pRead];
}
