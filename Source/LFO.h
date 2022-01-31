/*
  ==============================================================================

    LFO.h
    Created: 20 Jan 2022 10:39:02pm
    Author:  svkly

  ==============================================================================
*/

#include <JuceHeader.h>

class LFO
{
public:
    LFO();
    ~LFO();

    void prepare(double);
    void setFrequency(float);
    float getValue();

private:
    int sampleRate;
    const static int length = 1024;
    float incStep = 0.0f;
    float sineTable[length] = { 0.0f };
    float pReadFloat = 0.0f;

};