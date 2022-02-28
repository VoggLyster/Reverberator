/*
  ==============================================================================

    PropEq.h
    Created: 22 Feb 2022 3:19:05pm
    Author:  svkly

  ==============================================================================
*/
#include <JuceHeader.h>
#define N_EQ 10

struct State {
    float y0, y1, y2;
    float x1, x2;
};

struct Coeffs {
    float b0, b1, b2;
    float a0, a1, a2;
};

class PropEQ
{
public:
    PropEQ();
    ~PropEQ() {}

    void prepare(double _samplerate);
    void setGainVector(float gainVector[N_EQ]);
    float process(float input);

private:
    void setPolesAndZeros();
    float freqToW(float frequency);
    void updateState(int idx);

    double samplerate;

    // 0.25 < g < 4
    // All About Audio Equalization: Solutions and Frontiers, p.11
    float g[N_EQ] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    float g2 = 0.0f;
    float g4 = 0.0f;
    float g0 = 1.f;

    float omega = 0.f;
    float omega2 = 0.f;
    float Q = 0.15f;
    float wc = 0.f;
    float B = 0.f;
    float t = 0.f;
    float c = 0.f;

    Coeffs coeffs[N_EQ];
    float x0;

    State states[N_EQ];

    float centerFreqs[N_EQ] = { 31.5f ,63.f, 125.f, 250.f, 500.f, 1000.f, 2000.f, 3000.f, 8000.f, 16000.f };
};

#pragma once
