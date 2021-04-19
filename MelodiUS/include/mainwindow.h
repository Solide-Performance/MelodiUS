#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow_ui.h"
#include "recording.h"
#include <QtWidgets/QMainWindow>


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
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    //void darkMode();
    //void lightMode();


public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    bool voiceKey = false;

private:
    Ui_MainWindow ui;
    Recording     rec{};
};
#endif    // MAINWINDOW_H
