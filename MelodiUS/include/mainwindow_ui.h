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

    QLabel* label;
    QLabel* label_3;
    QLabel* label_A;

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

  

    void setupUi(QMainWindow* mainWindow)
    {
       // if(mainWindow->objectName().isEmpty())
        //    mainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        mainWindow->resize(1600, 900);    
        centralwidget = new QWidget(mainWindow);
      //  centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
       // label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(4, 489, 851, 91));
        label_3 = new QLabel(centralwidget);
       // label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 130, 51, 101));
        label_3->setPixmap(QPixmap(QString::fromUtf8("../../MelodiUS/meloiusImage/cle40x80T.png")));

        ///=== LIGNE HORIZONTALE ========//
        line = new QFrame(centralwidget);
       // line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 130, 1600, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralwidget);
       // line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 150, 1600, 21));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(centralwidget);
       // line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(10, 170, 1600, 21));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(centralwidget);
       //line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(10, 190, 1600, 21));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(centralwidget);
       // line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(10, 210, 1600, 21));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        ///=== LIGNE VERTICALE ========//
        line_6 = new QFrame(centralwidget);
      //  line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setGeometry(QRect(250, 140, 20, 81));
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);

        spinBox = new QSpinBox(centralwidget);
     //   spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(50, 150, 42, 22));
        spinBox->setValue(4);
        spinBox->setRange(1, 8);
        spinBox_2 = new QSpinBox(centralwidget);
      //  spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setGeometry(QRect(50, 190, 42, 22));
        spinBox_2->setValue(4);
        spinBox_2->setRange(2, 8);
        spinBox_2->setSingleStep(2);

        pushButtonA = new QPushButton(centralwidget);
      //  pushButtonA->setObjectName(QString::fromUtf8("pushButton"));
        pushButtonA->setGeometry(QRect(70, 370, 93, 28));

        buttonRecord = new QPushButton(centralwidget);
      //  buttonRecord->setObjectName(QString::fromUtf8("buttonRecord"));
        buttonRecord->setGeometry(QRect(50, 300, 93, 28));
        buttonStopRecord = new QPushButton(centralwidget);
      //  buttonStopRecord->setObjectName(QString::fromUtf8("buttonStopRecord"));
        buttonStopRecord->setGeometry(QRect(50, 300, 93, 28));
        buttonStopRecord->hide();

        mainWindow->setCentralWidget(centralwidget);
        

        retranslateUi(mainWindow);

        
    }    

    void retranslateUi(QMainWindow* MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
         
        label->setText(QCoreApplication::translate(
          "MainWindow",
          "<html><head/><body><p>MelodiUS V1.0   UwU Solide Performance</p></body></html>",
          nullptr));
      
        pushButtonA->setText(QCoreApplication::translate("MainWindow", "A", nullptr));
        buttonRecord->setText(QCoreApplication::translate("MainWindow", "Record", nullptr));
        buttonStopRecord->setText(
          QCoreApplication::translate("MainWindow", "Stop Recording", nullptr));
       
    }    
};

namespace Ui
{
class MainWindow : public Ui_MainWindow
{
};
}    // namespace Ui

QT_END_NAMESPACE
