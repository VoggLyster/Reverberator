/*
  ==============================================================================

    Reverb.cpp
    Created: 11 Oct 2021 5:29:24pm
    Author:  svkly

  ==============================================================================
*/

#include "Reverb.h"

ReverbProcessor::ReverbProcessor(int delays[N_LINES])
    : dlines(juce::dsp::Matrix<float>(N_LINES, juce::findMaximum(delays, N_LINES)))
{
    for (int i = 0; i < N_LINES; i++) {
        b[i] = 1.0f;
        c[i] = 1.0f;
        pwrite[i] = 0;
        pread[i] = 1;
        tempOut[i] = 0.0f;
        M[i] = delays[i];
    }
    A = juce::dsp::Matrix<float>(N_LINES, N_LINES, householder);
}

ReverbProcessor::~ReverbProcessor()
{
}

void ReverbProcessor::prepare(double samplerate, int samplesPerBlock)
{
    for (int i = 0; i < N_LINES; i++) {
        filters[i] = std::make_unique<Biquad>();
    }
    ready = true;
}

void ReverbProcessor::setParameters(std::atomic<float>* bParameters[N_LINES], std::atomic<float>* cParameters[N_LINES], std::atomic<float>* MParameters[N_LINES], std::atomic<float>* filterCoeffParameters[N_LINES][5]) {
    
    for (int i = 0; i < N_LINES; i++) {
        b[i] = *bParameters[i];
        c[i] = *cParameters[i];

        _M = *MParameters[i];
        if (M[i] != _M) {
            M[i] = _M;
            delayLenghtChanged = true;
        }
        filters[i]->setCoeffs(*filterCoeffParameters[i][0], *filterCoeffParameters[i][1], *filterCoeffParameters[i][2], *filterCoeffParameters[i][3], *filterCoeffParameters[i][4]);
    }
    if (delayLenghtChanged) {
        dlines = juce::dsp::Matrix<float>(N_LINES, juce::findMaximum(M, N_LINES));
        delayLenghtChanged = false;
    }
}

float ReverbProcessor::process(float input)
{
    float output = 0.0f;
    if (ready) {
        for (int i = 0; i < N_LINES; i++) {
            dlines(i, pwrite[i]) += b[i] * input;
            tempOut[i] = dlines(i, pread[i]);
            tempOut[i] = filters[i]->process(tempOut[i]);
            tempOut[i] *= c[i];
            for (int j = 0; j < N_LINES; j++) {
                dlines(j, pwrite[j]) += A(j, i) * tempOut[i];
            }
            // Increment pointers
            pread[i] = (pread[i] + 1) % M[i];
            pwrite[i] = (pwrite[i] + 1) % M[i];
            dlines(i, pwrite[i]) = 0.0f;
            output += tempOut[i];
        }
    }
    return output;
}

std::vector<float> ReverbProcessor::processStereo(std::vector<float> input)
{
    std::vector<float> output = { 0.0f, 0.0f };
    int n_lines = N_LINES / 2;
    if (ready) {
        for (int side = 0; side < 2; side++) {
            for (int i = side*n_lines; i < (side+1)*n_lines; i++) {
                dlines(i, pwrite[i]) += b[i] * input[side];
                tempOut[i] = dlines(i, pread[i]);
                tempOut[i] = filters[i]->process(tempOut[i]);
                tempOut[i] *= c[i];
                for (int j = 0; j < N_LINES; j++) {
                    dlines(j, pwrite[j]) += A(j, i) * tempOut[i];
                }
                // Increment pointers
                pread[i] = (pread[i] + 1) % M[i];
                pwrite[i] = (pwrite[i] + 1) % M[i];
                dlines(i, pwrite[i]) = 0.0f;
                output[side] += tempOut[i];
            }
        }

    }
    return output;
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
