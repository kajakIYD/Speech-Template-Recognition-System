/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QMainWindow>
#include <QMediaRecorder>
#include <QMediaPlayer>
#include <QTimer>
#include <QUrl>
#include <QVector>

#include "regulator.hpp"
#include "wavfileloader.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class AudioRecorder; }
class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;
QT_END_NAMESPACE

class AudioLevel;

class AudioRecorder : public QMainWindow
{
    Q_OBJECT

public:
    AudioRecorder();

public slots:
    void processBuffer(const QAudioBuffer&);

private slots:
    void setOutputLocation();
    void togglePause();
    void toggleRecord();

    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();

    void on_playButton_clicked();

    void updatePlot();
    void updateView();
    void updateViewStep();
    void PlotSP();

    void on_startButton_clicked(bool checked);
    void on_stepButton_clicked(bool checked);
    void on_stopButton_clicked(bool checked);

    void on_horizontalSlider_valueChanged(int value);

    void on_spinBoxGain_valueChanged(int arg1);

    void on_locateRecordToAnalyzeButton_clicked();

    void on_calculateCepstrumButton_clicked();


    void on_regulatorComboBox_currentIndexChanged(const QString &arg1);

    void on_runNeuralNetworkButton_clicked();

private:
    void clearAudioLevels();

    double SimulateInertia(double control);

    enum UpDown {up, down};
    UpDown changeUpDown;

    enum GoStop {go, stop};
    GoStop change;

    enum SelectMode {zero, one, two, three, four, five};
    int currentSelection = zero;

    void reactOnDetectedWord(std::string detectedWord);

    void changeSimulationParameters(GoStop change);


    Ui::AudioRecorder *ui = nullptr;

    QAudioRecorder *m_audioRecorder = nullptr;
    QAudioProbe *m_probe = nullptr;
    QList<AudioLevel*> m_audioLevels;
    QUrl outputLocation;
    QUrl pathToRecordToAnalyze;

    QString regulatorText;
    QString str;


    QTimer* timer;

    QVector<double> s_SPVec = {};
    QVector<double> s_YVec = {};
    QVector<double> s_TimeVec = {};

    Regulator* s_Regulator;

    double SP;
    double currentOutput;
    double currentControl;

    int time = 0;

    bool m_outputLocationSet = false;
    bool m_recordMade = false;
    bool m_analyzeLocationSet = false;

    bool start=false;
    bool step = false;
    bool restarted = false;
};

#endif // AUDIORECORDER_H
