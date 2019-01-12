#include "fileloader.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

FileLoader::FileLoader()
{

}


std::vector<std::tuple<std::vector<std::vector<double>>, int>> FileLoader::loadPatterns()
{
    std::vector<std::tuple<std::string, int>> patterns =
    {
        //std::make_tuple("zero", 0),
        //std::make_tuple("one", 1),
//        std::make_tuple("two", 2),
        //std::make_tuple("three", 3),
//        std::make_tuple("start", 11)
        std::make_tuple("ZeroMowa", 0),
        std::make_tuple("OneMowa", 1),
        std::make_tuple("TwoMowa", 2)
    };

    std::vector<std::tuple<std::vector<std::vector<double>>, int>> result;

    for (int i = 0; i < patterns.size(); ++i)
    {
        std::vector<std::vector<double>> cepstrumCoefficients;

        std::string patternName = std::get<0>(patterns[i]);
        int patternNumber = std::get<1>(patterns[i]);

        std::ifstream infile(patternName + ".txt");

        if (infile.is_open())
        {
            std::string line;
            while (std::getline(infile, line))
            {
                if (line.find("*") != std::string::npos)
                {
                    result.push_back(std::make_tuple(cepstrumCoefficients, patternNumber));
                    continue;
                }

                boost::algorithm::replace_all(line, "\t\t\t\t\t", ";");

                std::stringstream ss(line);
                std::string item;
                std::vector<double> doubleVec;
                while (std::getline(ss, item, ';'))
                {
                   doubleVec.push_back(boost::lexical_cast<double>(item));
                }
                cepstrumCoefficients.push_back(doubleVec);
            }
        }
        else
        {
            std::cout << "Cannot read file: " << patternName << ".txt" << std::endl;
        }

    }
    return result;
}
