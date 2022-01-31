/*
  ==============================================================================

    Reverb.h
    Created: 11 Oct 2021 5:29:24pm
    Author:  svkly

  ==============================================================================
*/
#include <JuceHeader.h>
#include "LFO.h"
#define N_LINES 8

class Biquad
{
public:
    Biquad();
    ~Biquad() {}

    void setCoeffs(float _b0, float _b1, float _b2, float _a1, float _a2);
    float process(float input);

private:
    float b0, b1, b2, a1, a2 = 0.0f;
    float y0, y1, y2 = 0.0f;
    float x0, x1, x2 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Biquad)
};

class SVF
{
public:
    SVF();
    ~SVF() {}

    void setParameters(float c_hp, float c_bp, float c_lp, float R, float g);
    float process(float input);

private:
    float h1 = 0.0f;
    float h2 = 0.0f;
    float y_hp = 0.0f;
    float y_bp = 0.0f;
    float y_lp = 0.0f;
    float c_hp = 0.0f;
    float c_bp = 0.0f;
    float c_lp = 0.0f;
    float R = 0.0f;
    float g = 0.0f;
};

class ReverbProcessor
{
public:
    ReverbProcessor(int delays[N_LINES]);
    ~ReverbProcessor();

    void prepare(double samplerate, int samplesPerBlock);
    void setParameters(std::atomic<float>* bParameters[N_LINES], std::atomic<float>* cParameters[N_LINES], std::atomic<float>* filterCoeffParameters[N_LINES][5], std::atomic<float>* modFreqParameters[N_LINES]);
    float process(float input);
    std::vector<float> processStereo(std::vector<float> input);

private:
    bool ready = false;

    float c[N_LINES];
    float b[N_LINES];
    float tempOut[N_LINES];
    int M[N_LINES];
    
    juce::dsp::Matrix<float> dlines;
    float s[N_LINES];
    float s_prev[N_LINES];
    juce::dsp::Matrix<float> A = juce::dsp::Matrix<float>(N_LINES, N_LINES);
    int ptr[N_LINES];
    std::unique_ptr<SVF> filters[N_LINES];
    std::unique_ptr<LFO> lfos[N_LINES];
    float lfoFrequencies[N_LINES] = { 2.0f, 1.0f, 1.3f, 0.7f, 4.0f, 0.3f, 0.2f, 1.1f };

    float householder[256] =
    { 0.25, -0.25,	-0.25,	-0.25,	-0.25,	 0.25,	 0.25,	 0.25,	-0.25,  0.25,	 0.25,	 0.25,	-0.25,  0.25,	 0.25,	 0.25,
    -0.25,	 0.25, -0.25,	-0.25,	 0.25,	-0.25,	 0.25,	 0.25,	 0.25, -0.25,	 0.25,	 0.25,	 0.25, -0.25,	 0.25,	 0.25,
    -0.25,	-0.25,	 0.25,	-0.25,	 0.25,	 0.25,	-0.25,	 0.25,	 0.25,  0.25,	-0.25,	 0.25,	 0.25,  0.25,	-0.25,	 0.25,
    -0.25,	-0.25,	-0.25,	 0.25,	 0.25,	 0.25,	 0.25,	-0.25,	 0.25,  0.25,	 0.25,	-0.25,	 0.25,  0.25,	 0.25,	-0.25,
    -0.25,	 0.25,  0.25,	 0.25,	 0.25,	-0.25,	-0.25,	-0.25,	-0.25,  0.25,	 0.25,	 0.25,	-0.25,  0.25,	 0.25,	 0.25,
     0.25,	-0.25,	 0.25,	 0.25,	-0.25,	 0.25,	-0.25,	-0.25,	 0.25, -0.25,	 0.25,	 0.25,	 0.25, -0.25,	 0.25,	 0.25,
     0.25,	 0.25, -0.25,	 0.25,	-0.25,	-0.25,	 0.25,	-0.25,	 0.25,  0.25,	-0.25,	 0.25,	 0.25,  0.25,	-0.25,	 0.25,
     0.25,	 0.25,  0.25,	-0.25,	-0.25,	-0.25,	-0.25,	 0.25,	 0.25,  0.25,	 0.25,	-0.25,	 0.25,  0.25,	 0.25,	-0.25,
    -0.25,	 0.25,  0.25,	 0.25,	-0.25,	 0.25,	 0.25,	 0.25,	 0.25, -0.25,	-0.25,	-0.25,	-0.25,  0.25,	 0.25,	 0.25,
     0.25,	-0.25,	 0.25,	 0.25,	 0.25,	-0.25,	 0.25,	 0.25,	-0.25,  0.25,	-0.25,	-0.25,	 0.25, -0.25,	 0.25,	 0.25,
     0.25,	 0.25, -0.25,	 0.25,	 0.25,	 0.25,	-0.25,	 0.25,	-0.25, -0.25,	 0.25,	-0.25,	 0.25,  0.25,	-0.25,	 0.25,
     0.25,	 0.25,  0.25,	-0.25,	 0.25,	 0.25,	 0.25,	-0.25,	-0.25, -0.25,	-0.25,	 0.25,	 0.25,  0.25,	 0.25,	-0.25,
    -0.25,	 0.25,  0.25,	 0.25,	-0.25,	 0.25,	 0.25,	 0.25,	-0.25,  0.25,	 0.25,	 0.25,	 0.25, -0.25,	-0.25,	-0.25,
     0.25,	-0.25,	 0.25,	 0.25,	 0.25,	-0.25,	 0.25,	 0.25,	 0.25, -0.25,	 0.25,	 0.25,	-0.25,  0.25,	-0.25,	-0.25,
     0.25,	 0.25, -0.25,	 0.25,	 0.25,	 0.25,	-0.25,	 0.25,	 0.25,  0.25,	-0.25,	 0.25,	-0.25, -0.25,	 0.25,	-0.25,
     0.25,	 0.25,  0.25,	-0.25,	 0.25,	 0.25,	 0.25,	-0.25,	 0.25,  0.25,	 0.25,	-0.25,	-0.25, -0.25,	-0.25,	 0.25 };

    //float householder[16] =
    //{ 0.5, -0.5, -0.5, -0.5,
    //  -0.5, 0.5, -0.5, -0.5,
    //  -0.5, -0.5, 0.5, -0.5,
    //  -0.5, -0.5, -0.5, 0.5 };

    float hadamard[64] =
    {
        0.3536,0.3536,0.3536,0.3536,0.3536,0.3536,0.3536,0.3536,
        0.3536,-0.3536,0.3536,-0.3536,0.3536,-0.3536,0.3536,-0.3536,
        0.3536,0.3536,-0.3536,-0.3536,0.3536,0.3536,-0.3536,-0.3536,
        0.3536,-0.3536,-0.3536,0.3536,0.3536,-0.3536,-0.3536,0.3536,
        0.3536,0.3536,0.3536,0.3536,-0.3536,-0.3536,-0.3536,-0.3536,
        0.3536,-0.3536,0.3536,-0.3536,-0.3536,0.3536,-0.3536,0.3536,
        0.3536,-0.3536,-0.3536,0.3536,-0.3536,0.3536,0.3536,-0.3536
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbProcessor)
};

#pragma once
