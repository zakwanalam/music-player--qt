#include "music.h"

#include "mpg123.h"
// #include <out123.h>
#include "portaudio.h"
#include "iostream"
#include <QFuture>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QtConcurrent/QtConcurrent>
using namespace std;

Music::Music()
{
}
//Playing the audio buffer in background
void Music::audioPlayBack(mpg123_handle *handle, size_t *buffer, size_t buffer_size, size_t done, PaStream *pstream) {
    qfuture = QtConcurrent::run([&] {

        while (mpg123_read(handle, buffer, buffer_size, &done) == MPG123_OK && done > 0) {
            pausedPosition = mpg123_tell(handle);
            qDebug()<<pausedPosition;
            if (isPaused == true) {
                break;
            }
            Pa_WriteStream(pstream, buffer, done / sizeof(int));
        }

        qDebug() << "Paused position: " << static_cast<int>(pausedPosition);
        qDebug() << "Total length: " << static_cast<int>(mpg123_length(handle));
        // Check for end of the song
        if (pausedPosition <= mpg123_length(handle)) {
            songEnded = true;
            qDebug() << "Song Ended";
        }
        // Clean up
        Pa_StopStream(pstream);
        Pa_CloseStream(pstream);
        Pa_Terminate();
        mpg123_close(handle);
        mpg123_delete(handle);
        mpg123_exit();
    });
}

void Music::PlayMusic(const char * filepath){
    // Initialize the library
    mpg123_init();

    // Open the MP3 file
    mpg123_handle* handle = mpg123_new(NULL, NULL);
    int x = mpg123_open(handle, filepath);
    mpg123_volume(handle,musicVolume);
    //    mpg123_seek(handle,pausedPosition,SEEK_SET);
    if (x != MPG123_OK) {
        cerr << "Music Not Found" << endl;
        return;
    }
    // Get the format information
    int channels, encoding;
    long rate;


    mpg123_getformat(handle, &rate, &channels, &encoding);

    // PaStream *pstream;
    // Read and play the MP3 file
    Pa_Initialize();
    Pa_OpenDefaultStream(&pstream, 0, channels, paInt16, rate, paFramesPerBufferUnspecified, nullptr, nullptr);

    if(isPaused==false){
        mpg123_seek(handle,pausedPosition,SEEK_SET);
    }

    Pa_StartStream(pstream);
    buffer_size = mpg123_outblock(handle);
    buffer = new size_t[buffer_size / sizeof(size_t)];
    audioPlayBack(handle,buffer,buffer_size,done,pstream);
}

void Music::PauseMusic(){
    isPaused=true;
}
void Music::ResumeMusic(const char* filepath2){
    isPaused=false;
    PlayMusic(filepath2);
}
void Music::resetPosition(){
    pausedPosition = 0;
}
void Music::updateDuration(const char* filepath,int currentDuration){
    // mpg123_seek()
    pausedPosition =  46080*currentDuration;

}
void Music::setVolume(int volume,const char*filepath){
    isPaused=true;
    musicVolume = volume/100.0;
    qDebug()<<"Volume is: "<<this->musicVolume;
    PlayMusic(filepath);
}
