#pragma once 
#include "Partition.h"
#include "widgets/widget_note.h"
#include <vector>

class Temps
{
public:
    Temps(NoteWidget* note)
    {
    }
    Temps()  = default;
    ~Temps() = default;

private:
    int nom;               //did you mean num as in numerator? 
    int denominateur;

    std::vector<NoteWidget> Temp;
};