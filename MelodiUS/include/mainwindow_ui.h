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
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>



class RoundButton : public QPushButton
{
    Q_DISABLE_COPY(RoundButton)
public:
    using QPushButton::QPushButton;

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QPushButton::resizeEvent(event);
        const QRect buttonRect = rect();
        setMask(QRegion(buttonRect.x(),
                        buttonRect.y(),
                        buttonRect.width(),
                        buttonRect.height(),
                        QRegion::Ellipse));
    }
};
    

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

    RoundButton buttonRecord;
    RoundButton buttonStopRecord;
    RoundButton buttonPlay;
    RoundButton buttonProcess;
    QPushButton buttonSaveLoad;

    QMessageBox SLD; 


    Ui_MainWindow()               = delete;
    Ui_MainWindow(Ui_MainWindow&) = delete;
    Ui_MainWindow(QMainWindow* mainwindow)
    : centralwidget(mainwindow),

      groupBoxPartition(&centralwidget),
      groupBoxMenu(&centralwidget),

      label(&groupBoxMenu),
      label_3(&groupBoxPartition),
      label_A(&groupBoxMenu),
      label_I(&groupBoxMenu),
      label_hey(&groupBoxMenu),
      label_est(&groupBoxMenu),


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
      buttonSaveLoad(&groupBoxMenu)
    {
    }

    void setupUi(QMainWindow* mainWindow)
    {
     
   
        mainWindow->resize(1600, 900);
        groupBoxMenu.setGeometry(0, 0, 250, 900);
        groupBoxPartition.setGeometry(250, 0, 1350, 900);

        label.setGeometry(QRect(10, 775,250, 10));
        label.setText("MelodiUS V1.4   UwU Solide Performance");
        label_3.setGeometry(QRect(10, 130, 51, 101));
        label_3.setPixmap(QPixmap(QString::fromUtf8("images/cle40x80T.png")));
        label_A.setGeometry(QRect(50, 50, 51, 101));
        label_A.setText("A");
        label_I.setGeometry(QRect(50, 200, 51, 101));
        label_I.setText("I");
        label_hey.setGeometry(QRect(50, 350, 51, 101));
        label_hey.setText("HEY");
        label_est.setGeometry(QRect(50, 500, 51, 101));
        label_est.setText("EST");

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
        buttonRecord.setText("Enregistrement");
        buttonRecord.setStyleSheet("Border : none");
        buttonRecord.setStyleSheet("background-color:gray");

        buttonStopRecord.setGeometry(QRect(100, 50, 100, 100));
        buttonStopRecord.setText("Fin de l'enregistreement");
        buttonStopRecord.hide();

        buttonPlay.setGeometry(QRect(100, 200, 100, 100));
        buttonPlay.setText("Lecture");
        buttonPlay.setStyleSheet("Border : none");
        buttonPlay.setStyleSheet("background-color:gray");
       
        

        buttonProcess.setText("Traitement");
        buttonProcess.setGeometry(QRect(100, 350, 100, 100));
        buttonProcess.setStyleSheet("background-color:gray");

        buttonSaveLoad.setGeometry(QRect(100, 500, 100, 100));
        buttonSaveLoad.setText("Sauvegarde / Charge");

        SLD.setText("Solide Sauvegarde");
        SLD.setInformativeText("Vous pouvez enregistrer votre dernier enregistrement ou utiliser un fichier existant"); //Mettre des mots plus tard!
        SLD.setStandardButtons(QMessageBox::Save | QMessageBox::Open | QMessageBox::Cancel);
        SLD.setDefaultButton(QMessageBox::Cancel);
        SLD.setStyleSheet("background-color: yellow;");

        mainWindow->setCentralWidget(&centralwidget);
    }
};
