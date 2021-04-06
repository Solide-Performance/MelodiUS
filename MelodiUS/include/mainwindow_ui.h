#pragma once
/********************************************************************************
** Form generated from reading UI file ''
**
** Created by: Qt User Interface Compiler version 6.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include <QtCore/QVariant>
#include <QtGui/QAction>
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

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
   
    QWidget* centralwidget;

    QGroupBox* groupBoxPartition;
    QGroupBox* groupBoxMenu;

    QLabel* label;
    QLabel* label_3;
    QLabel* label_A;
    QLabel* label_I;
    QLabel* label_hey;
    QLabel* label_est;

    QFrame* line;
    QFrame* line_2;
    QFrame* line_3;
    QFrame* line_4;
    QFrame* line_5;
    QFrame* line_6;

    QSpinBox* spinBox;
    QSpinBox* spinBox_2;

    QPushButton* pushButtonA;
    
    QPushButton* buttonRecord;
    QPushButton* buttonStopRecord;
    QPushButton* buttonPlay;
    QPushButton* buttonProcess;
    QPushButton* buttonSaveLoad;


  

    void setupUi(QMainWindow* mainWindow)
    {

        mainWindow->resize(1600, 900);    
        centralwidget = new QWidget(mainWindow);

        groupBoxPartition = new QGroupBox(centralwidget);
        groupBoxPartition->setGeometry(QRect(250, 0, 1200, 900)); // On peut enlever quelque hardcode 
        groupBoxPartition->setCheckable(false);

        groupBoxMenu = new QGroupBox(centralwidget);
        groupBoxMenu->setGeometry(QRect(0, 0, 250, 900));
        groupBoxMenu->setCheckable(false);

        label = new QLabel(centralwidget);
        label->setGeometry(QRect(5, 740, 851, 91));

        label_A = new QLabel(centralwidget);
        label_A->setGeometry(QRect(50, 50, 851, 91));
        label_I = new QLabel(centralwidget);
        label_I->setGeometry(QRect(50, 200, 851, 91));
        label_hey = new QLabel(centralwidget);
        label_hey->setGeometry(QRect(50, 350, 851, 91));
        label_est = new QLabel(centralwidget);
        label_est->setGeometry(QRect(50, 500, 851, 91));

        label_3 = new QLabel(centralwidget);
        label_3->setGeometry(QRect(groupBoxPartition->x()+10, groupBoxPartition->y()+130, 51, 101));
        label_3->setPixmap(QPixmap(QString::fromUtf8("../../MelodiUS/meloiusImage/cle40x80T.png")));

        ///=== LIGNE HORIZONTALE ========//

        line = new QFrame(groupBoxPartition);
        line->setGeometry(QRect(10, 130, 1600, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        line_2 = new QFrame(groupBoxPartition);
        line_2->setGeometry(QRect(10, 150, 1600, 21));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        line_3 = new QFrame(groupBoxPartition);
        line_3->setGeometry(QRect(10, 170, 1600, 21));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        line_4 = new QFrame(groupBoxPartition);
        line_4->setGeometry(QRect(10, 190, 1600, 21));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        line_5 = new QFrame(groupBoxPartition);
        line_5->setGeometry(QRect(10, 210, 1600, 21));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        ///=== LIGNE VERTICALE ========//

        line_6 = new QFrame(groupBoxPartition);
        line_6->setGeometry(QRect(250, 140, 20, 81));
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);

        spinBox = new QSpinBox(groupBoxPartition);
        spinBox->setGeometry(QRect(50, 150, 42, 22));
        spinBox->setValue(4);
        spinBox->setRange(1, 8);

        spinBox_2 = new QSpinBox(groupBoxPartition);
        spinBox_2->setGeometry(QRect(50, 190, 42, 22));
        spinBox_2->setValue(4);
        spinBox_2->setRange(2, 8);
        spinBox_2->setSingleStep(2);

        pushButtonA = new QPushButton(groupBoxPartition);
        pushButtonA->setGeometry(QRect(70, 370, 93, 28));

        buttonRecord = new QPushButton(groupBoxMenu);
        buttonRecord->setGeometry(QRect(100, 50, 100, 100));
       
        buttonStopRecord = new QPushButton(groupBoxMenu);
        buttonStopRecord->setGeometry(QRect(100, 50, 100, 100));
        buttonStopRecord->hide();

        buttonPlay = new QPushButton(groupBoxMenu);
        buttonPlay->setGeometry(QRect(100, 200, 100, 100));

        buttonProcess = new QPushButton(groupBoxMenu);
        buttonProcess->setGeometry(QRect(100, 350, 100, 100));

        buttonSaveLoad = new QPushButton(groupBoxMenu);
        buttonSaveLoad->setGeometry(QRect(100, 500, 100, 100));
       // QPushButton #buttonSaveLoad{border-radius :10 px} never worked yet. https://doc.qt.io/qt-5/stylesheet-examples.html
        

        mainWindow->setCentralWidget(centralwidget);
        

        retranslateUi(mainWindow);

        
    }    

    void retranslateUi(QMainWindow* MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
         
        label->setText(QCoreApplication::translate("MainWindow","<html><head/><body><p>MelodiUS V1.1   UwU Solide Performance</p></body></html>", nullptr));
        label_A->setText(QCoreApplication::translate("MainWindow", "A", nullptr));
        label_I->setText(QCoreApplication::translate("MainWindow", "I", nullptr));
        label_hey->setText(QCoreApplication::translate("MainWindow", "hey", nullptr));
        label_est->setText(QCoreApplication::translate("MainWindow", "est", nullptr));
        pushButtonA->setText(QCoreApplication::translate("MainWindow", "LA", nullptr));
        buttonRecord->setText(QCoreApplication::translate("MainWindow", "Record", nullptr));
        buttonStopRecord->setText(QCoreApplication::translate("MainWindow", "Stop Recording", nullptr));
        buttonPlay->setText(QCoreApplication::translate("MainWindow", "Play", nullptr));
        buttonProcess->setText(QCoreApplication::translate("MainWindow", "Process", nullptr));
        buttonSaveLoad->setText(QCoreApplication::translate("MainWindow", "Save &| Load", nullptr));
        groupBoxPartition->setTitle(QCoreApplication::translate("MainWindow", "Affichage partition", nullptr));
        groupBoxMenu->setTitle(QCoreApplication::translate("MainWindow", "Menu", nullptr));
       
    }    
};

namespace Ui
{
class MainWindow : public Ui_MainWindow
{
};
}    // namespace Ui

QT_END_NAMESPACE
