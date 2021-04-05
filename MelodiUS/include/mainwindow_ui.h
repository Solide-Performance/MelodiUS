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
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction*      actionNouveau;
    QAction*      actionImporter;
    QAction*      actionEnregistrer;
    QAction*      actionG_n_rer_un_sinus;
    QWidget*      centralwidget;

    QLabel*       label;
    QLabel*       label_3;
    QLabel*       label_A;


    QFrame*       line;
    QFrame*       line_2;
    QFrame*       line_3;
    QFrame*       line_4;
    QFrame*       line_5;
    QFrame*       line_6;

    QSpinBox*     spinBox;
    QSpinBox*     spinBox_2;

    QComboBox*    comboBox;
    QComboBox*    comboBox_2;

    QPushButton*  pushButtonA;

    QMenuBar*     menubar;
    QMenu*        menuFichier;
    QMenu*        menuOutil;
    QStatusBar*   statusbar;

    void setupUi(QMainWindow* MainWindow)
    {
        if(MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1600, 900);                                      //Window size
        actionNouveau = new QAction(MainWindow);
        actionNouveau->setObjectName(QString::fromUtf8("actionNouveau"));
        actionImporter = new QAction(MainWindow);
        actionImporter->setObjectName(QString::fromUtf8("actionImporter"));
        actionEnregistrer = new QAction(MainWindow);
        actionEnregistrer->setObjectName(QString::fromUtf8("actionEnregistrer"));
        actionG_n_rer_un_sinus = new QAction(MainWindow);
        actionG_n_rer_un_sinus->setObjectName(QString::fromUtf8("actionG_n_rer_un_sinus"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(4, 489, 851, 91));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 130, 51, 101));
        label_3->setPixmap(QPixmap(
          QString::fromUtf8("../../MelodiUS/meloiusImage/cle40x80T.png")));
        //groupBox = new QGroupBox(centralwidget);
        //groupBox->setObjectName(QString::fromUtf8("groupBox"));
        //groupBox->setGeometry(QRect(0, 0, 771, 491));
       // groupBox->setCheckable(false);

        ///=== LIGNE HORIZONTAL ========//
        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 130, 1600, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 150, 1600, 21));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(centralwidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(10, 170, 1600, 21));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(centralwidget);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(10, 190, 1600, 21));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(centralwidget);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(10, 210, 1600, 21));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        ///=== LIGNE VERTICAL ========//
        line_6 = new QFrame(centralwidget);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setGeometry(QRect(250, 140, 20, 81));
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);

      /*  comboBox = new QComboBox(centralwidget);              //Je les trouve pas mal moins belle
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(90, 150, 41, 22));
        comboBox_2 = new QComboBox(centralwidget);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setGeometry(QRect(90, 190, 41, 22));
        */
        spinBox = new QSpinBox(centralwidget);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(50, 150, 42, 22));
        spinBox->setValue(4);
        spinBox->setRange(1, 8);
        spinBox_2 = new QSpinBox(centralwidget);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setGeometry(QRect(50, 190, 42, 22));
        spinBox_2->setValue(4);
        spinBox_2->setRange(2, 8);
        spinBox_2->setSingleStep(2);

        pushButtonA = new QPushButton(centralwidget);
        pushButtonA->setObjectName(QString::fromUtf8("pushButton"));
        pushButtonA->setGeometry(QRect(70, 370, 93, 28));

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 26));
        menuFichier = new QMenu(menubar);
        menuFichier->setObjectName(QString::fromUtf8("menuFichier"));
        menuOutil = new QMenu(menubar);
        menuOutil->setObjectName(QString::fromUtf8("menuOutil"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFichier->menuAction());
        menubar->addAction(menuOutil->menuAction());
        menuFichier->addAction(actionNouveau);
        menuFichier->addAction(actionImporter);
        menuFichier->addAction(actionEnregistrer);
        menuOutil->addAction(actionG_n_rer_un_sinus);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    }    // setupUi

    void retranslateUi(QMainWindow* MainWindow)
    {
        MainWindow->setWindowTitle(
          QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNouveau->setText(QCoreApplication::translate("MainWindow", "Nouveau", nullptr));
        actionImporter->setText(QCoreApplication::translate("MainWindow", "Importer", nullptr));
        actionEnregistrer->setText(
          QCoreApplication::translate("MainWindow", "Enregistrer", nullptr));
        actionG_n_rer_un_sinus->setText(
          QCoreApplication::translate("MainWindow", "G\303\251n\303\251rer un sinus", nullptr));
        label->setText(QCoreApplication::translate(
          "MainWindow",
          "<html><head/><body><p>MelodiUS V0.3   UwU Solide Performance</p></body></html>",
          nullptr));
      //  centralwidget->setTitle(
        //  QCoreApplication::translate("MainWindow", "Affichage partition", nullptr));
        pushButtonA->setText(QCoreApplication::translate("MainWindow", "A", nullptr));
        menuFichier->setTitle(QCoreApplication::translate("MainWindow", "Fichier", nullptr));
        menuOutil->setTitle(QCoreApplication::translate("MainWindow", "Outil", nullptr));
    }    // retranslateUi


};

namespace Ui
{
class MainWindow : public Ui_MainWindow
{
};
}    // namespace Ui

QT_END_NAMESPACE
