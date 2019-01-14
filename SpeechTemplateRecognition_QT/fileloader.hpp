#ifndef FILELOADER_HPP
#define FILELOADER_HPP

#include <vector>
#include <tuple>



class FileLoader
{
public:
    FileLoader();

    std::vector<std::tuple<std::vector<std::vector<double>>, int>> loadPatterns();

    std::vector<std::tuple<std::vector<std::vector<double>>, int>> patternsLoaded;
};

#endif // FILELOADER_HPP
