#pragma once
/********************************************************************************
** Form generated from reading UI file ''
**
** Created by: Qt User Interface Compiler version 6.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include "Partition.h"
#include "Portee.h"
#include "fpga_phoneme.h"
#include "globaldef.h"
#include "widgets/roundbutton.h"
#include <array>


class Ui_MainWindow
{
public:
    QWidget centralwidget;

    QScrollArea scrollArea;

    QGroupBox   groupBoxPartition;
    QGroupBox   groupBoxMenu;
    QMessageBox msgBoxSave;
    QMessageBox msgBoxLoad;

    QLabel label;
    QLabel label_A;
    QLabel label_I;
    QLabel label_hey;
    QLabel label_est;
    QLabel labelbar1;
    QLabel labelbar2;
    QLabel labelbar3;
    QLabel labelbar4;

    Partition P;

    QPushButton pushButtonA;
    RoundButton buttonRecord;
    RoundButton buttonStopRecord;
    RoundButton buttonPlay;
    RoundButton buttonProcess;
    RoundButton buttonSaveLoad;

    QMessageBox SLD;

    QProgressBar bargraph1;
    QProgressBar bargraph2;
    QProgressBar bargraph3;
    QProgressBar bargraph4;

    QTimer bargraphUpdater;


    Ui_MainWindow()               = delete;
    Ui_MainWindow(Ui_MainWindow&) = delete;

    Ui_MainWindow(QMainWindow* mainwindow)
    : centralwidget(mainwindow),

      scrollArea(&centralwidget),
      groupBoxPartition(&centralwidget),
      groupBoxMenu(&centralwidget),
      msgBoxSave(&groupBoxMenu),
      msgBoxLoad(&groupBoxMenu),


      label(&groupBoxMenu),
      label_A(&groupBoxMenu),
      label_I(&groupBoxMenu),
      label_hey(&groupBoxMenu),
      label_est(&groupBoxMenu),
      labelbar1(&groupBoxMenu),
      labelbar2(&groupBoxMenu),
      labelbar3(&groupBoxMenu),
      labelbar4(&groupBoxMenu),

      P(&centralwidget, &groupBoxPartition),

      pushButtonA(&groupBoxPartition),

      buttonRecord(&groupBoxMenu),
      buttonStopRecord(&groupBoxMenu),
      buttonPlay(&groupBoxMenu),
      buttonProcess(&groupBoxMenu),
      buttonSaveLoad(&groupBoxMenu),

      SLD(&groupBoxMenu),

      bargraph1(&groupBoxMenu),
      bargraph2(&groupBoxMenu),
      bargraph3(&groupBoxMenu),
      bargraph4(&groupBoxMenu),


      bargraphUpdater(&groupBoxMenu)
    {
    }

    void setupUi(QMainWindow* mainWindow)
    {
        mainWindow->showMaximized();
        groupBoxMenu.setGeometry(0, 0, 250, 900);
        groupBoxPartition.setGeometry(250, 0, 1500, 1500);

        scrollArea.setGeometry(250, 0, 1000, 1000);
        scrollArea.setWidget(&groupBoxPartition);
        scrollArea.setWidgetResizable(false);
        scrollArea.show();


        label.setGeometry(QRect(10, 850, 250, 10));
        label.setText("MelodiUS V1.5   UwU Solide Performance");
        label_A.setGeometry(QRect(50, 50, 51, 101));
        label_A.setText("A");
        label_I.setGeometry(QRect(50, 200, 51, 101));
        label_I.setText("I");
        label_hey.setGeometry(QRect(50, 350, 51, 101));
        label_hey.setText(QString::fromWCharArray(L"É"));
        label_est.setGeometry(QRect(50, 500, 51, 101));
        label_est.setText(QString::fromWCharArray(L"È"));

        groupBoxPartition.setGeometry(250, 0, 980, 885);
        scrollArea.setGeometry(250, 0, 1000, groupBoxMenu.height());
        scrollArea.setWidget(&groupBoxPartition);
        scrollArea.setWidgetResizable(false);
        scrollArea.show();


        pushButtonA.setGeometry(QRect(0, 0, 93, 50));
        pushButtonA.setText("Ajouter Portee");

        buttonRecord.setGeometry(QRect(100, 50, 100, 100));
        buttonRecord.SetImage({"images/record.png"});

        buttonStopRecord.setGeometry(QRect(100, 50, 100, 100));
        buttonStopRecord.SetImage({"images/stopRecord.png"});
        buttonStopRecord.hide();

        buttonPlay.setGeometry(QRect(100, 200, 100, 100));
        buttonPlay.SetImage({"images/play.png"});

        buttonProcess.setGeometry(QRect(100, 350, 100, 100));
        buttonProcess.SetImage({"images/process.png"});

        buttonSaveLoad.setGeometry(QRect(100, 500, 100, 100));
        buttonSaveLoad.SetImage({"images/save.png"});


        SLD.setText("Solide Sauvegarde");
        SLD.setInformativeText(
          "Vous pouvez enregistrer votre dernier enregistrement ou utiliser un fichier existant");    // Mettre des mots plus tard!
        SLD.setStandardButtons(QMessageBox::Save | QMessageBox::Open | QMessageBox::Cancel);
        SLD.setDefaultButton(QMessageBox::Cancel);

        msgBoxSave.setText("Erreur");
        msgBoxSave.setInformativeText("Erreur durant la sauvegarde (y-a-t'il un enregistrement actuel?)");
        msgBoxLoad.setText("Erreur");
        msgBoxLoad.setInformativeText("Erreur durant la charge (votre fichier est-il sous le bon format?)");

        // Phoneme Bargraphs
        bargraph1.setMaximum(255);
        bargraph1.setMinimum(0);
        bargraph1.setOrientation(Qt::Vertical);
        bargraph1.setGeometry(QRect(50, 700, 25, 100));
        bargraph1.setValue(50);
        bargraph1.setTextVisible(true);
        labelbar1.setGeometry(QRect(50, 680, 10, 30));
        labelbar1.setText("1");

        bargraph2.setMaximum(255);
        bargraph2.setMinimum(0);
        bargraph2.setOrientation(Qt::Vertical);
        bargraph2.setGeometry(QRect(100, 700, 25, 100));
        bargraph2.setValue(50);
        bargraph2.setTextVisible(true);
        labelbar2.setGeometry(QRect(100, 680, 10, 30));
        labelbar2.setText("2");

        bargraph3.setMaximum(255);
        bargraph3.setMinimum(0);
        bargraph3.setOrientation(Qt::Vertical);
        bargraph3.setGeometry(QRect(150, 700, 25, 100));
        bargraph3.setValue(50);
        bargraph3.setTextVisible(true);
        labelbar3.setGeometry(QRect(150, 680, 10, 30));
        labelbar3.setText("3");

        bargraph4.setMaximum(255);
        bargraph4.setMinimum(0);
        bargraph4.setOrientation(Qt::Vertical);
        bargraph4.setGeometry(QRect(200, 700, 25, 100));
        bargraph4.setValue(50);
        bargraph4.setTextVisible(true);
        labelbar4.setGeometry(QRect(200, 680, 10, 30));
        labelbar4.setText("4");

        mainWindow->setCentralWidget(&centralwidget);
        groupBoxMenu.raise();
    }
};
