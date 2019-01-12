#include "pi.hpp"

PI::PI(double gain, double I) :
    Regulator()
{
    cumulativeIntegral = 0.0;
    s_Gain = gain;
    this->I = I;
}

double PI::SimulateStep(double error)
{
    double proportional = s_Gain * error;
    double integral = cumulativeIntegral + s_Gain * 1/I * error;
    return proportional + integral;
}
