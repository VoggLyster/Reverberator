/*
  ==============================================================================

    PropEQ.cpp
    Created: 22 Feb 2022 3:45:10pm
    Author:  svkly

  ==============================================================================
*/

#include "PropEQ.h"
#include <algorithm>

PropEQ::PropEQ(double _samplerate)
{
    samplerate = _samplerate;
   
    reset();
}

void PropEQ::setGainVector(std::vector<double> gainVector)
{
    gDB = gainVector;
    setPolesAndZeros();
}

void PropEQ::reset()
{
    Goptdb.clear();
    Gopt.clear();
    G2optdb.clear();
    G2opt.clear();
    G2woptdb.clear();
    G2wopt.clear();

    interactionMatrix.clear();

    Goptdb.resize(N_EQ, 0);
    Gopt.resize(N_EQ, 0);
    G2optdb.resize(N_EQ, 0);
    G2opt.resize(N_EQ, 0);
    G2woptdb.resize(N_EQ, 0);
    G2wopt.resize(N_EQ, 0);

    interactionMatrix.resize(N_EQ, std::vector<double>(N_DF, 0));

    for (int i = 0; i < N_EQ + 1; i++) {
        states[i].x1 = 0.f;
        states[i].x2 = 0.f;
        states[i].y0 = 0.f;
        states[i].y1 = 0.f;
        states[i].y2 = 0.f;
    }

    gDB.resize(N_EQ, 0);
    gDB2.resize(N_EQ, 0);

    zeroCoefficients();
}

float PropEQ::process(float input)
{
    for (int i = 0; i < N_EQ + 1; i++) {
        input = filter(input, i);
    }
    return input;
}

float PropEQ::filter(double x, int idx)
{
    x0 = x;
    states[idx].y0 = coeffs[idx].b0 * x0 + coeffs[idx].b1 * states[idx].x1 + coeffs[idx].b2 * states[idx].x2
        - coeffs[idx].a1 * states[idx].y1 - coeffs[idx].a2 * states[idx].y2;
    updateState(idx);
    return (float)states[idx].y0;
}

void PropEQ::setPolesAndZeros()
{
    float G0 = (gDB[N_EQ / 2] + gDB[N_EQ / 2 + 1]) * 0.5;

    for (int i = 0; i < N_EQ; i++) {
        gDB[i] = -G0 + gDB[i];
    }

    for (int i = 0; i < N_EQ; i++) {
        omegaGainFrequencies[i] = 2.0 * juce::double_Pi * commandGainFrequencies[i] / samplerate;
    }

    for (int i = 0; i < N_DF; i++) {
        omegaCenterFrequencies[i] = 2.0 * juce::double_Pi * centerFrequencies[i] / samplerate;
    }

    for (int i = 0; i < N_EQ; i++) {
        bandwidths[i] = 1.5 * omegaGainFrequencies[i];
    }

    bandwidths[N_EQ - 3] = 0.93 * bandwidths[N_EQ - 3];
    bandwidths[N_EQ - 2] = 0.78 * bandwidths[N_EQ - 2];
    bandwidths[N_EQ - 1] = 0.76 * omegaGainFrequencies[N_EQ - 1];

    protoGains.resize(N_EQ, protoGainValue);
    setInteractionMatrix(&protoGains[0], cParameter, &omegaGainFrequencies[0], &omegaCenterFrequencies[0], &bandwidths[0]);

    gDB2.resize(N_DF, 0);

    for (int i = 0; i < N_DF; i++) {
        if (i % 2 == 0)
            gDB2[i] = gDB[i / 2];
        else
            gDB2[i] = (gDB2[i - 1] + gDB2[i + 1]) / 2.0;
    }

    for (int i = 0; i < N_EQ; i++)
    {
        for (int j = 0; j < N_DF; j++)
        {
            eigenInteractionMat(j, i) = interactionMatrix[i][j];
        }
    }

    Gdb2Vect = Eigen::VectorXd::Map(&gDB2[0], gDB2.size());
    Gdb2Vect = Gdb2Vect.transpose();
    solution = eigenInteractionMat.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Gdb2Vect);

    for (int i = 0; i < N_EQ; i++)
    {
        Goptdb[i] = solution(i);
    }

    for (int k = 0; k < N_EQ; k++)
    {
        Gopt[k] = pow(10, Goptdb[k] / 20.0);
    }

    setInteractionMatrix(&Gopt[0], cParameter, &omegaGainFrequencies[0], &omegaCenterFrequencies[0], &bandwidths[0]);

    for (int i = 0; i < N_EQ; i++)
    {
        for (int j = 0; j < N_DF; j++)
        {
            eigenInteractionMat2(j, i) = interactionMatrix[i][j];
        }
    }

    G2optdbVect = Eigen::VectorXd::Map(&gDB2[0], gDB2.size());
    G2optdbVect = G2optdbVect.transpose();
    solution2 = eigenInteractionMat2.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(G2optdbVect);

    for (int i = 0; i < N_EQ; i++)
    {
        G2optdb[i] = solution2(i);
		if (G2optdb[i] <= 0.0)
			G2optdb[i] = 0.0;
    }

    for (int k = 0; k < N_EQ; k++)
    {
        G2opt[k] = pow(10, G2optdb[k] / 20.0);
        G2woptdb[k] = cParameter * G2optdb[k];
        G2wopt[k] = pow(10, G2woptdb[k] / 20.0);
    }

    for (int i = 0; i < N_EQ; i++) {
        tempCoeffs = getCoefficients(G2opt[i], G2wopt[i], omegaGainFrequencies[i], bandwidths[i]);
        coeffs[i].b0 = tempCoeffs[0] * pow(10, (G0 / 200.0));
        coeffs[i].b1 = tempCoeffs[1] * pow(10, (G0 / 200.0));
        coeffs[i].b2 = tempCoeffs[2] * pow(10, (G0 / 200.0));
        coeffs[i].a0 = tempCoeffs[3];
        coeffs[i].a1 = tempCoeffs[4];
        coeffs[i].a2 = tempCoeffs[5];
    }

    float fH = 20200.0f;
    float gH = pow(10, gDB[N_EQ - 1] / 20.0);
    float wH = 2.0 * juce::double_Pi * fH / samplerate;
    float tmp = sqrt(gH) * tan(wH * 0.5);

    float aH0 = tmp + 1;
    float aH1 = tmp - 1;
    float bH0 = tmp + gH;
    float bH1 = tmp - gH;

    coeffs[N_EQ].b0 = bH0 / aH0;
    coeffs[N_EQ].b1 = bH1 / aH0;
    coeffs[N_EQ].a0 = aH0 / aH0;
    coeffs[N_EQ].a1 = aH1 / aH0;
}

void PropEQ::setInteractionMatrix(double* g, double gw, double* wg, double* wc, double* bw)
{
    std::vector<std::complex<double>> dummyVect(N_EQ, 0);
    std::vector<double> Gdb(N_EQ, 0);
    std::vector<double> Gdbw(N_EQ, 0);
    std::vector<double> Gw(N_EQ, 0);
    double sumG = 0;
    std::vector<double> Gain(N_EQ, 0);
    std::complex<double> i(0, -1);

    for (int i = 0; i < N_EQ; i++)
    {
        Gdb[i] = 20 * log10(g[i]);
        Gdbw[i] = gw * Gdb[i];
        Gw[i] = pow(10, (Gdbw[i] / 20));
        sumG += Gdb[i];
    }

    std::vector<double> _coeffs(6, 0);
    std::vector<std::complex<double>> complexCoeffs(6, 0);
    std::complex<double> H;

    if (abs(sumG) != 0)
    {
        for (int m = 0; m < N_EQ; m++) {
            _coeffs = getCoefficients(g[m], Gw[m], wg[m], bw[m]);
            for (int j = 0; j < _coeffs.size(); ++j)
            {
                complexCoeffs[j].real(_coeffs[j]);
            }

            for (int k = 0; k < N_DF; k++)
            {
                H = (complexCoeffs[0] + complexCoeffs[1] * exp(-i * wc[k])
                    + complexCoeffs[2] * exp(-2.0 * i * wc[k]))
                    / (complexCoeffs[3] + complexCoeffs[4] * exp(-i * wc[k])
                        + complexCoeffs[5] * exp(-2.0 * i * wc[k]));
                interactionMatrix[m][k] = abs(20.0 * log10(H) / Gdb[m]);
            }
        }
    }

}

void PropEQ::zeroCoefficients()
{
    for (int i = 0; i < N_EQ + 1; i++) {
        coeffs[i].b0 = 0.0;
        coeffs[i].b1 = 0.0;
        coeffs[i].b2 = 0.0;
        coeffs[i].a0 = 0.0;
        coeffs[i].a1 = 0.0;
        coeffs[i].a2 = 0.0;
    }

}

float PropEQ::clampValue(float v, float lower, float upper)
{
    return std::max(lower, std::min(v, upper));
}

std::vector<double> PropEQ::getCoefficients(double g, double gb, double w0, double bw)
{
    double beta;
    if (g == 1)
        beta = tan(bw / 2.0);
    else
        beta = sqrt(abs(gb * gb - 1) / abs(g * g - gb * gb)) * tan(bw / 2.0);

    double b1 = (1.0 + g * beta) / (1.0 + beta);
    double b2 = (-2.0 * cos(w0)) / (1.0 + beta);
    double b3 = (1.0 - g * beta) / (1.0 + beta);

    double a1 = 1.0;
    double a2 = (-2.0 * cos(w0)) / (1.0 + beta);
    double a3 = (1.0 - beta) / (1.0 + beta);

    return std::vector<double> {b1, b2, b3, a1, a2, a3};
}

void PropEQ::enforceStability()
{
	//for (int i = 0; i < N_EQ; i++)
	//{
 //       if (abs(coeffs[i].a1) >= 1.0 + coeffs[i].a2)
 //       {

 //       }
 //       if (abs(coeffs[i].a2) >= 1.0)
 //       {

 //       }
	//}
}

void PropEQ::updateState(int idx)
{
    states[idx].x2 = states[idx].x1;
    states[idx].x1 = x0;
    states[idx].y2 = states[idx].y1;
    states[idx].y1 = states[idx].y0;

	 //Clip states for stability - this should be addressed in the future
    states[idx].x2 = clampValue(states[idx].x2, -1.5f, 1.5f);
    states[idx].x1 = clampValue(states[idx].x1, -1.5f, 1.5f);
    states[idx].y2 = clampValue(states[idx].y2, -1.5f, 1.5f);
    states[idx].y1 = clampValue(states[idx].y1, -1.5f, 1.5f);

}


