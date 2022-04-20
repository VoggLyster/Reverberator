/*
  ==============================================================================

    PropEq.h
    Created: 22 Feb 2022 3:19:05pm
    Author:  svkly

  ==============================================================================
*/
#include <JuceHeader.h>
#define N_EQ 10

class PropEQ
{
public:
    PropEQ();
    ~PropEQ() {}

    void prepare(double samplerate);
    void setGainVector(float gainVector[N_EQ]);
    float process(float input);

private:
    void setPolesAndZeros();
    double samplerate;
    bool isReady = false;

    // 0.25 < g < 4
    // All About Audio Equalization: Solutions and Frontiers, p.11
    float g[N_EQ] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    float Q = 1.f / sqrtf(2.f);

    juce::dsp::IIR::Filter<float> iir[N_EQ];

    juce::Array<float> tmpCoeffs;
    juce::dsp::IIR::Coefficients<float> coeffs[N_EQ];
    float tmpVal;

    //float centerFreqs[N_EQ] = { 31.5f ,63.f, 125.f, 250.f, 500.f, 1000.f, 2000.f, 3000.f, 8000.f, 16000.f };
    float centerFreqPartials[N_EQ] = { 0.0007143f, 0.0014286f, 0.0028345f, 0.0056689f, 0.0113379f, 0.0226757f,
        0.0453515f, 0.0680272f, 0.1814059f, 0.3628118f };
};

#pragma once
