#include "Partition.h"


int Partition::ajoutLigne()
{
    feuille.push_back(Portee(nbsLigne, m_parent));
    nbsLigne++;
    spinBox.raise();
    spinBox_2.raise();
    return nbsLigne;
}
