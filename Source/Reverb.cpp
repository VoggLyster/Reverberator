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
        b[i] = 0.0f;
        c[i] = 0.0f;
        tempOut[i] = 0.0f;
        s[i] = 0.0f;
        s_prev[i] = 0.0f;
        delayLengths[i] = 0;
        modDepth[i] = 0;
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
        delayLines[i] = std::make_unique<juce::dsp::DelayLine<float>>(maxDelayLineLength);
        propEQs[i] = std::make_unique<PropEQ>();
        lfos[i] = std::make_unique<LFO>();
        lfoFrequencies[i] = juce::Random::getSystemRandom().nextFloat() * 3.0f;
    }
    predelayLine = std::make_unique<juce::dsp::DelayLine<float>>(maxDelayLineLength);
    predelayLength = 0;
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
    int minDelayLength = int(0.01 * samplerate);
    int desiredModeDensity = int(0.15 * samplerate) - (30 * N_LINES);
    std::vector<int> delayLengths_ = getPrimePowerDelays(1.5, 40);
    int modeDensity = getModeDensity(delayLengths_);
    jassert(modeDensity > desiredModeDensity);
    for (int i = 0; i < N_LINES; i++) {
        delayLengths[i] = delayLengths_[i];
        delayLines[i]->prepare(procSpec);
        propEQs[i]->prepare(fs);
        lfos[i]->prepare(fs);
        lfos[i]->setFrequency(lfoFrequencies[i]);      
    }
    predelayLine->prepare(procSpec);
    ready = true;
}

void ReverbProcessor::setParameters(std::atomic<float>* bParameters[N_LINES],
    std::atomic<float>* cParameters[N_LINES],
    std::atomic<float>* eqGainParameters[N_EQ],
    std::atomic<float>* delayLengthMaxParameter,
    std::atomic<float>* delayLengthMinParameter,
    std::atomic<float>* predelayLengthParameter,
    std::atomic<float>* modFrequencyParameters[N_LINES],
    std::atomic<float>* modDepthParameters[N_LINES]) {
    
    delayLengthMin = 10.0f + 0.1f * *delayLengthMinParameter;
    delayLengthMax = delayLengthMin + 10.0f + 0.2f * *delayLengthMaxParameter;
    std::vector<int> delayLengths_ = getPrimePowerDelays(delayLengthMin, delayLengthMax);

    predelayLength = floor(*predelayLengthParameter/100.f * (fs/1000 * predelayMaxLengthMs));
    predelayLine->setDelay(predelayLength);
    tempGain.resize(N_EQ, 0.f);

    for (int j = 0; j < N_EQ; j++) {
        //tempGain[j] = (*eqGainParameters[j]/100.f * 29.0) - 30.0;
        tempGain[j] = (*eqGainParameters[j] / 100.f * 2.4f + 0.1f);
    }	

    for (int i = 0; i < N_LINES; i++) {
        b[i] = pow(10, (*bParameters[i] / 100.f * 12.f - 12.f) / 20.0);
        c[i] = pow(10, (*cParameters[i] / 100.f * 12.f - 12.f) / 20.0);

        delayLengths[i] = delayLengths_[i];
        DBG(juce::String(delayLengths[i]));
        delayLines[i]->setDelay(delayLengths[i]);
        propEQs[i]->setGainVector(RTtoGain(tempGain, i));
        lfos[i]->setFrequency(*modFrequencyParameters[i] * 0.03f);
        modDepth[i] = *modDepthParameters[i] * 0.1f;
    }
}

float ReverbProcessor::process(float input)
{
    float output = 0.0f;
    for (int i = 0; i < N_LINES; i++) {
        s[i] = 0.0f;
    }
    if (ready) {

        predelayLine->pushSample(0, input);
        input = predelayLine->popSample(0, -1, true);

        for (int i = 0; i < N_LINES; i++) {
            float fdelay = delayLengths[i] - lfos[i]->getValue() * modDepth[i];
            tempOut[i] = delayLines[i]->popSample(0, fdelay, true);
            delayLines[i]->pushSample(0, b[i] * input + s_prev[i]);
            tempOut[i] = propEQs[i]->process(tempOut[i]);
            for (int j = 0; j < N_LINES; j++) {
                s[j] += A(j, i) * tempOut[i];
            }
            output += tempOut[i] * c[i];
        }
        for (int i = 0; i < N_LINES; i++) {
            s_prev[i] = s[i];
        }
    }
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
        DBG(juce::String(number));
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

std::vector<int> ReverbProcessor::generateDelayLineLengths(int delayLengthMaxSamples, int delayLengthMinSamples)
{
    // https://ccrma.stanford.edu/~jos/pasp/Prime_Power_Delay_Line_Lengths.html

    int range = delayLengthMaxSamples - delayLengthMinSamples;
    int rangeInterval = range / jmax((N_LINES - 1), 1);

    std::vector<int> delayLengths = std::vector<int>();

    for (int i = 0; i < N_LINES; i++) {
        int Mi = delayLengthMinSamples + (i * rangeInterval);
        int mi = 0.5 + (logf(Mi) / logf(primes[i]));
        Mi = pow(primes[i],mi);
        DBG(juce::String(mi));
        DBG(juce::String(Mi));
        delayLengths.push_back(Mi);
    }

    return delayLengths;
}

int ReverbProcessor::getModeDensity(std::vector<int> delayLengths) {
    int modeDensity = 0;
    for (auto& n : delayLengths)
        modeDensity += n;
    return modeDensity;
}

std::vector<int> ReverbProcessor::getPrimePowerDelays(float minPath, float maxPath)
{
    // https://github.com/grame-cncm/faustlibraries/blob/master/delays.lib

    std::vector<int> _delayLengths = std::vector<int>();
    int dmin = fs * minPath / c_;
    if (N_LINES == 1) {
        _delayLengths.push_back(dmin);
        return _delayLengths;
    }
    int dmax = fs * maxPath / c_;
    for (int i = 0; i < N_LINES; i++) {
        int Mi = dmin * pow((dmax / dmin), (i / float(N_LINES - 1)));
        int mi = floor(0.5 + log(Mi) / log(primes[i]));
        Mi = juce::jmin(int(pow(primes[i], mi)), maxDelayLineLength);
        //DBG(juce::String(mi));
        DBG(juce::String(Mi));
        _delayLengths.push_back(Mi);
    }
    return _delayLengths;
}

std::vector<double> ReverbProcessor::RTtoGain(std::vector<float> RT, int idx)
{
	std::vector<double> gain = std::vector<double>();
    float delta;
	
	for (int i = 0; i < N_EQ; i++) {
        delta = (-60 / (fs * RT[i]));
        gain.push_back(delta * delayLengths[idx]);
	}

    return gain;
}

void ReverbProcessor::reset()
{
	for (int i = 0; i < N_LINES; i++) {
        delayLines[i]->reset();
        propEQs[i]->reset();
        s[i] = 0.0f;
		s_prev[i] = 0.0f;
	}
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