#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "mainwindow_ui.h"



class MainWindow : public QMainWindow
{
    
public slots:
   void on_pushButton_clicked();
   void startRecord();
   void stopRecord();

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui_MainWindow ui;
};
#endif    // MAINWINDOW_H
