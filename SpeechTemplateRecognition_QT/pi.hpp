#ifndef PI_HPP
#define PI_HPP

#include "regulator.hpp"


class PI : public Regulator
{
public:
    PI(double gain, double I);
    double SimulateStep(double error);

    double I;
private:
    double cumulativeIntegral;
};

#endif // PI_HPP
