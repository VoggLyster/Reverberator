/*
  ==============================================================================

    PropEq.h
    Created: 22 Feb 2022 3:19:05pm
    Author:  svkly

  ==============================================================================
*/
#include <JuceHeader.h>
#include "Eigen/Dense"
#include "Eigen/src/Core/Transpose.h"
#define N_EQ 10
#define N_DF 19

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
    PropEQ(double samplerate);
    ~PropEQ() {}

    void setGainVector(std::vector<double> gainVector);
    float process(float input);
    void reset();
    void setPolesAndZeros();

private:
    std::vector<double> getCoefficients(double g, double gb, double w0, double bw);
    void updateState(int idx);
    float filter(double input, int idx);
    void setInteractionMatrix(double* g, double gw, double* wg, double* wc, double* bw);
    float clampValue(float v, float lower, float upper);
    void zeroCoefficients();

    double samplerate;
    bool isReady = false;

    std::vector<double> g;
    std::vector<double> g2;

    std::vector<double> tempCoeffs;
    double x0;
    State states[N_EQ + 1];
    Coeffs coeffs[N_EQ + 1];

    double commandGainFrequencies[N_EQ] = { 31.25, 62.5, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
    double centerFrequencies[N_DF] = { 31.25, 44.2, 62.5, 88.39, 125, 176.78, 250, 353.55, 500, 707.11, 1000, 1414.2, 2000, 2828.4, 4000, 5656.9, 8000, 11313.7, 16000 };
    double bandwidths[N_EQ];
    double omegaGainFrequencies[N_EQ];
    double omegaCenterFrequencies[N_DF];
    const double cParameter = 0.3f;
    const double protoGainValue = pow(10, 19.0 / 20.0);
    std::vector<std::vector<double>> interactionMatrix;
    std::vector<double> protoGains;

    Eigen::MatrixXd eigenInteractionMat{ N_DF, N_EQ };
    Eigen::MatrixXd eigenInteractionMat2{ N_DF, N_EQ };

    Eigen::VectorXd g2Vect;
    Eigen::VectorXd solution;
    Eigen::VectorXd g2optVect;
    Eigen::VectorXd solution2;

    std::vector<double> gopt;
    std::vector<double> Gopt;
    std::vector<double> g2opt;
    std::vector<double> G2opt;
    std::vector<double> g2wopt;
    std::vector<double> G2wopt;
};

#pragma once