#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_ui.h"
#include "recording.h"
#include "note.h"

#include <QtWidgets/QMainWindow>
#include <thread>


class MainWindow : public QMainWindow
{

public slots:
    void on_pushButtonA_clicked();
    void startRecord();
    void stopRecord();
    void updateBargraph();

    void play();
    void processing();
    void saveOrLoad();
    void saving();
    void loading();
    void interpret();

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private:
    void DisableAllButtons();
    void EnableAllButtons();
    void ConnectControlSignals();
    void DisconnectControlSignals();
    void SetupMenus();

private:
    Ui_MainWindow     ui;
    Recording         rec{};
    std::thread       recordingThread;
    std::vector<Note> playedNotes;
};
#endif    // MAINWINDOW_H
