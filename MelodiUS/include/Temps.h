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
    Temps() = default;
    ~Temps() = default;

private:
    int nom;
    int denominateur;

    std::vector<NoteWidget> Temp;
};