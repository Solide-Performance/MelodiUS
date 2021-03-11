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
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

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
    QGroupBox*    groupBox;
    QFrame*       line;
    QFrame*       line_2;
    QFrame*       line_3;
    QFrame*       line_4;
    QFrame*       line_5;
    QMenuBar*     menubar;
    QMenu*        menuFichier;
    QMenu*        menuOutil;
    QStatusBar*   statusbar;

    void setupUi(QMainWindow* MainWindow)
    {
        if(MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
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
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 771, 491));
        groupBox->setCheckable(false);
        line = new QFrame(groupBox);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 130, 761, 21));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(groupBox);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 150, 761, 21));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(groupBox);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(10, 170, 761, 21));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        line_4 = new QFrame(groupBox);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(10, 190, 761, 21));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(groupBox);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(10, 210, 761, 21));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

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
          "<html><head/><body><p>MelodiUS V0.1   UwU Solide Performance</p></body></html>",
          nullptr));
        groupBox->setTitle(
          QCoreApplication::translate("MainWindow", "Affichage partition", nullptr));
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
