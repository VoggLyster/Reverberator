/*
  ==============================================================================

    Reverb.h
    Created: 11 Oct 2021 5:29:24pm
    Author:  svkly

  ==============================================================================
*/
#include <JuceHeader.h>
#include "LFO.h"
#include "PropEQ.h"

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
    ReverbProcessor();
    ~ReverbProcessor();

    void prepare(double samplerate, int samplesPerBlock);
    void setParameters(/*std::atomic<float>* bParameters[N_LINES],
        std::atomic<float>* cParameters[N_LINES],*/
        std::atomic<float>* eqGainParameters[N_LINES][N_EQ]/*,
        std::atomic<float>* delayLengthMaxParameter,
        std::atomic<float>* delayLengthMinParameter,
        std::atomic<float>* modFrequencyParameters[N_LINES],
        std::atomic<float>* modDepthParameters[N_LINES]*/);
    float process(float input);
    std::vector<int> generateCoprimeRange(int delayLengthMaxSamples, int delayLengthMinSamples);
    int gcd(int a, int b);
    std::vector<int> generateDelayLineLengths(int delayLengthMaxSamples, int delayLengthMinSamples);
    int getModeDensity(std::vector<int> delayLengths);
    std::vector<int> getPrimePowerDelays(float minPath, float maxPath);

private:
    double fs = 48000; // Assume 48000 as default
    bool ready = false;
    float b[N_LINES];
    float c[N_LINES];
    float tempOut[N_LINES];
    int delayLengths[N_LINES];
    //int delayLengths[16] = {441,513,593,738,890,1135,1388,1689,2059,2446,2874,3388,3906,4466,5089,5755};
    int delayLengthMaxSamples;
    int delayLengthMinSamples;
    int primes[16] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53 };
    int c_ = 343; // Soundspeed in m/s at 20 degrees C for dry air
    std::unique_ptr<juce::dsp::DelayLine<float>> delayLines[N_LINES];
    float s[N_LINES];
    float s_prev[N_LINES];
    juce::dsp::Matrix<float> A = juce::dsp::Matrix<float>(N_LINES, N_LINES);
    std::unique_ptr<PropEQ> propEQs[N_LINES];
    float tempGain[N_EQ];
    std::unique_ptr<LFO> lfos[N_LINES];
    float lfoFrequencies[N_LINES];
    float modDepth[N_LINES];

    float predelayMaxLengthMs = 100;

    float householder16[256] =
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

    float householder4[16] =
    { 0.5, -0.5, -0.5, -0.5,
      -0.5, 0.5, -0.5, -0.5,
      -0.5, -0.5, 0.5, -0.5,
      -0.5, -0.5, -0.5, 0.5 };

    float householder2[4] = { 0.707106781, -0.707106781, -0.707106781, 0.707106781 };

    float householder1[1] = { 1.0f };

    //float hadamard[64] =
    //{
    //    0.3536,0.3536,0.3536,0.3536,0.3536,0.3536,0.3536,0.3536,
    //    0.3536,-0.3536,0.3536,-0.3536,0.3536,-0.3536,0.3536,-0.3536,
    //    0.3536,0.3536,-0.3536,-0.3536,0.3536,0.3536,-0.3536,-0.3536,
    //    0.3536,-0.3536,-0.3536,0.3536,0.3536,-0.3536,-0.3536,0.3536,
    //    0.3536,0.3536,0.3536,0.3536,-0.3536,-0.3536,-0.3536,-0.3536,
    //    0.3536,-0.3536,0.3536,-0.3536,-0.3536,0.3536,-0.3536,0.3536,
    //    0.3536,-0.3536,-0.3536,0.3536,-0.3536,0.3536,0.3536,-0.3536
    //};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbProcessor)
};

#pragma once
