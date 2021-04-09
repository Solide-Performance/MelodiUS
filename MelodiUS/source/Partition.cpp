#include "Partition.h"

Partition::~Partition()
{
}

void Partition::ajoutLigne()
{
    feuille.emplace_back(nbsLigne, PartitionGroupBox);
    nbsLigne++;
}
