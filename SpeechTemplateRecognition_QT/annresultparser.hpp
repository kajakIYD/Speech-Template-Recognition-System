#ifndef ANNRESULTPARSER_HPP
#define ANNRESULTPARSER_HPP

#include <string>
#include <tuple>


class annresultparser
{
public:
    static std::tuple<std::string, double> parseANNResult();
};

#endif // ANNRESULTPARSER_HPP
