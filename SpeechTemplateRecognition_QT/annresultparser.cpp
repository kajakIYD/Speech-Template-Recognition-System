#include "annresultparser.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>


std::tuple<std::string, double> annresultparser::parseANNResult()
{
    std::string pathToResultFile = "E:\\Qt\\Projects\\SpeechTemplateRecognition_QT\\build-SpeechTemplateRecognition_QT-Desktop_Qt_5_10_1_MSVC2015_64bit-Debug\\results.txt";
    std::ifstream myfile (pathToResultFile.c_str());
      if (myfile.is_open())
      {
        std::string line;
        getline (myfile,line);
        myfile.close();
        std::string word = line.substr(0, line.find(";"));
        int pos = line.find(";");
        line.erase(0, pos + 1);
        std::string probabilityStr = line.substr(0, 4);
        double probability = std::atof(probabilityStr.c_str());
        return std::make_tuple(word, probability);
      }
      else std::cout << "Unable to open file";
      return std::make_tuple("null", 0.0);
}
