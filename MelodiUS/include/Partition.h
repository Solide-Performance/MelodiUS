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
        spinBox.setGeometry(QRect(55, 180, 30, 22));
        spinBox.setValue(4);
        spinBox.setRange(1, 8);
        spinBox.raise();

        spinBox_2.setGeometry(QRect(55, 210, 30, 22));
        spinBox_2.setValue(4);
        spinBox_2.setRange(1, 4);
        spinBox_2.setSingleStep(1);
        spinBox_2.raise();
    };
    ~Partition() = default;

    void ecrireMusique(std::vector<Note> Note);
    void clear()
    {
        nom = 0;
        denom = 0;
        valeur = 0;
        nbsLigne = 0;       
        composition.clear();
        compo.clear();
        for(size_t i = 1; i < feuille.size(); i++)
        {
            feuille[i].~Portee();
        }
        ajoutLigne();
    }
    void resteApres(double reste, int mesure, int valeurParMesure, std::vector<Note> vecNote,int i);

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
    std::vector<std::vector<Note>> compo;
};
