#include "Partition.h"
NoteType GetNoteValueValue(double d, NoteValue t);

Partition::~Partition()
{
}

int Partition::ajoutLigne()
{
    feuille.push_back(Portee(nbsLigne, PartitionGroupBox));
    nbsLigne++;
    spinBox.raise();
    spinBox_2.raise();
    return nbsLigne;
}

void Partition::ecrireMusique(std::vector<Note> vecNote)
{
    nom                                   = spinBox.value();
    denom                                 = spinBox.value();
    int                            ligne  = 0;
    int                            mesure = 0;
    std::vector<std::vector<Note>> compo;
    compo.push_back(std::vector<Note>());
    std::array<double, 5> valNoteComparasion = {4, 2, 1, 0.5, 0.25};
    if((nom != 1 && nom != 2 && nom != 4 && nom != 8 && nom != 16)
       && (denom != 1 && denom != 2 && denom != 3 && denom != 4 && denom != 5 && denom != 6
           && denom != 7 && denom != 8))
    {
        // QErrorMessage() n(&PartitionGroupBox);
    }
    int valeurParMesure = nom * (4 / denom);


    //=============== Boute de code qui ce repete pour chaque note du vecteur et qui transforme le tout en note prete a être mise dans une portee============//
    for(int i = 0; i < vecNote.size(); i++)
    {
        // On commence par verifier si la mesure est pleine et si oui on rajoute une mesuere
        double sum = 0;
        for(int j = 0; i < compo[mesure].size(); i++)
        {
            sum += compo[mesure][j].getNoteSum();
        }
        if(sum >= valeurParMesure)
        {
            mesure++;
            compo.push_back(std::vector<Note>());
        }
        //============================================================================
        // On regarde maintenant si la note rentre telle quelle dans le vecteur
        if(sum + vecNote[i].getNoteSum() <= valeurParMesure)    // si sa rentre on ajoute la note
        {
            compo[mesure].push_back(vecNote[i]);
        }
        else    // si sa rentre pas
        {
            // on commence par rentrer ce qui renrte et definir le reste qui rentre pas
            double reste        = sum + vecNote[i].getNoteSum() - valeurParMesure;
            double noteAAjouter = valeurParMesure - sum;
            compo[mesure].push_back(Note(GetNoteValueValue(noteAAjouter, vecNote[i].getNoteValue()),
                                         vecNote[i].getNoteValue(),
                                         false));
            mesure++;    // on change de mesure pour la suite
            compo.push_back(std::vector<Note>());
            while(std::find(valNoteComparasion.begin(), valNoteComparasion.end(), reste)
                  == valNoteComparasion.end())
            {
                // on regarde si la mesure suivante est pleine
                double sum = 0;
                for(int j = 0; i < compo[mesure].size(); i++)
                {
                    sum += compo[mesure][j].getNoteSum();
                }
                // on regarde si sa rentre dans la mesure
                if(sum + reste >= valeurParMesure)    // si non
                {
                    double noteAAjouter = valeurParMesure - sum;
                    compo[mesure].push_back(
                      Note(GetNoteValueValue(noteAAjouter, vecNote[i].getNoteValue()),
                           vecNote[i].getNoteValue(),
                           true));
                    mesure++;    // on change de mesure pour la suite
                    compo.push_back(std::vector<Note>());
                    reste = reste - noteAAjouter;
                }
                else    // si oui
                {
                    double noteAAjouter = 0;
                    if(reste - 2 > 0)
                    {
                        noteAAjouter = 2;
                    }
                    else if(reste - 1 > 0)
                    {
                        noteAAjouter = 1;
                    }
                    else if(reste - 0.5 > 0)
                    {
                        noteAAjouter = 0.5;
                    }
                    compo[mesure].push_back(
                      Note(GetNoteValueValue(noteAAjouter, vecNote[i].getNoteValue()),
                           vecNote[i].getNoteValue(),
                           true));
                    reste = reste - noteAAjouter;
                }
            }
            compo[mesure].push_back(Note(GetNoteValueValue(reste, vecNote[i].getNoteValue()),
                                         vecNote[i].getNoteValue(),
                                         true));
        }
    }
    //=============== Boute de code qui transforme les note en notewidget et qui affiche chaque mesure à la bonne place====================//

}

bool Partition::mesureEstPleine()
{
    return true;
}
NoteType GetNoteValueValue(double d, NoteValue t)
{
    if(t == NoteValue::UNKNOWN)
    {
        if(d == 4)
        {
            return NoteType::Ronde;
        }
        else if(d == 2)
        {
            return NoteType::Blanche;
        }
        else if(d == 1)
        {
            return NoteType::Noire;
        }
        else if(d == 0.5)
        {
            return NoteType::Croche;
        }
        else if(d == 0.25)
        {
            return NoteType::DoubleCroche;
        }
    }
    else
    {
        if(d == 4)
        {
            return NoteType::Pause;
        }
        else if(d == 2)
        {
            return NoteType::DemiPause;
        }
        else if(d == 1)
        {
            return NoteType::Silence;
        }
        else if(d == 0.5)
        {
            return NoteType::DemiSilence;
        }
        else if(d == 0.25)
        {
            return NoteType::QuartSilence;
        }
    }
}