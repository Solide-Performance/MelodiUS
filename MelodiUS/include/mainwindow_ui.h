#pragma once
/********************************************************************************
** Form generated from reading UI file ''
**
** Created by: Qt User Interface Compiler version 6.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include "globaldef.h"

class Ui_MainWindow
{
public:
    QWidget centralwidget;

    QGroupBox groupBoxPartition;
    QGroupBox groupBoxMenu;

    QLabel label;
    QLabel label_3;
    QLabel label_A;
    QLabel label_I;
    QLabel label_hey;
    QLabel label_est;

    QFrame line;
    QFrame line_2;
    QFrame line_3;
    QFrame line_4;
    QFrame line_5;
    QFrame line_6;

    QSpinBox spinBox;
    QSpinBox spinBox_2;

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
      label_3(&groupBoxPartition),

      line(&groupBoxPartition),
      line_2(&groupBoxPartition),
      line_3(&groupBoxPartition),
      line_4(&groupBoxPartition),
      line_5(&groupBoxPartition),

      line_6(&groupBoxPartition),

      spinBox(&groupBoxPartition),
      spinBox_2(&groupBoxPartition),

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

        label_3.setGeometry(QRect(10, 130, 51, 101));
        label_3.setPixmap(QPixmap(QString::fromUtf8("images/cle40x80T.png")));

        ///=== LIGNE HORIZONTALE ========//

        line.setGeometry(QRect(10, 130, 1600, 21));
        line.setFrameShape(QFrame::HLine);
        line.setFrameShadow(QFrame::Sunken);

        line_2.setGeometry(QRect(10, 150, 1600, 21));
        line_2.setFrameShape(QFrame::HLine);
        line_2.setFrameShadow(QFrame::Sunken);

        line_3.setGeometry(QRect(10, 170, 1600, 21));
        line_3.setFrameShape(QFrame::HLine);
        line_3.setFrameShadow(QFrame::Sunken);

        line_4.setGeometry(QRect(10, 190, 1600, 21));
        line_4.setFrameShape(QFrame::HLine);
        line_4.setFrameShadow(QFrame::Sunken);

        line_5.setGeometry(QRect(10, 210, 1600, 21));
        line_5.setFrameShape(QFrame::HLine);
        line_5.setFrameShadow(QFrame::Sunken);

        ///=== LIGNE VERTICALE ========//

        line_6.setGeometry(QRect(250, 140, 20, 81));
        line_6.setFrameShape(QFrame::VLine);
        line_6.setFrameShadow(QFrame::Sunken);



        spinBox.setGeometry(QRect(50, 150, 42, 22));
        spinBox.setValue(4);
        spinBox.setRange(1, 8);

        spinBox_2.setGeometry(QRect(50, 190, 42, 22));
        spinBox_2.setValue(4);
        spinBox_2.setRange(2, 8);
        spinBox_2.setSingleStep(2);


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
