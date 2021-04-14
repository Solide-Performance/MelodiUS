#include "Partition.h"
NoteType GetNoteValueValue(double d, NoteValue t);

Partition::~Partition()
{
}

int Partition::ajoutLigne()
{
    feuille.push_back(Portee(nbsLigne,PartitionGroupBox));
    nbsLigne++;
    spinBox.raise();
    spinBox_2.raise();
    return nbsLigne;
}

void Partition::ecrireMusique(std::vector<Note> vecNote)
{
    nom = spinBox.value();
    denom = spinBox.value();
    int ligne = 0;
    int mesure = 0;
    std::vector<std::vector<Note>> compo;
    compo.push_back(std::vector<Note>());
    std::array < double, 5> valNoteComparasion= {4, 2, 1, 0.5, 0.25};
    if((nom != 1 && nom != 2 && nom != 4 && nom != 8 && nom != 16)
       && (denom != 1 && denom != 2 && denom != 3 && denom != 4 && denom != 5 && denom != 6
             && denom != 7 && denom != 8))
    {
        //QErrorMessage() n(&PartitionGroupBox);
    }
    int valeurParMesure = nom * (4 / denom);


    //=============== boute de code qui ce repete pour chaque note du vecteur============//
    for(int i = 0; i < vecNote.size(); i++)
    {
        //On commence par verifier si la mesure est pleine et si oui on rajoute une mesuere
        double sum = 0;
        for(int j = 0; i < compo[mesure].size(); i++)
        {
            sum += compo[mesure][j].getNoteSum();
        }
        if(sum >= valeurParMesure)
        {
            if(mesure % 4 == 0)
            {
                mesure++;
                compo.push_back(std::vector<Note>());
            }
            else
            {
                mesure++;
                compo.push_back(std::vector<Note>());
            }
        }
        //============================================================================
        // On regarde maintenant si la note rentre telle quelle dans le vecteur
        if(sum + vecNote[i].getNoteSum() <= valeurParMesure)
        {
            compo[mesure].push_back(Note(vecNote[i]));
        }
        else
        {
            double reste = sum + vecNote[i].getNoteSum()-valeurParMesure;
            double noteAAjouter=valeurParMesure-sum;
            compo[mesure].push_back(Note(GetNoteValueValue(noteAAjouter,vecNote[i].getNoteValue()), vecNote[i].getNoteValue(), true));
            while(std::find(valNoteComparasion.begin(), valNoteComparasion.end(), reste)
                  == valNoteComparasion.end())
            {

            }
        }






    }

}

bool Partition::mesureEstPleine()
{
    return true;
}
NoteType GetNoteValueValue(double d, NoteValue t)
{
    if(t == NoteValue::UNKNOWN)
    {
        if(d==4)
        {
            return NoteType::Ronde;
        }
        else if(d==2)
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