#pragma once 
#include "widgets/widget_note.h"
#include <vector>

class Temps
{
public:
    Temps(NoteWidget* note);
    ~Temps();

private:
    int nom;
    int denom;
    int valeur;

    std::vector<NoteWidget> Temp;
};