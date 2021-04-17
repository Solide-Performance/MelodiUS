#pragma once
#include <QtGlobal>
#include <vector>
#if QT_VERSION >= 0x060000
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif
#include "Portee.h"
#include "widgets/widget_note.h"
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
#include <QtWidgets/qerrormessage.h>

class Partition
{
public:
    Partition(QWidget* centralwidget, QGroupBox* partitionGroupBox)
    : PartitionGroupBox(partitionGroupBox),
      spinBox(PartitionGroupBox),
      spinBox_2(PartitionGroupBox),
      feuille{}
    {
        nbsLigne = 0;
        ajoutLigne();
        spinBox.setGeometry(QRect(55, 150, 30, 22));
        spinBox.setValue(4);
        spinBox.setRange(1, 8);
        spinBox.raise();

        spinBox_2.setGeometry(QRect(55, 190, 30, 22));
        spinBox_2.setValue(4);
        spinBox_2.setRange(2, 8);
        spinBox_2.setSingleStep(2);
        spinBox_2.raise();
    };
    ~Partition();
    
    int  ajoutLigne();
    void ecrireMusique(std::vector<Note> Note);
    bool mesureEstPleine();
    

private:
    
    int                     nom;
    int                     denom;
    int                     valeur;
    int                 nbsLigne;
    
    QGroupBox*          PartitionGroupBox;
    QSpinBox            spinBox;
    QSpinBox            spinBox_2;
    
    std::vector<Portee> feuille;
    std::vector<NoteWidget>  composition;
};
