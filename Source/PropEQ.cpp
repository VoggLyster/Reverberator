/*
  ==============================================================================

    PropEQ.cpp
    Created: 22 Feb 2022 3:45:10pm
    Author:  svkly

  ==============================================================================
*/

#include "PropEQ.h"

PropEQ::PropEQ()
{
    for (int i = 0; i < N_EQ; i++) {
        states[i].x1 = 0.f;
        states[i].x2 = 0.f;
        states[i].y0 = 0.f;
        states[i].y1 = 0.f;
        states[i].y2 = 0.f;
    }
}

void PropEQ::prepare(double _samplerate) {
    samplerate = _samplerate;
    setPolesAndZeros();
}

void PropEQ::setGainVector(float gainVector[N_EQ])
{
    for (int i = 0; i < N_EQ; i++) {
        g[i] = gainVector[i];
    }
    setPolesAndZeros();
}

float PropEQ::process(float input)
{
    x0 = input;
    for (int i = 1; i < N_EQ; i++) {
        states[i].y0 = 
            (coeffs[i].b0 / coeffs[i].a0) * x0 + (coeffs[i].b1 / coeffs[i].a0) * states[i].x1 
            + (coeffs[i].b2 / coeffs[i].a0) * states[i].x2 - (coeffs[i].a1 / coeffs[i].a0) * states[i].y1 
            - (coeffs[i].a2 / coeffs[i].a0) * states[i].y2;
        updateState(i);
        x0 = states[i].y0;
    }


    //float output = 0.0f;

    //for (int i = 0; i < N_EQ; i++) {
    //    output += states[i].y0;
    //}

    float output = g0 * states[N_EQ-1].y0;

    if (input >= 1.0f) {
        int test = 0;
    }

    return output;
}

void PropEQ::setPolesAndZeros()
{
    // Low shelf EQ
    omega = tanf(freqToW(centerFreqs[0]) / 2);
    omega2 = powf(omega, 2.f);
    g2 = powf(g[0], 0.5f);
    g4 = powf(g[0], 0.25f);
    coeffs[0].b0 = (g2 * omega2 + sqrt(2) * omega * g4 + 1) * g2;
    coeffs[0].b1 = (2 * g2 * omega - 2) * g2;
    coeffs[0].b2 = (g2 * omega2 - sqrt(2) * omega * g4 + 1) * g2;
    coeffs[0].a0 = g2 + sqrt(2) * omega * g4 + omega2;
    coeffs[0].a1 = 2 * omega2 - 2 * g2;
    coeffs[0].a2 = g2 - sqrt(2) * omega * g4 + omega2;

    // High shelf EQ
    int idx = N_EQ - 1;
    omega = tanf(freqToW(centerFreqs[idx]) / 2);
    omega2 = powf(omega, 2.f);
    g2 = powf(g[idx], 0.5f);
    g4 = powf(g[idx], 0.25f);
    coeffs[idx].b0 = (g2 * omega2 + sqrt(2) * omega * g4 + 1) * g2;
    coeffs[idx].b1 = (2 * g2 * omega - 2) * g2;
    coeffs[idx].b2 = (g2 * omega2 - sqrt(2) * omega * g4 + 1) * g2;
    coeffs[idx].a0 = g2 + sqrt(2) * omega * g4 + omega2;
    coeffs[idx].a1 = 2 * omega2 - 2 * g2;
    coeffs[idx].a2 = g2 - sqrt(2) * omega * g4 + omega2;
    coeffs[idx].b0 = coeffs[idx].a0 * g[idx];
    coeffs[idx].b1 = coeffs[idx].a1 * g[idx];
    coeffs[idx].b2 = coeffs[idx].a2 * g[idx];
    coeffs[idx].a0 = coeffs[idx].b0;
    coeffs[idx].a1 = coeffs[idx].b1;
    coeffs[idx].a2 = coeffs[idx].b2;

    // Notch EQs
    for (int i = 1; i < N_EQ - 1; i++) {
        float wc = freqToW(centerFreqs[i]);
        float B = wc / Q;
        float omega = tanf(B / 2.f);
        float c = cosf(wc);
        g2 = sqrtf(g[i]);
        coeffs[i].b0 = g2 + g[i] * omega;
        coeffs[i].b1 = -2 * g2 * c;
        coeffs[i].b2 = g2 - g[i] * omega;
        coeffs[i].a0 = g2 + omega;
        coeffs[i].a1 = -2 * g2 * c;
        coeffs[i].a2 = g2 - omega;
    }
}

float PropEQ::freqToW(float frequency)
{
    return frequency / (float)samplerate * juce::MathConstants<float>::twoPi;
}

void PropEQ::updateState(int idx)
{
    states[idx].x2 = states[idx].x1;
    states[idx].x1 = x0;
    states[idx].y2 = states[idx].y1;
    states[idx].y1 = states[idx].y0;
}
