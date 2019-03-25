# Speech-Template-Recognition-System
Multiplatform project, which main goal is to detect speech templates like "one", "two", "start", "stop"

## Algorithms and technologies
Matching speech templates is implemented in both python and C++. On the **Python** side multilayer artificial neural network does the trick. It is implemented with the usage of **Keras** and **Tensorflow**. On the other side (C++ side :) all the magic is the algorithm that computes cepstral coefficients (described in Cyfrowe Przetwarzanie Sygnałów, Tomasz P. Zieliński, WKŁ Warszawa 2007).

## Multiplatform
Whole project has also its **Raspberry Pi 3B+** version. Cross compilation allows to make the code easily portable.


## Brief insight
Front panel of the application looks as shown below:
[!Cepstrum_NN_Tab](https://github.com/kajakIYD/Speech-Template-Recognition-System/blob/master/DocumentationImages/Cepstrum_NN_Tab.png)
[!Play_Record_Tab](https://github.com/kajakIYD/Speech-Template-Recognition-System/blob/master/DocumentationImages/Play_Record_Tab.png)
<br>
Learning process looks like shown below:
<br>
[!Learning_Process](https://github.com/kajakIYD/Speech-Template-Recognition-System/blob/master/DocumentationImages/Learning_Process.png)

## Additional information
https://github.com/kajakIYD/Speech-Template-Recognition-System/blob/master/README.pdf
