/*
  ==============================================================================

    Reverb.cpp
    Created: 11 Oct 2021 5:29:24pm
    Author:  svkly

  ==============================================================================
*/

#include "Reverb.h"

ReverbProcessor::ReverbProcessor()
{
    for (int i = 0; i < N_LINES; i++) {
        b[i] = 1.0f;
        c[i] = 1.0f;
        tempOut[i] = 0.0f;
        s[i] = 0.0f;
        s_prev[i] = 0.0f;
        delayLengths[i] = 0;
        modDepth[i] = 10;
    }
    const float* householder;
    switch (N_LINES)
    {
    case 16:
        householder = householder16;
        break;
    case 4:
        householder = householder4;
        break;
    case 2:
        householder = householder2;
        break;
    default:
        householder = householder1;
        break;
    }
    A = juce::dsp::Matrix<float>(N_LINES, N_LINES, householder);
    for (int i = 0; i < N_LINES; i++) {
        delayLines[i] = std::make_unique<juce::dsp::DelayLine<float>>(6000);
        propEQs[i] = std::make_unique<PropEQ>();
        lfos[i] = std::make_unique<LFO>();
        lfoFrequencies[i] = juce::Random::getSystemRandom().nextFloat() * 3.0f;
    }
}

ReverbProcessor::~ReverbProcessor()
{
}

void ReverbProcessor::prepare(double samplerate, int samplesPerBlock)
{
    fs = samplerate;
    dsp::ProcessSpec procSpec = dsp::ProcessSpec();
    procSpec.maximumBlockSize = samplesPerBlock;
    procSpec.numChannels = 2;
    procSpec.sampleRate = samplerate;
    for (int i = 0; i < N_LINES; i++) {
        delayLines[i]->prepare(procSpec);
        propEQs[i]->prepare(fs);
        lfos[i]->prepare(fs);
        lfos[i]->setFrequency(lfoFrequencies[i]);      
    }
    ready = true;
}

void ReverbProcessor::setParameters(std::atomic<float>* bParameters[N_LINES], 
    std::atomic<float>* cParameters[N_LINES], 
    std::atomic<float>* eqGainParameters[N_LINES][N_EQ],
    std::atomic<float>* delayLengthMaxParameter,
    std::atomic<float>* delayLengthMinParameter,
    std::atomic<float>* modFrequencyParameters[N_LINES],
    std::atomic<float>* modDepthParameters[N_LINES]) {
    
    delayLengthMaxSamples = 2500 + int(5000 * *delayLengthMaxParameter);
    delayLengthMinSamples = 100 + int(2400 * *delayLengthMinParameter);
    std::vector<int> delayLengths_ = generateCoprimeRange(delayLengthMaxSamples, delayLengthMinSamples);

    for (int i = 0; i < N_LINES; i++) {
        b[i] = *bParameters[i];
        c[i] = *cParameters[i];

        for (int j = 0; j < N_EQ; j++) {
            tempGain[j] = (*eqGainParameters[i][j] * 0.5) + 0.5;
        }

        propEQs[i]->setGainVector(tempGain);
        delayLengths[i] = delayLengths_[i];
        delayLines[i]->setDelay(delayLengths[i]);
        lfos[i]->setFrequency(*modFrequencyParameters[i] * 3.0f);
        modDepth[i] = *modDepthParameters[i] * 10.0f;
    }
}

float ReverbProcessor::process(float input)
{
    float output = 0.0f;
    for (int i = 0; i < N_LINES; i++) {
        s[i] = 0.0f;
    }
    if (ready) {
        for (int i = 0; i < N_LINES; i++) {
            float fdelay = delayLengths[i] - lfos[i]->getValue() * modDepth[i];
            tempOut[i] = delayLines[i]->popSample(0, fdelay, true);
            delayLines[i]->pushSample(0, b[i] * input + s_prev[i]);
            tempOut[i] = propEQs[i]->process(tempOut[i]);
            tempOut[i] *= c[i];
            for (int j = 0; j < N_LINES; j++) {
                s[j] += A(j, i) * tempOut[i];
            }
            output += tempOut[i];
        }
        for (int i = 0; i < N_LINES; i++) {
            s_prev[i] = s[i];
        }
    }
    return output;
}

std::vector<float> ReverbProcessor::processStereo(std::vector<float> input)
{
    std::vector<float> output = { 0.0f, 0.0f };
    //int n_lines = N_LINES / 2;
    //if (ready) {
    //    for (int side = 0; side < 2; side++) {
    //        for (int i = side*n_lines; i < (side+1)*n_lines; i++) {
    //            dlines(i, pwrite[i]) += b[i] * input[side];
    //            tempOut[i] = dlines(i, pread[i]);
    //            tempOut[i] = filters[i]->process(tempOut[i]);
    //            tempOut[i] *= c[i];
    //            for (int j = 0; j < N_LINES; j++) {
    //                dlines(j, pwrite[j]) += A(j, i) * tempOut[i];
    //            }
    //            // Increment pointers
    //            pread[i] = (pread[i] + 1) % M[i];
    //            pwrite[i] = (pwrite[i] + 1) % M[i];
    //            dlines(i, pwrite[i]) = 0.0f;
    //            output[side] += tempOut[i];
    //        }
    //    }

    //}
    return output;
}

std::vector<int> ReverbProcessor::generateCoprimeRange(int delayLengthMaxSamples, int delayLengthMinSamples)
{
    int range = delayLengthMaxSamples - delayLengthMinSamples;
    int rangeInterval = range / jmax((N_LINES - 1),1);
    
    std::vector<int> coprimeRange = std::vector<int>();

    for (int i = 0; i < N_LINES; i++) {
        int number = delayLengthMinSamples + (i * rangeInterval);
        if (i > 0) {
            for (int j = i-1; j >= 0; j--) {
                while (gcd(number, coprimeRange[j]) != 1) {
                    number -= 1;
                }
            }
        }
        coprimeRange.push_back(number);
    }

    return coprimeRange;
}

int ReverbProcessor::gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

Biquad::Biquad() {}

void Biquad::setCoeffs(float _b0, float _b1, float _b2, float _a1, float _a2) {
    if (b0 != _b0 || b1 != _b1 || b2 != _b2 || a1 != _a1 || a2 != _a2) {
        b0 = _b0;
        b1 = _b1;
        b2 = _b2;
        a1 = _a1;
        a2 = _a2;
        x1 = x2 = y0 = y1 = y2 = 0.0f;
    }
}

float Biquad::process(float input) {
    x0 = input;
    y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
    y2 = y1;
    y1 = y0;
    x2 = x1;
    x1 = x0;

    return y0;
}

SVF::SVF() {}

void SVF::setParameters(float _c_hp, float _c_bp, float _c_lp, float _R, float _g) {
    if (c_hp != _c_hp || c_bp != _c_bp || c_lp != _c_lp || R != _R || g != _g) {
        c_hp = _c_hp;
        c_bp = _c_bp;
        c_lp = _c_lp;
        R = _R;
        g = _g;
        h1, h2 = 0.0f;
        y_hp, y_bp, y_lp = 0.0f;
    }
}

float SVF::process(float input) {
    y_bp = (g * (input - h2 + h1)) / (1 + g * (g + 2 * R));
    y_lp = g * y_bp + h2;
    y_hp = input - y_lp - 2 * R * y_bp;
    h1 = 2 * y_bp - h1;
    h2 = 2 * y_lp - h2;
    return c_hp * y_hp + c_bp * y_bp + c_lp * y_lp;
}