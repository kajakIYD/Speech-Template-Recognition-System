#ifndef REGULATOR_HPP
#define REGULATOR_HPP


class Regulator
{
public:
    virtual ~Regulator() {}
    double GetControl() {return s_Control;};
    virtual double SimulateStep(double error) = 0;
    void SetGain(double gain);

protected:
    double s_Error = 0;
    double s_Control = 0;
    double s_Gain = 0;
};

#endif // REGULATOR_HPP
