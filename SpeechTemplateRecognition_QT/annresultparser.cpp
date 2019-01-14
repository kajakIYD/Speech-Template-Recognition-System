#include "annresultparser.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>


std::vector<std::tuple<std::string, double>> annresultparser::parseANNResult()
{
    std::vector<std::tuple<std::string, double>> result;
    std::string pathToResultFile = "E:\\Qt\\Projects\\SpeechTemplateRecognition_QT\\build-SpeechTemplateRecognition_QT-Desktop_Qt_5_10_1_MSVC2015_64bit-Debug\\results.txt";
    std::ifstream myfile (pathToResultFile.c_str());
      if (myfile.is_open())
      {
        std::string line;
        while(getline(myfile,line))
        {
            std::string word = line.substr(0, line.find(";"));
            int pos = line.find(";");
            line.erase(0, pos + 1);
            std::string probabilityStr = line.substr(0, 4);
            double probability = std::atof(probabilityStr.c_str());
            probability = probability * 100;
            probability = std::round(probability);
            probability = probability / 100;
            result.push_back(std::make_tuple(word, probability));
        }
        myfile.close();
        return result;
      }
      else std::cout << "Unable to open file";

      result.push_back(std::make_tuple("null", 0.0));
      return result;
}


std::tuple<std::string, double> annresultparser::pickAnnResult(std::vector<std::tuple<std::string, double>> annResultVec)
{
    for(int i = 0; i < annResultVec.size(); ++i)
    {
        auto item = annResultVec[i];
        std::string name = std::get<0>(item);
        if (name == "go")
        {
            return item;
        }
        if (name == "stop")
        {
            return item;
        }
        if (name == "up")
        {
            return item;
        }
        if (name == "down")
        {
            return item;
        }
    }
    return std::make_tuple("null", 0.0);
}
