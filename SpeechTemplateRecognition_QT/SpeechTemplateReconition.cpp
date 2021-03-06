#include <QCoreApplication>

#include <iostream>
#include <string>
#include <vector>

#include <fstream>

#include "Cepstrum.hpp"
#include "QtAudioWrapper.hpp"
#include "fileloader.hpp"

#include "audiorecorder.h"

#include <QtWidgets>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Cepstrum::loadPatterns(); //Load speech patterns at the very beginning of the program

    FileLoader fl;
    fl.loadPatterns();
    std::vector<std::vector<std::vector<double>>> Cwzr = Cepstrum::extractCwzrFromPatterns(fl.patternsLoaded);
    Cepstrum::extractNwzrFromCwzr(Cwzr); //Vector of number of rows in Cwz

    AudioRecorder recorder;
    recorder.show();

    return app.exec();
}
