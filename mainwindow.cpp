#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QThread>
#include <QString>
#include <QDir>
#include <algorithm>
#include <queue.h>
#include <QDebug>
#include <QPropertyAnimation>
#include <QFont>
#include <QFontDatabase>
#include <QVector>
#include <QFileDialog>
#include <QJsonArray>
#include <QCheckBox>
#include <QFile>
#include <music.h>
#include <QProcess>
#include <unistd.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QMediaPlayer>
extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#include <QGraphicsPixmapItem>
using namespace std;

#include <cstdint> // For std::uint8_t


QString path= "Icons/pause.png";
bool isplaying = false;
QString str = "\0";
QString fileName;
QVector <QString> songPaths;
QVector<QString> songNames;
int currIndex=0;
int listItemClicked=0;

int currentDuration=0;

QString favsongsfile = "C:/Users/User/Documents/untitled/favsongs.txt";
QVector<QString> playlistPath;
QVector<QString> favsongsname;
QString playlistFile="C:/Users/User/Documents/untitled/playlist.txt";

int volume=100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    threadObj.moveToThread(&qthread);

    playListForm.moveToThread(&playListThread);

    connect(this,&MainWindow::ResumeOperation,&threadObj,&Music::ResumeMusic);
    connect(this,&MainWindow::reset_Music,&threadObj,&Music::resetPosition);
    connect(this,&MainWindow::seekTo,&threadObj,&Music::updateDuration);
    connect(this,&MainWindow::PauseOperation,&threadObj,&Music::PauseMusic);
    connect(this,&MainWindow::startOperation,&threadObj,&Music::PlayMusic);
    connect(this,&MainWindow::setVolume,&threadObj,&Music::setVolume);
    connect(&playListForm, &PlayListPrompt::sendplaylistName, this, &MainWindow::setPlaylistName);

    elapsedTime = 0;
    stopwatchTimer = new QTimer(this);
    connect(stopwatchTimer, &QTimer::timeout, this, &MainWindow::updateStopwatch);

    qthread.start();
    MainWindow::playbutton = new QIcon("Icons/play.png");
    MainWindow::backButton =  new QIcon("Icons/backward.png");
    MainWindow::forwButton =  new QIcon("Icons/forward.png");
    MainWindow::pausebutton = new QIcon("Icons/pause.png");
    volumeHigh = new QIcon ("Icons/volume-high.png");
    volumeLow= new QIcon("Icons/volume-low.png");
    volumeMute= new QIcon("Icons/mute.png");
    QSize qsize;
    QSize qsize2;
    ui->menubar->setStyleSheet(
        "QMenuBar{"
            "background-color: transparent;"
            "border: none;"
        "}"
        );
    ui->pushButton_6->setFlat(true);

    ui->forwButton->setFlat(true);

    ui->backButton->setFlat(true);

    ui->AlbumPlaceHolder->setFlat(true);
    QString noalbumart = "Images/noalbumart.png";
    QIcon albumCover(noalbumart);
    QSize coversize;
    coversize.setHeight(200);
    coversize.setWidth(200);
    ui->AlbumPlaceHolder->setIcon(albumCover);
    ui->AlbumPlaceHolder->setIconSize(coversize);

    qsize.setHeight(70);
    qsize.setWidth(70);

    qsize2.setWidth(30);
    qsize2.setHeight(30);

    ui->pushButton_6->setIcon(*playbutton);
    ui->pushButton_6->setIconSize(qsize);

    ui->backButton->setIcon(*backButton);
    ui->backButton->setIconSize(qsize2);

    ui->forwButton->setIcon(*forwButton);
    ui->forwButton->setIconSize(qsize2);
    ui->repeatButton->setCheckable(true);
    ui->shuffle_button->setCheckable(true);

    QSize size;
    size.setWidth(18);
    size.setHeight(18);
    QIcon shuffleIcon("Icons/shuffle-01.png");

    ui->shuffle_button->setIcon(shuffleIcon);
    ui->shuffle_button->setIconSize(size);
    QIcon repeatButton("Icons/repeat-01.png");

    QIcon refreshIcon("Icons/refresh.png");
    ui->refreshButton->setIcon(refreshIcon);
    ui->repeatButton->setIcon(repeatButton);

    ui->playlist_dropdown_menu->setPlaceholderText("Playlists");
    QMenu *myMenu = new QMenu();
    myMenu->setVisible(true);
    myMenu->setTitle("File");
    myMenu->setStyleSheet("QMenu::title{color: white;}");
    myMenu->show();

    QAction *openfolder = new QAction();
    openfolder->setText("Open Folder");

    ui->volume_button->setIcon(*volumeHigh);
    ui->volumesilder->setRange(0,100);
    ui->volumesilder->setValue(volume);
    myMenu->addAction(openfolder);
    // ui->volumesilder->hide();

    ui->menubar->addMenu(myMenu);
    ui->menubar->setStyleSheet("QMenuBar{"
                               "background-color:transparent;"
                               "color:white};");

    connect(openfolder, &QAction::triggered, this, &MainWindow::onFolderOptionCLicked);
    ui->textEdit->setPlaceholderText("Search Music");
    path= "Icons/play.png";
    QDir dir("D:/Music");
    LoadMusic(dir);

    waveOutSetVolume(NULL, MAKELONG(65535, 65535));
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->volumesilder);
    ui->volumesilder->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);
}
void MainWindow::startStopwatch(){
    if (!stopwatchTimer->isActive()) {
        if (elapsedTime == 0) {
            ui->currentSongLabel->setText("00:00");
        }

        stopwatchTimer->start(1000);
        // Start the timer to update every second
    }
}

void MainWindow::stopStopwatch() {
    if (stopwatchTimer->isActive()) {
        stopwatchTimer->stop();
        ui->currentSongLabel->setText("00:00");
    }
}

void MainWindow::pauseStopwatch() {
    if (stopwatchTimer->isActive()) {
        stopwatchTimer->stop();
    }
}
void MainWindow::updateStopwatch() {
    ui->seekbar_slider->setRange(0,duration);
    ui->seekbar_slider->setValue(elapsedTime);
    if(elapsedTime<duration){
        elapsedTime++; // Increment the elapsed time
    }
    // Format the elapsed time in hours, minutes, and seconds
    if(elapsedTime==duration){
        stopwatchTimer->stop();
        on_forwButton_clicked();
    }
    int hours = elapsedTime / 3600;
    int minutes = (elapsedTime % 3600) / 60;
    int seconds = elapsedTime % 60;

    QString stopwatchText = QString("%1:%2:%3")
                                .arg(hours, 2, 10, QChar('0'))
                                .arg(minutes, 2, 10, QChar('0'))
                                .arg(seconds, 2, 10, QChar('0'));

    ui->currentSongLabel->setText(stopwatchText);
}
void MainWindow::retrieve_album_art(const char *path, const char *album_art_file) {
    int i, ret = 0;

    if (!path) {
        printf("Path is NULL\n");
        return;
    }

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    printf("Opening %s\n", path);

    // open the specified path
    if (avformat_open_input(&pFormatCtx, path, NULL, NULL) != 0) {
        printf("avformat_open_input() failed");
        return;
    }

    // read the format headers
    if (pFormatCtx->iformat->read_header(pFormatCtx) < 0) {
        printf("could not read the format header\n");
        return;
    }
    // find the first attached picture, if available
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
            AVPacket pkt = pFormatCtx->streams[i]->attached_pic;
            FILE* album_art = fopen(album_art_file, "wb");
            ret = fwrite(pkt.data, pkt.size, 1, album_art);
            fclose(album_art);
            break;
        }

    if (ret) {
        printf("Wrote album art to %s\n", album_art_file);
    }

    //fail:
    //    av_free(pFormatCtx);
    // this line crashes for some reason...
    //avformat_free_context(pFormatCtx);
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug()<<"Window Closed";
}

void MainWindow::getMusicLength()
{

    string inputFileName = str.toStdString();
    AVFormatContext *formatContext = avformat_alloc_context();

    if (avformat_open_input(&formatContext, inputFileName.c_str(), nullptr, nullptr) != 0) {
        avformat_free_context(formatContext);
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
        qDebug()<< "Error: Could not find stream information";
    }

    duration = ((formatContext->duration / AV_TIME_BASE))+3;
    int hours = static_cast<int>(duration / 3600);
    int minutes = static_cast<int>((duration % 3600) / 60);
    int seconds = static_cast<int>(duration % 60);

    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);

    string str2 =std::to_string(hours)+":"+std::to_string(minutes) + ":" + std::to_string(seconds);
    QString songLength = QString::fromStdString(str2);
    ui->songLengthLabel->setText(songLength);
}

void MainWindow::onFolderOptionCLicked(){
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);

    dialog.setOption(QFileDialog::ShowDirsOnly);

    dialog.setWindowTitle("Open Folder");

    if (dialog.exec()) {
        selectedDirectory = dialog.selectedFiles().first();

        qDebug() << "Selected Directory: " << selectedDirectory;
    } else {
        qDebug() << "Operation canceled by the user.";
    }
    ui->listWidget->clear();
    QDir dir(selectedDirectory);
    LoadMusic(dir);
}
bool isPlaying=false;

void MainWindow::PlayMusic(){
    getMusicLength();
    filepath=str.toUtf8().constData();
    qDebug()<<"Elapsed Time: "<<elapsedTime;
    emit seekTo(filepath,elapsedTime);
    emit ResumeOperation(filepath);
    isPlaying=true;
    ui->pushButton_6->setIcon(*pausebutton);
    updateStopwatch();
    startStopwatch();
    QLabel *label = ui->label;
    label->setMaximumWidth(250);
    label->setText("<marquee>"+fileName+"</marquee>");
}
void MainWindow::PauseMusic(){
    pauseStopwatch();
    emit PauseOperation();
    isPlaying=false;
    ui->pushButton_6->setIcon(*playbutton);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(!isPlaying){
        if(str=="\0"){
            //Error
            QMessageBox message;
            message.setText("No Music");
            message.exec();
            message.show();
            return;
        }
        else{
            PlayMusic();
        }
    }
    else{
        PauseMusic();
    }
}

void MainWindow::setImage(QString imageDirectorystr,QString imageName){
    QDir imageDirectory(imageDirectorystr);
    if(imageDirectory.exists(imageName)){
        QString imageFilePath = imageDirectorystr + imageName;
        qDebug()<<"Path found: "<<imageFilePath;
        QIcon albumCover(imageFilePath);
        QSize coversize;
        coversize.setHeight(200);
        coversize.setWidth(200);
        ui->AlbumPlaceHolder->setIcon(albumCover);
        ui->AlbumPlaceHolder->setIconSize(coversize);
    }
    else{
        QString noalbumart = "Images/noalbumart.png";
        QIcon albumCover(noalbumart);
        QSize coversize;
        coversize.setHeight(200);
        coversize.setWidth(200);
        ui->AlbumPlaceHolder->setIcon(albumCover);
        ui->AlbumPlaceHolder->setIconSize(coversize);
    }
}

QListWidgetItem *currentItem;
int currentItemRow=-1;
void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(ui->listWidget->row(item)==currentItemRow){
        return;
    }
    currentItem = item;
    currentItemRow=ui->listWidget->row(currentItem);
    resetTimer();
    fileName= item->text();
    currIndex = ui->listWidget->row(item);

    //ouput index
    qDebug()<<currIndex;
    ui->label->setText(fileName);
    QString path = songPaths.at(currIndex);
    str = path;
    QString imageName = fileName + ".jpg";
    QString imageDirectorystr = "Images/";
    setImage(imageDirectorystr,imageName);
    emit reset_Music();

    if(isPlaying){
        QThread::msleep(500);
        on_pushButton_6_clicked();
        filepath= str.toUtf8().constData();
        on_pushButton_6_clicked();
    }
    else{
        on_pushButton_6_clicked();
    }

}

bool isRepeated = false;

void MainWindow::on_forwButton_clicked()
{
    if(ui->repeatButton->isChecked())
    {
        on_seekbar_slider_sliderMoved(0);
        emit reset_Music();
        QThread::msleep(500);
        on_pushButton_6_clicked();
        filepath= str.toUtf8().constData();
        on_pushButton_6_clicked();
        return;
    }

    if(currIndex+1==ui->listWidget->count()){
        currIndex=-1;
    }
    resetTimer();
    currIndex++;
    ui->listWidget->item(currIndex)->setSelected(true);
    QString path = songPaths.at(currIndex);
    QListWidgetItem *currItem = ui->listWidget->item(currIndex);
    fileName = currItem->text();
    ui->label->setText(fileName);
    str=path;

    QString str = currItem->text();

    QString imageName = str + ".jpg";
    QString imageDirectorystr =  "Images/";
    setImage(imageDirectorystr,imageName);
    qDebug()<<"dsadd"<<str;
    emit reset_Music();
    if(isPlaying){
        QThread::msleep(500);
        on_pushButton_6_clicked();
        filepath= str.toUtf8().constData();
        on_pushButton_6_clicked();
    }
    else{
        on_pushButton_6_clicked();
    }
}
void MainWindow::resetTimer(){
    ui->currentSongLabel->setText("00:00:00");
    elapsedTime=-1;
}
void MainWindow::on_backButton_clicked()
{
    if(ui->repeatButton->isChecked())
    {
        on_seekbar_slider_sliderMoved(0);
        emit reset_Music();
        on_pushButton_6_clicked();
        return;
    }

    if(currIndex==0){
        currIndex=ui->listWidget->count();
    }

    resetTimer();
    currIndex--;
    ui->listWidget->item(currIndex)->setSelected(true);
    QString path = songPaths.at(currIndex);
    QListWidgetItem *currItem = ui->listWidget->item(currIndex);
    fileName = currItem->text();
    ui->label->setText(fileName);
    str=path;

    QString str = currItem->text();

    QString imageName = str + ".jpg";
    QString imageDirectorystr =  "Images/";
    setImage(imageDirectorystr,imageName);
    emit reset_Music();
    if(isPlaying){
        QThread::msleep(500);
        on_pushButton_6_clicked();
        filepath= str.toUtf8().constData();
        on_pushButton_6_clicked();
    }
    else{
        on_pushButton_6_clicked();
    }
}

void MainWindow::on_textEdit_textChanged()
{
    QVector<QString> widgetItem;
    QString searchText = ui->textEdit->toPlainText().toLower();
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        widgetItem.push_back(item->text().toLower()); // Convert item text to lowercase for case-insensitive search
    }

    qDebug()<<searchText;
    sort(widgetItem.begin(), widgetItem.end());

    int low = 0;
    int high = widgetItem.size() - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        if (widgetItem[mid] == searchText) {
            // qDebug() << "Value Found";
            return;
        }

        if (widgetItem[mid] < searchText)
            low = mid + 1;
        else
            high = mid - 1;
    }

    searchText = ui->textEdit->toPlainText().toLower();

    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem *item = ui->listWidget->item(i);
        QString fileName = item->text().toLower();
        if (fileName.contains(searchText, Qt::CaseInsensitive))
        {
            item->setHidden(false);
        }
        else
        {
            item->setHidden(true);
        }
    }

}

void MainWindow::on_shuffle_button_clicked()
{
    if(ui->shuffle_button->isEnabled()){
        random_shuffle(songPaths.begin(), songPaths.end());

        ui->listWidget->clear();
        QVector<QString>::iterator it = songPaths.begin();
        while (it != songPaths.end())
        {
            QFileInfo fileInfo(*it);
            ui->listWidget->addItem(fileInfo.fileName());
            ++it;
        }
        return;
    }
    //unshuffle
    QDir dir(selectedDirectory);
    int count =1;
    ui->listWidget->clear();
    foreach(QFileInfo var,dir.entryInfoList())
    {   if (count> 2) {

            songPaths.push_back(var.absoluteFilePath());
            songNames.push_back(var.fileName());
            ui->listWidget->addItem(var.fileName());
    }
        count++;
    }
}

void MainWindow::on_repeatButton_clicked()
{
//    ui->repeatButton->setCheckable(true);
    isRepeated = true;
}

void MainWindow::on_playlist_itemClicked(QListWidgetItem *item)
{
    fileName= item->text();
    QString playlistName = ui->playlist_dropdown_menu->currentText();

}

void MainWindow::ReadFromFile(QString filepath,QVector<QString> vector){
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"error reading the file";
        return;
    }
    while(!file.atEnd()){
        QString str = file.readLine();
        vector.push_back(str);
    }
}

void MainWindow::WriteToFile2(QString filepath,QString str){
    QFile file(filepath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream<<str<<"\n";
        file.close();
    } else {
        qDebug()<<"error";
    }
}

void MainWindow::on_create_new_playlist_clicked()
{
    QString str = PLAYLIST + " " +QString::number(++playlistCount);

    playListForm.setWindowModality(Qt::ApplicationModal);
    playListForm.show();
}

void MainWindow::on_addToPlaylistButton_clicked()
{
    if(ui->playlist_dropdown_menu->count()==0){
        qDebug()<<"No PlayList Created";
        return;
    }
    QJsonArray selectedSongs;
    QJsonObject obj;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        QWidget *widget = ui->listWidget->itemWidget(item);
        if (widget) {
            QCheckBox *checkbox = qobject_cast<QCheckBox*>(widget);
            if (checkbox) {
                if (checkbox->isChecked()) {
                    qDebug() << "Checkbox for item" << i << "is checked";
                    obj["Music Name"] = checkbox->text();
                    obj["Music Path"] = songPaths.at(i);
                    selectedSongs.append(obj);
                } else {
                    qDebug() << "Checkbox for item" << i << "is unchecked";
                }
            } else {
                qDebug() << "No checkbox found for item" << i;
            }
        } else {
            qDebug() << "No widget found for item" << i;
        }
    }

    QString playListPath = "Playlists/"+ui->playlist_dropdown_menu->currentText()+".json";
    writeToPlaylist(playListPath,selectedSongs);
}

void MainWindow::writeToPlaylist(QString playlistPath,QJsonArray selectedSongs){

    QJsonDocument* doc = new QJsonDocument(selectedSongs);
    QFile playlist(playlistPath);

    if(!playlist.open(QIODevice::Append)){
        qDebug()<<"File Does Not Exist";
        return;
    }
    else{
        playlist.write(doc->toJson());
    }
}

void MainWindow::on_seekbar_slider_sliderMoved(int position)
{
    if(!ui->seekbar_slider->isMaximized()){
        elapsedTime = position;
        emit seekTo(filepath,elapsedTime);
    }
}

void MainWindow::on_seekbar_slider_sliderReleased()
{
    on_pushButton_6_clicked();
    on_pushButton_6_clicked();
}

void MainWindow::LoadMusic(QDir dir){
    int count =1;
    foreach(QFileInfo var,dir.entryInfoList())
    {   if (count> 2 && var.fileName().endsWith(".mp3")) {
            songPaths.push_back(var.absoluteFilePath());
            songNames.push_back(var.fileName());
            ui->listWidget->addItem(var.fileName());
            QString strMusic = var.fileName();
            QString strMusicImageFolderPath = "Images/";
            QString combinedPath =  strMusicImageFolderPath + strMusic + ".jpg";
            qDebug()<<combinedPath;
            string myStr = combinedPath.toStdString();
            const char *album_art_file = myStr.c_str();
            const char* musicPath = var.filePath().toStdString().c_str();
            qDebug()<<musicPath;
            retrieve_album_art(musicPath,album_art_file);
        }
        count++;
    }
}

void MainWindow::on_refreshButton_clicked()
{
    ui->listWidget->clear();
    songNames.clear();
    songPaths.clear();
    QDir dir(selectedDirectory);
    ui->listWidget->clear();
    LoadMusic(dir);
}

void MainWindow::createPlaylist(QString playlist_name)
{
    ui->playlist_dropdown_menu->addItem(playlist_name);
    QJsonArray jsonArray;
    QJsonObject jsonObject;
    jsonObject["Music Name"];
    jsonObject["Music Path"];
    jsonArray.append(jsonObject);
    QJsonDocument* jsonDoc= new QJsonDocument(jsonObject);
    QFile playListJSONFile("Playlists/"+playlist_name+""
                                                ".json");
    if(!playListJSONFile.open(QIODevice::Append)){
        qDebug()<<"Error";
    }
    playListJSONFile.write(jsonDoc->toJson());
    playListJSONFile.close();
}

void MainWindow::setPlaylistName(QString playlist_name)
{
    this->playlist_name = playlist_name;
    qDebug()<<playlist_name;
    createPlaylist(playlist_name);
}

void MainWindow::on_listWidget_itemPressed(QListWidgetItem *item)
{
    qDebug()<<"item pressed";
}

QVector<QString>names;
void MainWindow::on_pushButton_clicked()
{
    if(!checkBox){
        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            QCheckBox *itemCheckBox = new QCheckBox(item->text(),ui->listWidget);
            itemCheckBox->setStyleSheet("QCheckBox{"
                "background-color: transparent;"
                "font: 700 9pt ""TT Norms Pro"";"
                "border:none;"
                "color:white;"
                "}"
            "QCheckBox::indicator{color:black;}"
            );
            ui->listWidget->setItemWidget(item,itemCheckBox);
            names.push_back(item->text());
            item->setText("");
        }
        checkBox=true;
        return;
    }

    if (checkBox){
        ui->listWidget->clear();
        for (const QString &name : names) {
            QListWidgetItem *item = new QListWidgetItem(name);
            ui->listWidget->addItem(item);
        }
        checkBox = false;
        ui->listWidget->item(currentItemRow)->setSelected(true);
        names.clear();
        return;
    }
}

void MainWindow::on_volumesilder_sliderReleased()
{

}

void MainWindow::on_volumesilder_sliderMoved(int volume)
{
    if(volume==50){
        ui->volume_button->setIcon(*volumeLow);
    }
    else if(volume==0){
        ui->volume_button->setIcon(*volumeMute);
    }
    else if(volume>=80){
        ui->volume_button->setIcon(*volumeHigh);
    }
    int newVolume = (volume * 65535) / 100;
    waveOutSetVolume(NULL, MAKELONG(newVolume, newVolume));
}

void MainWindow::hideVolumeSlider(){
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->volumesilder);
    ui->volumesilder->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(1);
    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(200);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->start();
    isSliderHidden=true;
}
void MainWindow::revealVolumeSlider(){
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->volumesilder);
    ui->volumesilder->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);
    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(200);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    isSliderHidden=false;
}
void MainWindow::on_volume_button_clicked()
{
    if(!isSliderHidden){
        hideVolumeSlider();
    }
    else{
        revealVolumeSlider();
    }
}

QList<QString> currentPlaylistPaths;
void MainWindow::on_playlist_dropdown_menu_currentTextChanged(const QString &arg1)
{
    QFile file("Playlists/"+arg1+".json");
    file.open(QFile::ReadWrite);
    QByteArray b = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(b);
    QJsonArray jsonArray = doc.array();

    for(int i=0;i<jsonArray.count();i++){
        currentPlaylistPaths.push_back(jsonArray.at(i).toObject()["Music Path"].toString());
    }
    qDebug()<<"Current Playlist Items";
    for(int i=0;i<currentPlaylistPaths.count();i++){
        qDebug()<<currentPlaylistPaths.at(i);
    }
    qDebug()<<"Activated";
}

