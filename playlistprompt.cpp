#include "playlistprompt.h"
#include "ui_playlistprompt.h"
#include <mainwindow.h>

PlayListPrompt::PlayListPrompt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayListPrompt)

{
    ui->setupUi(this);
    ui->textEdit->setPlaceholderText("Playlist Name");
}

PlayListPrompt::~PlayListPrompt()
{
    delete ui;
}

void PlayListPrompt::on_textEdit_textChanged()
{
    playlist_name = ui->textEdit->toPlainText();
}


void PlayListPrompt::on_done_clicked()
{
    emit sendplaylistName(playlist_name);
    close();
}

