#pragma once
/*************************************************************************************************/
/* Includes ------------------------------------------------------------------------------------ */
#include "Partition.h"
#include "Portee.h"
#include "fpga_phoneme.h"
#include "globaldef.h"
#include "widgets/levelled_progressbar.h"
#include "widgets/roundbutton.h"


/*************************************************************************************************/
/* Class definition ---------------------------------------------------------------------------- */
class Ui_MainWindow
{
public:
    /* --------------------------------- */
    /* Qt Widgets                        */
    QWidget centralwidget;

    QGroupBox   groupBoxPartition;
    QGroupBox   groupBoxMenu;
    QMessageBox msgBoxSave;
    QMessageBox msgBoxLoad;
    QMessageBox msgBoxCalibrate;
    QScrollArea scrollArea;

    QGridLayout globalLayout;
    QGridLayout menuLayout;
    QGridLayout barLayout;

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
    QPushButton buttonCalibrateA;
    QPushButton buttonCalibrateI;
    QPushButton buttonCalibrateEY;
    QPushButton buttonCalibrateAE;

    QMessageBox SLD;

    LevelledProgressBar bargraph1;
    LevelledProgressBar bargraph2;
    LevelledProgressBar bargraph3;
    LevelledProgressBar bargraph4;

    QTimer bargraphUpdater;


    /* --------------------------------- */
    /* Constructors                      */
    Ui_MainWindow()               = delete;
    Ui_MainWindow(Ui_MainWindow&) = delete;

    Ui_MainWindow(QMainWindow* mainwindow)
    : centralwidget(mainwindow),

      groupBoxPartition(&centralwidget),
      groupBoxMenu(&centralwidget),
      msgBoxSave(&groupBoxMenu),
      msgBoxLoad(&groupBoxMenu),
      msgBoxCalibrate(&centralwidget),
      scrollArea(&centralwidget),

      globalLayout(&centralwidget),
      menuLayout(&groupBoxMenu),
      barLayout(&groupBoxMenu),

      label(&groupBoxMenu),
      label_A(&groupBoxMenu),
      label_I(&groupBoxMenu),
      label_hey(&groupBoxMenu),
      label_est(&groupBoxMenu),
      labelbar1(&groupBoxMenu),
      labelbar2(&groupBoxMenu),
      labelbar3(&groupBoxMenu),
      labelbar4(&groupBoxMenu),

      P(&groupBoxPartition),

      pushButtonA(&scrollArea),

      buttonRecord(&groupBoxMenu),
      buttonStopRecord(&groupBoxMenu),
      buttonPlay(&groupBoxMenu),
      buttonProcess(&groupBoxMenu),
      buttonSaveLoad(&groupBoxMenu),
      buttonCalibrateA(&msgBoxCalibrate),
      buttonCalibrateI(&msgBoxCalibrate),
      buttonCalibrateEY(&msgBoxCalibrate),
      buttonCalibrateAE(&msgBoxCalibrate),

      SLD(&groupBoxMenu),

      bargraph1(&groupBoxMenu),
      bargraph2(&groupBoxMenu),
      bargraph3(&groupBoxMenu),
      bargraph4(&groupBoxMenu),


      bargraphUpdater(&groupBoxMenu)
    {
    }


    /* --------------------------------- */
    /* Methods                           */
    void setupUi(QMainWindow* mainWindow)
    {
        mainWindow->showMaximized();

        /* Setup labels */
        label.setText("MelodiUS V1.5   UwU Solide Performance");
        label_A.setText("A");
        label_I.setText("I");
        label_hey.setText(QString::fromWCharArray(L"É"));
        label_est.setText(QString::fromWCharArray(L"È"));
        labelbar1.setText("1");
        labelbar2.setText("2");
        labelbar3.setText("3");
        labelbar4.setText("4");


        /* Setup groupbox and scroll areas */
        groupBoxMenu.setMaximumWidth(250);
        groupBoxPartition.setGeometry(250, 0, 980, 1000);
        groupBoxPartition.setMaximumWidth(980);
        scrollArea.setGeometry(250, 0, 1080, 1000);
        scrollArea.setMaximumWidth(1000);
        scrollArea.setMinimumWidth(980);
        scrollArea.setWidget(&groupBoxPartition);
        scrollArea.setWidgetResizable(false);
        scrollArea.show();

        /* Setup buttons */
        pushButtonA.setText("Ajouter Portee");

        buttonRecord.SetImage({"images/record.png"});
        buttonRecord.setMinimumSize(100, 100);

        buttonStopRecord.SetImage({"images/stopRecord.png"});
        buttonStopRecord.setMinimumSize(100, 100);
        buttonStopRecord.hide();

        buttonPlay.SetImage({"images/play.png"});
        buttonPlay.setMinimumSize(100, 100);

        buttonProcess.SetImage({"images/process.png"});
        buttonProcess.setMinimumSize(100, 100);

        buttonSaveLoad.SetImage({"images/save.png"});
        buttonSaveLoad.setMinimumSize(100, 100);

        buttonCalibrateA.setText("A");
        buttonCalibrateI.setText("I");
        buttonCalibrateEY.setText(QString::fromWCharArray(L"É"));
        buttonCalibrateAE.setText(QString::fromWCharArray(L"È"));


        /* Setup message boxes */
        SLD.setText("Solide Sauvegarde");
        SLD.setInformativeText(
          "Vous pouvez sauvegarder votre dernier enregistrement"
          "ou utiliser un fichier existant");
        SLD.setStandardButtons(QMessageBox::Save | QMessageBox::Open | QMessageBox::Cancel);
        SLD.setDefaultButton(QMessageBox::Cancel);

        msgBoxSave.setText("Erreur");
        msgBoxSave.setInformativeText(
          "Erreur durant la sauvegarde (y-a-t'il un enregistrement actuel?)");
        msgBoxLoad.setText("Erreur");
        msgBoxLoad.setInformativeText(
          "Erreur durant la charge (votre fichier est-il sous le bon format?)");

        msgBoxCalibrate.setText(QString::fromWCharArray(L"Veuillez choisir un phonème à calibrer"));
        msgBoxCalibrate.setStandardButtons(QMessageBox::Cancel);
        msgBoxCalibrate.setDefaultButton(QMessageBox::Cancel);
        msgBoxCalibrate.addButton(&buttonCalibrateA, QMessageBox::ButtonRole::ActionRole);
        msgBoxCalibrate.addButton(&buttonCalibrateI, QMessageBox::ButtonRole::ActionRole);
        msgBoxCalibrate.addButton(&buttonCalibrateEY, QMessageBox::ButtonRole::ActionRole);
        msgBoxCalibrate.addButton(&buttonCalibrateAE, QMessageBox::ButtonRole::ActionRole);

        /* Layout */
        barLayout.addWidget(&labelbar1, 0, 0, Qt::AlignBottom | Qt::AlignHCenter);
        barLayout.addWidget(&labelbar2, 0, 1, Qt::AlignBottom | Qt::AlignHCenter);
        barLayout.addWidget(&labelbar3, 0, 2, Qt::AlignBottom | Qt::AlignHCenter);
        barLayout.addWidget(&labelbar4, 0, 3, Qt::AlignBottom | Qt::AlignHCenter);
        barLayout.addWidget(&bargraph1, 1, 0, Qt::AlignTop | Qt::AlignHCenter);
        barLayout.addWidget(&bargraph2, 1, 1, Qt::AlignTop | Qt::AlignHCenter);
        barLayout.addWidget(&bargraph3, 1, 2, Qt::AlignTop | Qt::AlignHCenter);
        barLayout.addWidget(&bargraph4, 1, 3, Qt::AlignTop | Qt::AlignHCenter);

        menuLayout.addWidget(&label_A, 0, 0, 2, 1, Qt::AlignCenter);
        menuLayout.addWidget(&label_I, 2, 0, 2, 1, Qt::AlignCenter);
        menuLayout.addWidget(&label_est, 4, 0, 2, 1, Qt::AlignCenter);
        menuLayout.addWidget(&label_hey, 6, 0, 2, 1, Qt::AlignCenter);
        menuLayout.addWidget(&buttonRecord, 0, 1, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);
        menuLayout.addWidget(&buttonStopRecord, 0, 1, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);
        menuLayout.addWidget(&buttonPlay, 2, 1, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);
        menuLayout.addWidget(&buttonProcess, 4, 1, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);
        menuLayout.addWidget(&buttonSaveLoad, 6, 1, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);

        menuLayout.addLayout(&barLayout, 8, 0, 2, 2);
        menuLayout.addWidget(&label, 11, 0, 1, 2);

        globalLayout.addWidget(&groupBoxMenu, 0, 0, 1, 1);
        globalLayout.addWidget(&scrollArea, 0, 1, 1, 3);
        centralwidget.setLayout(&globalLayout);

        bargraph1.ChangeVal(0);
        bargraph2.ChangeVal(255);
        bargraph3.ChangeVal(64);

        mainWindow->setCentralWidget(&centralwidget);
        groupBoxMenu.raise();
    }
};


/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */
