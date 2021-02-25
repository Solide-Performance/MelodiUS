/*
 *   CommunicationFPGA est une classe d'interface à la pour la communication
 *   avec la carte FPGA de la session S2. Pour les details de l'utilisation
 *   de cette classe, consultez le guide d'utilisation.
 *
 *   $Author: bruc2001 $
 *   $Date: 2018-02-16 14:17:45 -0500 (ven., 16 févr. 2018) $
 *   $Revision: 135 $
 *   $Id: CommunicationFPGA.h 135 2018-02-16 19:17:45Z bruc2001 $
 *
 *   Copyright 2013 Département de génie électrique et génie informatique
 *                  Université de Sherbrooke
 */

#ifndef COMMUNICATIONFPGA_H
#define COMMUNICATIONFPGA_H

class CommunicationFPGA
{
public:
    CommunicationFPGA();
    ~CommunicationFPGA();

    bool lireRegistre(int registre, int& valeur);
    bool ecrireRegistre(int registre, int valeur);
    void sleep(unsigned int millisecondes = 10);

    bool        estOk();
    const char* messageErreur();

private:
    bool          erreur;
    char          msgErreur[1024];
    unsigned long hif;
};

#endif
