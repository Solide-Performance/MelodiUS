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
#include <array>


class RoundButton : public QPushButton
{
    Q_DISABLE_COPY(RoundButton)
private:
    QPixmap image;

public:
    using QPushButton::QPushButton;
    void SetImage(QPixmap newImage)
    {
        image = newImage;
    }

public slots:
    void paintEvent(QPaintEvent*)
    {
        QColor background = isDown() ? QColor("grey") : QColor("lightgrey");
        int    diameter   = qMin(height(), width()) - 4;

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.translate(width() / 2, height() / 2);

        painter.setPen(QPen(QColor("grey"), 1));
        painter.setBrush(QBrush(background));
        painter.drawEllipse(QRect(-diameter / 2, -diameter / 2, diameter, diameter));

        if(!image.isNull())
        {
            int xOff = (-diameter / 2) + (image.width() * 3 / 2) - 1;
            int yOff = (-diameter / 2) + (image.height() * 3 / 2) - 1;
            painter.drawPixmap(QRect(xOff, yOff, image.width(), image.height()), image);
        }
    }

    void resizeEvent(QResizeEvent* event) override
    {
        QPushButton::resizeEvent(event);

        int diameter = qMin(height(), width()) + 4;
        int xOff     = (width() - diameter) / 2;
        int yOff     = (height() - diameter) / 2;
        setMask(QRegion(xOff, yOff, diameter, diameter, QRegion::Ellipse));
         
    }
};


class Ui_MainWindow
{
public:
    QWidget centralwidget;

    QScrollArea scrollArea;
    QGroupBox   groupBoxPartition;
    QGroupBox   groupBoxMenu;


    QLabel label;
    QLabel label_A;
    QLabel label_I;
    QLabel label_hey;
    QLabel label_est;
    
    QFrame lineDeTest;

    Partition P;


    QPushButton pushButtonA;

    RoundButton buttonRecord;
    RoundButton buttonStopRecord;
    RoundButton buttonPlay;
    RoundButton buttonProcess;
    RoundButton buttonSaveLoad;
    QPushButton buttonDark;
    QPushButton buttonLight;

    QMessageBox SLD;

    QProgressBar bargraph1;
    QProgressBar bargraph2;
    QProgressBar bargraph3;
    QProgressBar bargraph4;
    QLabel       labelbar1;
    QLabel       labelbar2;
    QLabel       labelbar3;
    QLabel       labelbar4;
    QTimer       bargraphUpdater;


    Ui_MainWindow()               = delete;
    Ui_MainWindow(Ui_MainWindow&) = delete;

    Ui_MainWindow(QMainWindow* mainwindow)
    : centralwidget(mainwindow),

      scrollArea(&centralwidget),
      groupBoxPartition(&centralwidget),
      groupBoxMenu(&centralwidget),


      label(&groupBoxMenu),
      label_A(&groupBoxMenu),
      label_I(&groupBoxMenu),
      label_hey(&groupBoxMenu),
      label_est(&groupBoxMenu),


      P(&centralwidget, &groupBoxPartition),


      pushButtonA(&groupBoxPartition),
      buttonRecord(&groupBoxMenu),
      buttonStopRecord(&groupBoxMenu),
      buttonPlay(&groupBoxMenu),
      buttonProcess(&groupBoxMenu),
      buttonSaveLoad(&groupBoxMenu),
      buttonDark(&groupBoxMenu),
      buttonLight(&groupBoxMenu),
      bargraph1(&groupBoxMenu),
      bargraph2(&groupBoxMenu),
      bargraph3(&groupBoxMenu),
      bargraph4(&groupBoxMenu),
      labelbar1(&groupBoxMenu),
      labelbar2(&groupBoxMenu),
      labelbar3(&groupBoxMenu),
      labelbar4(&groupBoxMenu),
      bargraphUpdater(&groupBoxMenu),
      lineDeTest(&groupBoxMenu)
    {
    }

    void setupUi(QMainWindow* mainWindow)
    {
        mainWindow->resize(1250, 900);
        groupBoxMenu.setGeometry(0, 0, 250, 900);
        groupBoxMenu.setStyleSheet("background-color:#ffffff");
        
        label.setGeometry(QRect(10, 850, 250, 10));
        label.setText("MelodiUS V1.5   Quelle Solide Performance!");
        
        label_A.setGeometry(QRect(50, 50, 51, 101));
        label_A.setText("A");
        label_I.setGeometry(QRect(50, 200, 51, 101));
        label_I.setText("I");
        label_hey.setGeometry(QRect(50, 350, 51, 101));
        label_hey.setText("HEY");
        label_est.setGeometry(QRect(50, 500, 51, 101));
        label_est.setText("EST");

        groupBoxPartition.setGeometry(250, 0, (mainWindow->width()-250), mainWindow->height());
        //groupBoxPartition.setStyleSheet("background-color:#ffffff");
        scrollArea.setGeometry(250, 0, (mainWindow->width() - 250), groupBoxMenu.height());
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
        buttonPlay.setText("Lecture");
        buttonPlay.setStyleSheet("Border : none");
        buttonPlay.setStyleSheet("background-color:gray");



        buttonProcess.setText("Traitement");
        buttonProcess.setGeometry(QRect(100, 350, 100, 100));

        buttonSaveLoad.setGeometry(QRect(100, 500, 100, 100));
        buttonSaveLoad.setText("Sauvegarde / Charge");

        buttonDark.setGeometry(QRect(0, 0, 50, 20));
        buttonDark.setText("Nuit");
        buttonDark.raise();

        buttonLight.setGeometry(QRect(0, 0, 50, 20));
        buttonLight.setText("Jour");
        buttonLight.hide();


        SLD.setText("Solide Sauvegarde");
        SLD.setInformativeText(
          "Vous pouvez enregistrer votre dernier enregistrement ou utiliser un fichier existant");    // Mettre des mots plus tard!
        SLD.setStandardButtons(QMessageBox::Save | QMessageBox::Open | QMessageBox::Cancel);
        SLD.setDefaultButton(QMessageBox::Cancel);


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

        
        /*==============Test==============*/
        lineDeTest.setGeometry(QRect());
        lineDeTest.setFrameShape(QFrame::VLine);
        lineDeTest.setFrameShadow(QFrame::Plain);
        
        /*==============Fin des test==============*/


        mainWindow->setCentralWidget(&centralwidget);
        groupBoxMenu.raise();
    }
};
