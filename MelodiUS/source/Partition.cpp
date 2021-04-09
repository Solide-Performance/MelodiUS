#include "Partition.h"

Partition::~Partition()
{
}

void Partition::ajoutLigne(int index)
{
    feuille.push_back(Portee(index, PartitionGroupBox));
}
