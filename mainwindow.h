#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <music.h>
#include <string>
#include <QListWidgetItem>
#include <queue.h>
#include <QDir>
#include <QCheckBox>
#include <playlistprompt.h>
#include <windows.h>
#include <QSplitter>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace std;
class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    QIcon *playbutton;
    QIcon *pausebutton;
    QIcon *backButton;
    QIcon *forwButton;
    QIcon *volumeHigh;
    QIcon *volumeLow;
    QIcon *volumeMute;
    MainWindow(QWidget *parent = nullptr);
    QString PLAYLIST = "Playlist";
    int playlistCount = 1;

    ~MainWindow();
    void PlayMusic();
    void PauseMusic();
    void getImage(const char *inputFilePath, const char *outputImagePath);
    void retrieve_album_art(const char *path, const char *album_art_file);
    void setImage(QString imageDirectorystr,QString imagePath);
    void writeToPlaylist(QString playlistPath,QJsonArray selectedSongs);
    void WriteToFile2(QString filepath,QString path);
    void ReadFromFile(QString filepath,QVector<QString> vector);
    void LoadMusic(QDir directory);
    void getMusicLength();
    void setApplicationVolume(unsigned int volume);
    void hideVolumeSlider();
    void revealVolumeSlider();
private slots:
    void on_pushButton_6_clicked();
    void startStopwatch();
    void updateStopwatch();
    void stopStopwatch();
    void pauseStopwatch();
    void resetTimer();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void onFolderOptionCLicked();
    void on_forwButton_clicked();
    void on_backButton_clicked();
    void on_textEdit_textChanged();
    void on_shuffle_button_clicked();
    void on_repeatButton_clicked();
    void on_playlist_itemClicked(QListWidgetItem *item);
    void on_create_new_playlist_clicked();
    void on_seekbar_slider_sliderMoved(int position);
    void on_seekbar_slider_sliderReleased();
    void on_addToPlaylistButton_clicked();
    void on_refreshButton_clicked();
    void on_listWidget_itemPressed(QListWidgetItem *item);
    void on_pushButton_clicked();
    void on_volumesilder_sliderMoved(int position);

    void on_volumesilder_sliderReleased();

    void on_volume_button_clicked();

    void on_playlist_dropdown_menu_currentTextChanged(const QString &arg1);

public slots:
    void setPlaylistName(QString playlist_name);

signals:
    void startOperation(const char* filepath);
    void PauseOperation();
    void ResumeOperation(const char* filepath);
    void reset_Music();
    void seekTo(const char* filepath,int seekToSecond);
    void ResumeFromVal(const char* filepath,int currentDuration);
    void setVolume(int,const char*);

private:
    QSplitter *splitter;
    Ui::MainWindow *ui;
    Music threadObj;
    QThread qthread;
    QThread playListThread;
    Queue queue;
    QString selectedDirectory="D:/Music";
    PlayListPrompt playListForm;
    QTimer *stopwatchTimer; // QTimer to update the stopwatch
    int elapsedTime;
    int64_t duration;
    const char* filepath;
    QString playlist_name;
    QMutex mutex;
    QWaitCondition condition;
    bool formClosed;
    void createPlaylist(QString playlist_name);
    bool checkBox=false;
    bool isSliderHidden=true;

};

#endif // MAINWINDOW_H
