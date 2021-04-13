#include "Partition.h"

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
    if((nom != 1 && nom != 2 && nom != 4 && nom != 8 && nom != 16)
       && (denom != 1 && denom != 2 && denom != 3 && denom != 4 && denom != 5 && denom != 6
             && denom != 7 && denom != 8))
    {
        //QErrorMessage() n(&PartitionGroupBox);
    }
    int valeurParMesure = nom * (4 / denom);
    for(int i = 0; i < vecNote.size(); i++)
    {
        if(!compo.empty())
        {
            if(!compo[mesure].empty())
            {
                double sum = 0;
                for(int i = 0; i < compo[mesure].size(); i++)
                {
                    sum += compo[mesure][i].getNoteSum();
                }
                if(sum >= valeurParMesure)
                {
                    if(mesure % 4 == 0)
                    {
                        ligne++;
                        mesure++;
                    }
                    else
                    {
                        mesure++;
                    }
                }
            }
        }
       
    }

}
bool Partition::mesureEstPleine()
{
    return true;
}
