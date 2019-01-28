# Speech-Template-Recognition-System
Multiplatform project, which main goal is to detect speech templates like "one", "two", "start", "stop"

## Algorithms and technologies
Matching speech templates is implemented in both python and C++. On the python side multilayer artificial neural network does the trick. It is implemented with the usage of Keras and Tensorflow. On the other side (C++ side :) all the magic is the algorithm that computes cepstral coefficients (described in Cyfrowe Przetwarzanie Sygnałów, Tomasz P. Zieliński, WKŁ Warszawa 2007).

## Multiplatform
Whole project has also its Raspberry Pi 3B+ version. Cross compilation allows to make the code easily (huh, I must admit that configuration of cross compilation was not so easy:D) portable.
