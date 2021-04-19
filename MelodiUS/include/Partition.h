#pragma once
#include "Portee.h"
#include "globaldef.h"
#include "widgets/widget_note.h"
#include <vector>


class Partition
{
public:
    Partition(QWidget* parent) : m_parent(parent), spinBox(parent), spinBox_2(parent), feuille{}
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
    ~Partition() = default;

    void ecrireMusique(std::vector<Note> Note);

private:
    QWidget* m_parent;
    int      ajoutLigne();
    bool     mesureEstPleine();

private:
    int      nom;
    int      denom;
    int      valeur;
    int      nbsLigne;
    QSpinBox spinBox;
    QSpinBox spinBox_2;

    std::vector<Portee>     feuille;
    std::vector<NoteWidget> composition;
};
