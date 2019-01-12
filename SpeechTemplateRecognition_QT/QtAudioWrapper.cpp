#include <QUrl>
#include <QFileInfo>

#include "QtAudioWrapper.hpp"

#include <iostream>

QTAudioWrapper::QTAudioWrapper()
{
    player = new QMediaPlayer;
    audioRecorder = new QAudioRecorder();
}

QTAudioWrapper::~QTAudioWrapper()
{
    if (player)
    {
        delete player;
    }
    if (audioRecorder)
    {
        delete audioRecorder;
    }
}

int QTAudioWrapper::PlayAudio(QString path)
{
    // ...

    // check if path exists and if yes: Is it a file and no directory?
    bool fileExists = QFileInfo::exists(path) && QFileInfo(path).isFile();
    qDebug() << QUrl::fromLocalFile(path);
    try
    {
        player->setMedia(QUrl::fromLocalFile(path));
        player->setVolume(50);
        player->play();
    }
    catch (...)
    {
        return -1;
    }
    return 0;
}


int QTAudioWrapper::RecordAudio(QString fileName)
{
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/amr");
    audioSettings.setQuality(QMultimedia::HighQuality);

    audioRecorder->setEncodingSettings(audioSettings);

    audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName + ".amr"));
    audioRecorder->record();
    return 0;
}
