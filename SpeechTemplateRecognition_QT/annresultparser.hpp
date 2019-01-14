#ifndef ANNRESULTPARSER_HPP
#define ANNRESULTPARSER_HPP

#include <string>
#include <tuple>
#include <vector>

class annresultparser
{
public:
    static std::vector<std::tuple<std::string, double>> parseANNResult();
    static std::tuple<std::string, double> pickAnnResult(std::vector<std::tuple<std::string, double>> annResultVec);
};

#endif // ANNRESULTPARSER_HPP
