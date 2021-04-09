#pragma once
/********************************************************************************
** Form generated from reading UI file ''
**
** Created by: Qt User Interface Compiler version 6.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtGlobal>
#if QT_VERSION >= 0x060000
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif
#include "Partition.h"
#include "Portee.h"
#include <QtCore/QVariant>
#include <QtGui/QBitmap>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>


class Ui_MainWindow
{
public:
    QWidget   centralwidget;
    QGroupBox groupBoxPartition;
    QGroupBox groupBoxMenu;

    QLabel label;
    QLabel label_A;
    QLabel label_I;
    QLabel label_hey;
    QLabel label_est;


    Partition P;


    QPushButton pushButtonA;

    QPushButton buttonRecord;
    QPushButton buttonStopRecord;
    QPushButton buttonPlay;
    QPushButton buttonProcess;
    QPushButton buttonSaveLoad;

    QRegion circle;

    Ui_MainWindow()               = delete;
    Ui_MainWindow(Ui_MainWindow&) = delete;

    Ui_MainWindow(QMainWindow* mainwindow)
    : centralwidget(mainwindow),

      groupBoxPartition(&centralwidget),
      groupBoxMenu(&centralwidget),

      label(&groupBoxMenu),


      P(&centralwidget,&groupBoxPartition),


      pushButtonA(&groupBoxPartition),
      buttonRecord(&groupBoxMenu),
      buttonStopRecord(&groupBoxMenu),
      buttonPlay(&groupBoxMenu),
      buttonProcess(&groupBoxMenu),
      buttonSaveLoad(&groupBoxMenu),
      circle(QRect(100, 200, 75, 75))




    {
    }

    void setupUi(QMainWindow* mainWindow)
    {
        // if(mainWindow->objectName().isEmpty())
        mainWindow->resize(1600, 900);
        groupBoxMenu.setGeometry(0, 0, 250, 900);
        groupBoxPartition.setGeometry(250, 0, 1000, 1000);


        label.setGeometry(QRect(10, 600, 500, 800));
        label.setText("MelodiUS V1.3   UwU Solide Performance");

        

        pushButtonA.setGeometry(QRect(70, 370, 93, 28));
        pushButtonA.setText("A");



        buttonRecord.setGeometry(QRect(100, 50, 100, 100));
        buttonRecord.setText("Record");

        buttonStopRecord.setGeometry(QRect(100, 50, 100, 100));
        buttonStopRecord.setText("Stop Recording");
        buttonStopRecord.hide();

        buttonPlay.setGeometry(QRect(100, 200, 100, 100));
        buttonPlay.setText("Play");
        QRegion cercle(QRect(100, 200, 75, 75));

        buttonProcess.setGeometry(QRect(100, 350, 100, 100));
        buttonProcess.setText("Process");

        buttonSaveLoad.setGeometry(QRect(100, 500, 100, 100));
        buttonSaveLoad.setText("Save| Load");

        mainWindow->setCentralWidget(&centralwidget);
    }
};
