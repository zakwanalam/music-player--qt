#ifndef PLAYLISTPROMPT_H
#define PLAYLISTPROMPT_H

#include <QWidget>
#include <QMainWindow>
namespace Ui {
class PlayListPrompt;
}

class PlayListPrompt : public QWidget
{
    Q_OBJECT
public:
    explicit PlayListPrompt(QWidget *parent = nullptr);
    ~PlayListPrompt();
    QString playlist_name;
private slots:
    void on_textEdit_textChanged();
    void on_done_clicked();
signals:
    void sendplaylistName(QString playlist_name);

private:
    Ui::PlayListPrompt *ui;

};

#endif // PLAYLISTPROMPT_H

