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
    PropEQ();
    ~PropEQ() {}

    void prepare(double samplerate);
    void setGainVector(std::vector<double> gainVector);
    float process(float input);
    void reset();

private:
    void setPolesAndZeros();
    std::vector<double> getCoefficients(double g, double gb, double w0, double bw);
    void enforceStability();
    void updateState(int idx);
    float filter(double input, int idx);
    void interactionMatrix(double* g, double gw, double* wg, double* wc, double* bw);
    float clampValue(float v, float lower, float upper);

    double samplerate;
    bool isReady = false;

    std::vector<double> gDB;
    std::vector<double> gDB2;
    double g2[N_EQ];
    double g2wdb[N_EQ];
    double g2w[N_EQ];

    std::vector<double> tempCoeffs;
    double x0;
    State states[N_EQ];
    Coeffs coeffs[N_EQ];

    double commandGainFrequencies[N_EQ] = { 31.25, 62.5, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
    double centerFrequencies[N_DF] = { 31.25, 44.2, 62.5, 88.39, 125, 176.78, 250, 353.55, 500, 707.11, 1000, 1414.2, 2000, 2828.4, 4000, 5656.9, 8000, 11313.7, 16000 };
    double bandwidths[N_EQ];
    double wg[N_EQ];
    double wc[N_DF];
    const double gw = 0.3f;
    const double cc = pow(10, 19.0 / 20.0);
    std::vector<std::vector<double>> leak;
    std::vector<double> inG;

    Eigen::MatrixXd leakMat{ N_DF, N_EQ };
    Eigen::MatrixXd leakMat2{ N_DF, N_EQ };

    Eigen::VectorXd Gdb2Vect;
    Eigen::VectorXd solution;
    Eigen::VectorXd G2optdbVect;
    Eigen::VectorXd solution2;

    std::vector<double> Goptdb;
    std::vector<double> Gopt;
    std::vector<double> G2optdb;
    std::vector<double> G2opt;
    std::vector<double> G2woptdb;
    std::vector<double> G2wopt;
};

#pragma once