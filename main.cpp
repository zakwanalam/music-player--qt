#include "mainwindow.h"
#include <QApplication>
#include <QMediaPlayer>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setStyleSheet("background-colour: #A9A9A9;");
    MainWindow w;
    w.setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                    "stop:0 #042940, "
                    "stop:0.5 #00364a, "
                    "stop:0.7 #004351, "
                    "stop:1 #005054, "
                    "stop:2 #005c53); "
                    "border: 1px solid rgba(75, 0, 130, 50); "
                    "border-radius: 10px;");
    w.setMinimumSize(730,550);
    w.setMaximumSize(800, 600);
    w.show();

    return a.exec();
}

