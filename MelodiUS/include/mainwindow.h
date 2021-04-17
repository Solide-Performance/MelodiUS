#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "mainwindow_ui.h"



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
   void darkMode();
   void lightMode();


public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void resizeEvent(QResizeEvent* main);

private:
    Ui_MainWindow ui;
};
#endif    // MAINWINDOW_H
