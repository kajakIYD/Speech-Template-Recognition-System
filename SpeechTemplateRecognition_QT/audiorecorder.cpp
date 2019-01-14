#include "audiorecorder.h"
#include "audiolevel.h"
#include "Cepstrum.hpp"
#include "p.hpp"
#include "pi.hpp"
#include "annresultparser.hpp"

#include <boost/lexical_cast.hpp>
#include "ui_audiorecorder.h"

#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioProbe>
#include <QAudioRecorder>
#include <QDir>
#include <QFileDialog>
#include <QMediaRecorder>
#include <QStandardPaths>

//#include "C:\Users\PF\AppData\Local\Programs\Python\Python35\include\Python.h"

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

AudioRecorder::AudioRecorder()
    : ui(new Ui::AudioRecorder)
{
    currentOutput = 0.0;

    ui->setupUi(this);

    m_audioRecorder = new QAudioRecorder(this);
    m_probe = new QAudioProbe(this);
    connect(m_probe, &QAudioProbe::audioBufferProbed,
            this, &AudioRecorder::processBuffer);
    m_probe->setSource(m_audioRecorder);

    //audio devices
    ui->audioDeviceBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &device: m_audioRecorder->audioInputs()) {
        ui->audioDeviceBox->addItem(device, QVariant(device));
    }

    //audio codecs
    ui->audioCodecBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &codecName: m_audioRecorder->supportedAudioCodecs()) {
        ui->audioCodecBox->addItem(codecName, QVariant(codecName));
    }

    //containers
    ui->containerBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &containerName: m_audioRecorder->supportedContainers()) {
        ui->containerBox->addItem(containerName, QVariant(containerName));
    }

    //sample rate
    ui->sampleRateBox->addItem(tr("Default"), QVariant(0));
    for (int sampleRate: m_audioRecorder->supportedAudioSampleRates()) {
        ui->sampleRateBox->addItem(QString::number(sampleRate), QVariant(
                sampleRate));
    }

    //channels
    ui->channelsBox->addItem(tr("Default"), QVariant(-1));
    ui->channelsBox->addItem(QStringLiteral("1"), QVariant(1));
    ui->channelsBox->addItem(QStringLiteral("2"), QVariant(2));
    ui->channelsBox->addItem(QStringLiteral("4"), QVariant(4));

    //quality
    ui->qualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
    ui->qualitySlider->setValue(int(QMultimedia::NormalQuality));

    //bitrates:
    ui->bitrateBox->addItem(tr("Default"), QVariant(0));
    ui->bitrateBox->addItem(QStringLiteral("32000"), QVariant(32000));
    ui->bitrateBox->addItem(QStringLiteral("64000"), QVariant(64000));
    ui->bitrateBox->addItem(QStringLiteral("96000"), QVariant(96000));
    ui->bitrateBox->addItem(QStringLiteral("128000"), QVariant(128000));

    connect(m_audioRecorder, &QAudioRecorder::durationChanged, this, &AudioRecorder::updateProgress);
    connect(m_audioRecorder, &QAudioRecorder::statusChanged, this, &AudioRecorder::updateStatus);
    connect(m_audioRecorder, &QAudioRecorder::stateChanged, this, &AudioRecorder::onStateChanged);
    connect(m_audioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this,
            &AudioRecorder::displayErrorMessage);


    //simulation
    this->ui->Plot1->addGraph();
    this->ui->Plot1->graph(0)->setPen(QPen(Qt::red)); // line color red for first graph
    this->ui->Plot1->xAxis->setLabel("t");
    this->ui->Plot1->yAxis->setLabel("y");
    this->ui->Plot1->yAxis->scaleRange(0,10);
    this->ui->Plot1->xAxis->setRange(0,50);
    this->ui->Plot1->replot();

    SP = (double)this->ui->spinBoxSP->value();

    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateView()));
    this->timer->start(1000);

    s_Regulator = new P(1 + double(ui->spinBoxGain->value() / 20));

}

double AudioRecorder::SimulateInertia(double control)
{
    const int val = 10;
    double T =  10;
    for (int i = 0; i < val; ++i)
        currentOutput += (control - 1 / T * currentOutput) * 1/val;
    return currentOutput;
}

void AudioRecorder::updatePlot()
{
    s_YVec.append(currentOutput);
    s_TimeVec.append(time);
    this->ui->Plot1->graph(0)->setData(this->s_TimeVec,this->s_YVec);
    if (s_TimeVec.size()>50)
    {
        this->ui->Plot1->xAxis->setRange(s_TimeVec.size()-50, s_TimeVec.size());
    }
    this->ui->Plot1->yAxis->rescale();
    this->ui->Plot1->replot();
}

void AudioRecorder::updateView()
{
    //s_p->time=time;
    SP = (double)this->ui->spinBoxSP->value();
    if (restarted)
    {
        restarted = false;
        ui->Plot1->yAxis->setRange(-10, 10);
        ui->Plot1->replot();
    }

    if (!step && start)
    {
        if (this->time == 0)
        {
            s_YVec.append(currentOutput);
            s_TimeVec.append(time);
            this->ui->Plot1->graph(0)->setData(this->s_TimeVec,this->s_YVec);
            if (s_TimeVec.size()>50)
            {
                this->ui->Plot1->xAxis->setRange(s_TimeVec.size()-50, s_TimeVec.size());
            }

            this->ui->Plot1->replot();
        }
        if (time>0)
        {
            updatePlot();
         //   PlotSP();
        }
        this->time++;

        currentControl = s_Regulator->SimulateStep(SP - currentOutput);
        currentOutput = SimulateInertia(currentControl);
    }
}

void AudioRecorder::updateViewStep()
{
    //s_p->time=time;

    if (restarted)
    {
        restarted = false;
        time = 0;
//        ui->Plot1->yAxis->setRange(-10, 10);
//        ui->Plot1->replot();
    }
    else
    {
        updatePlot();
    }
     //PlotSP();
    this->time++;
}

void AudioRecorder::PlotSP()
{
    this->s_TimeVec.push_back(time);
    this->s_SPVec.push_back(SP);
//    this->ui->Plot1->graph(1)->setData(this->s_TimeVec,this->s_SPVec);
//    this->ui->Plot1->yAxis->rescale();

    if (s_TimeVec.size()>50)
    {
//        this->ui->Plot1->xAxis->setRange(s_TimeVec.size()-50, s_TimeVec.size());
    }

//    this->ui->Plot1->replot();
    return;
}


void AudioRecorder::on_startButton_clicked(bool checked)
{
    start = true;
    step = false;
    regulatorText = ui->regulatorComboBox->currentText();
}

void AudioRecorder::on_stepButton_clicked(bool checked)
{
    start = true;
    step = true;
    updateViewStep();
}

void AudioRecorder::on_stopButton_clicked(bool checked)
{
    start = false;
    step = false;
    currentOutput = 0;
    //time = 0;
}


void AudioRecorder::on_horizontalSlider_valueChanged(int value)
{
    this->timer->start(value);
}


void AudioRecorder::updateProgress(qint64 duration)
{
    if (m_audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    ui->statusbar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecorder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(m_audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        statusMessage = tr("Stopped");
    default:
        break;
    }

    if (m_audioRecorder->error() == QMediaRecorder::NoError)
        ui->statusbar->showMessage(statusMessage);
}

void AudioRecorder::onStateChanged(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::RecordingState:
        ui->recordButton->setText(tr("Stop"));
        ui->pauseButton->setText(tr("Pause"));
        break;
    case QMediaRecorder::PausedState:
        ui->recordButton->setText(tr("Stop"));
        ui->pauseButton->setText(tr("Resume"));
        break;
    case QMediaRecorder::StoppedState:
        ui->recordButton->setText(tr("Record"));
        ui->pauseButton->setText(tr("Pause"));
        break;
    }

    ui->pauseButton->setEnabled(m_audioRecorder->state() != QMediaRecorder::StoppedState);
}

static QVariant boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void AudioRecorder::toggleRecord()
{
    if (m_audioRecorder->state() == QMediaRecorder::StoppedState && m_outputLocationSet) {
        m_audioRecorder->setAudioInput(boxValue(ui->audioDeviceBox).toString());

        QAudioEncoderSettings settings;
        settings.setCodec(boxValue(ui->audioCodecBox).toString());
        settings.setSampleRate(boxValue(ui->sampleRateBox).toInt());
        settings.setBitRate(boxValue(ui->bitrateBox).toInt());
        settings.setChannelCount(boxValue(ui->channelsBox).toInt());
        settings.setQuality(QMultimedia::EncodingQuality(ui->qualitySlider->value()));
        settings.setEncodingMode(ui->constantQualityRadioButton->isChecked() ?
                                 QMultimedia::ConstantQualityEncoding :
                                 QMultimedia::ConstantBitRateEncoding);

        QString container = boxValue(ui->containerBox).toString();

        m_audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
        m_audioRecorder->record();
    }
    else if (!m_outputLocationSet)
    {
        qDebug() << "Set output location first!";
    }
    else {
        m_audioRecorder->stop();
        m_recordMade = true;
    }
}

void AudioRecorder::togglePause()
{
    if (m_audioRecorder->state() != QMediaRecorder::PausedState)
        m_audioRecorder->pause();
    else
        m_audioRecorder->record();
}

void AudioRecorder::setOutputLocation()
{
#ifdef Q_OS_WINRT
    // UWP does not allow to store outside the sandbox
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!QDir().mkpath(cacheDir)) {
        qWarning() << "Failed to create cache directory";
        return;
    }
    QString fileName = cacheDir + QLatin1String("/output.wav");
#else
    QString fileName = QFileDialog::getSaveFileName();
#endif
    outputLocation = QUrl::fromLocalFile(fileName);
    pathToRecordToAnalyze = outputLocation;
    m_audioRecorder->setOutputLocation(outputLocation);
    ui->labelChosenFile->setText("Chosen file: " + outputLocation.toString());
    m_outputLocationSet = true;
}

void AudioRecorder::displayErrorMessage()
{
    ui->statusbar->showMessage(m_audioRecorder->errorString());
}

void AudioRecorder::clearAudioLevels()
{
    for (int i = 0; i < m_audioLevels.size(); ++i)
        m_audioLevels.at(i)->setLevel(0);
}

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

// returns the audio level for each channel
QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

template <class T>
QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

void AudioRecorder::processBuffer(const QAudioBuffer& buffer)
{
    if (m_audioLevels.count() != buffer.format().channelCount()) {
        qDeleteAll(m_audioLevels);
        m_audioLevels.clear();
        for (int i = 0; i < buffer.format().channelCount(); ++i) {
            AudioLevel *level = new AudioLevel(ui->centralwidget);
            m_audioLevels.append(level);
            ui->levelsLayout1->addWidget(level);
        }
    }

    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i)
        m_audioLevels.at(i)->setLevel(levels.at(i));
}

void AudioRecorder::on_playButton_clicked()
{
    if (m_outputLocationSet && m_recordMade || (m_analyzeLocationSet))
    {
        QMediaPlayer* player = new QMediaPlayer;
        // ...
        player->setMedia(outputLocation);
        player->setVolume(50);
        player->play();
    }
    else
    {
        qDebug() << "Set output location and record something firstly!";
    }
}

void AudioRecorder::on_spinBoxGain_valueChanged(int gain)
{
    s_Regulator->SetGain(1 + (double)gain / 10);
}


void AudioRecorder::on_locateRecordToAnalyzeButton_clicked()
{
#ifdef Q_OS_WINRT
    // UWP does not allow to store outside the sandbox
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!QDir().mkpath(cacheDir)) {
        qWarning() << "Failed to create cache directory";
        return;
    }
    QString fileName = cacheDir + QLatin1String("/output.wav");
#else
    QString fileName = QFileDialog::getOpenFileName();
#endif
    pathToRecordToAnalyze = QUrl::fromLocalFile(fileName);
    outputLocation = QUrl::fromLocalFile(fileName);
    ui->labelChosenFile->setText("Chosen file: " + outputLocation.toString());
    m_outputLocationSet = true;
    m_analyzeLocationSet = true;

}

void AudioRecorder::on_calculateCepstrumButton_clicked()
{
    if (m_analyzeLocationSet) //(m_outputLocationSet && m_recordMade)
    {
        WAVFileLoader wavFileLoader;
        wavFileLoader.setOutputLocation(pathToRecordToAnalyze);
        wavFileLoader.loadFile();

        Cepstrum cepstrum;
        std::tuple<std::vector<std::vector<double>>, int> cepDetected =
                cepstrum.CalculateCepstrum(wavFileLoader.signal);        

//        std::cout << "Get<0>: " << std::endl;
//        std::vector<CArray> arr = std::get<0>(cepDetected);
//        for (auto &it : arr)
//        {
//            for(int i = 0; i < it.size(); ++i)
//            {
//                std::cout << it[i] << std::endl;
//            }
//        }
//        std::cout << "Get<1>: " << std::endl << std::get<1>(cepDetected) << std::endl;

        ui->detectedNumberLabel->setText(QString::number(cepstrum.detectedNumber));

        std::cout << cepstrum.detectedNumber << std::endl;

        QString detectedWord;

        switch (cepstrum.detectedNumber)
        {
            case 0:
                detectedWord = "zero";
                break;
            case 1:
                detectedWord = "one";
                break;
            case 2:
                detectedWord = "two";
                break;
            case 3:
                detectedWord = "three";
                break;
            case 4:
                detectedWord = "four";
            case 6:
                detectedWord = "five";
            case 9:
                detectedWord = "left";
            case 10:
                detectedWord = "right";
                break;
            case 11:
                detectedWord = "start";
                break;
            case 12:
                detectedWord = "stop";
                break;
            default:
                break;
        }

        reactOnDetectedWord(detectedWord.toStdString());

        ui->detectedNumberLabel->setText(detectedWord);

        m_analyzeLocationSet = false;
    }
    else
    {
        std::cout << "Set output location and record something firstly!";
    }
}

void AudioRecorder::on_regulatorComboBox_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "P")
    {
        delete s_Regulator;
        s_Regulator = new P(1 + ui->spinBoxGain->value() / 10);
    }
    else
    {
        delete s_Regulator;
        s_Regulator = new PI(1 + ui->spinBoxGain->value() / 10, 10);
    }
}

void AudioRecorder::on_runNeuralNetworkButton_clicked()
{
    if (m_analyzeLocationSet)
    {
        std::string command = pathToRecordToAnalyze.path().toStdString();
        command = command.substr(1, command.length()-1);
        std::string graphPath = "C:/tmp/my_frozen_graph.pb";
        std::string labelsPath = "C:/tmp/speech_commands_train/conv_labels.txt";
        command = "python label_wav.py --graph=" + graphPath + " --labels=" + labelsPath + " --wav=" + command;
        std::system(command.c_str());
        std::vector<std::tuple<std::string, double>> annResultVec = annresultparser::parseANNResult();
        std::tuple<std::string, double> annResult = annresultparser::pickAnnResult(annResultVec);
        reactOnDetectedWord(std::get<0>(annResult));
        std::string numRes = boost::lexical_cast<std::string>(std::get<1>(annResult));
        numRes = numRes.substr(0, 3);
        QString detectedWord = QString::fromStdString(std::get<0>(annResult) + " " + numRes);
        ui->detectedNumberLabel->setText(detectedWord);
    }
    else
    {
        std::cout << "Set file to analyze location firstly!" << std::endl;
    }
}

void AudioRecorder::reactOnDetectedWord(std::string detectedWord)
{
    if (detectedWord == "go")
    {
        change = go;
        changeSimulationParameters(change);
    }
    else if (detectedWord == "start")
    {
        change = go;
        changeSimulationParameters(change);
    }
    else if (detectedWord == "stop")
    {
        change = stop;
        changeSimulationParameters(change);
    }
    else if (detectedWord == "zero")
    {
        currentSelection = zero;
    }
    else if (detectedWord == "one")
    {
        currentSelection = one;
    }
    else if (detectedWord == "two")
    {
        currentSelection = two;
    }
    else if (detectedWord == "three")
    {
        currentSelection = three;
    }
    else if (detectedWord == "four")
    {
        currentSelection = four;
    }
    else if (detectedWord == "five")
    {
        currentSelection = five;
    }
    else if (detectedWord == "up")
    {
        currentSelection++;
        currentSelection %= 5;
    }
    else if (detectedWord == "down")
    {
        currentSelection--;
        if (currentSelection < 0)
        {
            currentSelection = 4;
        }
        currentSelection %= 5;
    }
    else if (detectedWord == "left")
    {
        currentSelection = zero;
    }
    else if (detectedWord == "right")
    {
        currentSelection = zero;
    }
    else
    {
        ui->labelCurrentSelection->setText("Current selection: " + str + " ||unable to recognize speech correctly||");
        return;
    }

    switch (currentSelection) {
    case 0:
        str = "0 (Start)";
        break;
    case 1:
        str = "1 (Stop)";
        break;
    case 2:
        str = "2 (SP)";
        break;
    case 3:
        str = "3 (Gain)";
        break;
    case 4:
        str = "4 (Regulator)";
        break;
    default:
        str = "Unknown";
        break;
    }
    ui->labelCurrentSelection->setText("Current selection: " + str);
}


void AudioRecorder::changeSimulationParameters(GoStop change)
{
    int valueChange;
    valueChange = (change == go) ? 1 : 0;
    switch(currentSelection) {
    case zero:
        if (valueChange == 1)
            on_startButton_clicked(true);
        break;
    case one:
        if (valueChange == 1)
            on_stopButton_clicked(true);
        break;
    case two:
        //Change SP
        ui->spinBoxSP->setValue(ui->spinBoxSP->value() + valueChange);
        break;
    case three:
        //Change gain
        ui->spinBoxGain->setValue(ui->spinBoxGain->value() + valueChange);
        break;
    case four:
        //Change regulator
        ui->regulatorComboBox->setCurrentIndex(valueChange);
        break;
    default:
        break;
    }
}
