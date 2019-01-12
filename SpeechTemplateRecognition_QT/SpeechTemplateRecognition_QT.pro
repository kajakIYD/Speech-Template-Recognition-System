QT += gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport


CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += E:\boost_1_67_0

SOURCES += \
    Cepstrum.cpp \
    Fft.cpp \
    SpeechTemplateReconition.cpp \
    QtAudioWrapper.cpp \
    audiolevel.cpp \
    audiorecorder.cpp \
    main.cpp \
    qcustomplot.cpp \
    p.cpp \
    regulator.cpp \
    wavfileloader.cpp \
    fileloader.cpp \
    AudioFile.cpp \
    pi.cpp \
    annresultparser.cpp

HEADERS += \
    Cepstrum.hpp \
    Fft.h \
    MatrixOperations.hpp \
    QtAudioWrapper.hpp \
    audiolevel.h \
    audiorecorder.h \
    qcustomplot.h \
    regulator.hpp \
    p.hpp \
    wavfileloader.hpp \
    fileloader.hpp \
    AudioFile.h \
    pi.hpp \
    annresultparser.hpp

FORMS += \
    audiorecorder.ui
