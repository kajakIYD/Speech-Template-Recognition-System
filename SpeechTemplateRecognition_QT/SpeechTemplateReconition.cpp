#include <QCoreApplication>

#include <iostream>
#include <string>
#include <vector>

#include <fstream>

#include "Cepstrum.hpp"
#include "QtAudioWrapper.hpp"

#include "audiorecorder.h"

#include <QtWidgets>

//int Act()
//{
//    //Load some samples (for now samples are preprocessed - silent at the
//    //end and at the beginning of the record is removed)
//    //And learn the "base" - cepstrum coefficients
//    //remember to label each example ("one", "two", "three")

//    QTAudioWrapper audioWrapper;
//    audioWrapper.RecordAudio("test2");
//    audioWrapper.PlayAudio("test2");

//    auto record = 0;
//    int fpr = 8000;
//    int Np = 10;
//    int Nc = 12;
//    int twind = 30;
//    int tstep = 10;
//    int Mlen = (twind * 0.001) * fpr;
//    int Mstep = (tstep * 0.001) * fpr;

//    std::string words [3] = {"sinus0", "sinus1", "sinus2" };
//    int M = sizeof(words) / sizeof(words[0]);

//    std::vector<double> signal;

//    std::vector<CArray> Cwz;
//    std::vector<std::vector<CArray>> Cwzr;
//    std::vector<int> Nwzr;
//    int Nframes;

//    //Detect features
//    for (int k = 0; k < M; ++k)
//    {
//        std::tuple<std::vector<CArray>, int> cepRes =
//        Cepstrum::calculateCxMatrix(signal, Mlen, Mstep, Nc, Np);

//        signal.clear();

//        Cwz = std::get<0>(cepRes);
//        Nframes = std::get<1>(cepRes);

//        int Nw = Cwz.size();
//        int Nk = Cwz[0].size();
//        Cwzr.push_back(Cwz);
//        Nwzr.push_back(Nframes);
//    }

//    std::tuple<std::vector<CArray>, int> cepDetected =
//    Cepstrum::calculateCxMatrix(signal, Mlen, Mstep, Nc, Np);

//    int nr = Cepstrum::dtw(std::get<0>(cepDetected), Cwzr, Nwzr);
//    std::cout << "Should be " << "xxxx" << " is: "<< nr << std::endl;

//    return 0;
//}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Cepstrum::loadPatterns(); //Load speech patterns at the very beginning of the program

    AudioRecorder recorder;
    recorder.show();

    return app.exec();
}
