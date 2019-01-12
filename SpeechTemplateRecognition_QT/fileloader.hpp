#ifndef FILELOADER_HPP
#define FILELOADER_HPP

#include <vector>
#include <tuple>



class FileLoader
{
public:
    FileLoader();

    std::vector<std::tuple<std::vector<std::vector<double>>, int>> loadPatterns();
};

#endif // FILELOADER_HPP
