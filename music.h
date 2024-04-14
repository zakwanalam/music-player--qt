#ifndef MUSIC_H
#define MUSIC_H
#include <QThread>
#include <QDebug>
#include <portaudio.h>
#include <mpg123.h>
#include <QMutex>
#include <QRunnable>
#include <QFuture>
#include <QWaitCondition>

#include <portaudio.h>

class Music: public QObject
{
    Q_OBJECT
public:
    Music();
    mpg123_handle*handle;
    const char*filepath;
    size_t* buffer;
    size_t buffer_size;
    size_t done;
    PaStream *pstream;
    int channels, encoding;
    long rate;
    double musicVolume=1;
    bool isPaused=false;
    off_t pausedPosition=0;
    QFuture<void> qfuture;
    bool hasPlayed = false;
    bool songEnded=false;
private:
    QMutex mutex;
    QWaitCondition condition;
    bool stopRequested;

public slots:
    void audioPlayBack(mpg123_handle *handle,size_t* buffer,size_t buffer_size,size_t done,PaStream *pstream);
    void PlayMusic(const char* str);
    void PauseMusic();
    void ResumeMusic(const char*filepath);
    void resetPosition();
    void updateDuration(const char* filepath,int currentDuration);
    void setVolume(int volume,const char* filepath);
};

#endif // MUSIC_H
