#ifndef QTPLAYERWRAPPER_HPP
#define QTPLAYERWRAPPER_HPP

#include <QMediaPlayer>
#include <QAudioRecorder>


#include <QString>

class QTAudioWrapper
{
public:
    QTAudioWrapper();
    ~QTAudioWrapper();
    int PlayAudio(QString = "test.amr");

    int RecordAudio(QString fileName);

private:
    QMediaPlayer* player;
    QAudioRecorder* audioRecorder;
};

#endif // QTPLAYERWRAPPER_HPP
