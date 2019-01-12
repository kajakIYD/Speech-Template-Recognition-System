#include "p.hpp"

P::P(double gain) :
    Regulator()
{
    s_Gain = gain;
}


double P::SimulateStep(double error)
{
    return s_Gain * error;
}

