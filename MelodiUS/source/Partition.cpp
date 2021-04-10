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