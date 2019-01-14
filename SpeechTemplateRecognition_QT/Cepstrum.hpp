#pragma once
#include <complex>
#include <vector>
#include <math.h>
#include <numeric>
#include <algorithm>
#include <limits> 
#include <vector>

#include <boost/qvm/mat_operations.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/container/vector.hpp>

#include "Fft.h"

#include "MatrixOperations.hpp"

#include "QtAudioWrapper.hpp"


class Cepstrum
{
public:
    static std::tuple<std::vector<std::vector<double>>, int> calculateCxMatrix(std::vector<double> signal, int Mlen, int Mstep, int Nc, int Np);
    static int dtw(std::vector<std::vector<double>>, std::vector<std::vector<std::vector<double>>>, std::vector<int>);
    static std::tuple<std::vector<std::vector<double>>, int> CalculateCepstrum(std::vector<double> signal);
    static bool loadPatterns();

    static std::vector<std::vector<std::vector<double>>> extractCwzrFromPatterns( std::vector<std::tuple<std::vector<std::vector<double>>, int>> patterns);
    static std::vector<int> Cepstrum::extractNwzrFromCwzr(std::vector<std::vector<std::vector<double>>> Cwzr);

    static int detectedNumber;
    static double sum;
    static std::vector<std::tuple<std::vector<std::vector<double>>, int>> patterns;

private:
    static std::vector<double> silence(std::vector<double> signal, int fpr);
//    static std::vector<std::vector<double>> rCeps(std::vector<std::vector<double>> x);
//    static std::vector<std::vector<double>> stdVecToCArray(std::vector<double> x);
//    static std::vector<std::vector<double>> fft_x_abs(std::vector<std::vector<double>> x);

    static double calculateSquareErrorSum(std::vector<double> & Cx, std::vector<std::vector<double>> & Cwzr, int ns, int Np, int nw, int numberWzorca);

    static std::vector<std::vector<std::vector<double>>> Cwzr;
    static std::vector<int> Nwzr;
};

