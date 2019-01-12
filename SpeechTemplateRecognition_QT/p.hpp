#ifndef P_HPP
#define P_HPP

#include "regulator.hpp"

class P : public Regulator
{
public:
    P(double gain);
    double SimulateStep(double error);
private:
};

#endif // P_HPP
