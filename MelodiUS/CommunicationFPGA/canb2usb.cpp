#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#include "CommunicationFPGA/CommunicationFPGA.h"


const std::string CANB2USBCONSOLE_VERS = "V01.00a";


//============================================================================
// programme principal
//============================================================================
int main(int argc, char** argv, char** envp)

{
    // ------------------------------------------------------------------------
    // Donnees du main
    // ------------------------------------------------------------------------

    bool statutport = false;    // statut du port de communication qui sera cree


    int compteur_temps = 0;
    int swt            = 0;             // donnee recue du FPGA
    int aff7sg_octet0  = 0;             // octet 0 (droite) pour afficheur 7 segments
    int aff7sg_octet1  = 0;             // octet 0 (droite) pour afficheur 7 segments
    int stat_btn       = 0;             // donnee recue du FPGA: statut et BTN
    int echconv[4];                     // donnee converties recues du FPGA
    int canal_a_afficher        = 0;    // donnee recue du FPGA
    int indice_canal_a_afficher = 0;

    const int nitermax     = 10000;
    const int delai_boucle = 10;    // delai d'attente ajout・dans la boucle de lecture en ms

    // numeros de registres correspondants pour les echanges FPGA <-> PC  ...


    CommunicationFPGA port;    // Instance du port de communication


    int statut_circuit = 0;

    for(int niter = 0; ((niter < nitermax) && ((stat_btn & 8) == 0));
        niter++)    // tant que nitermax non atteint et que btn3 non 1
    {

        if(statutport)
            statutport =
              port.lireRegistre(nreg_lect_stat_btn, stat_btn);    // lecture statut et BTN
        else
        {
            std::cout << "Erreur du port nreg_lect_stat_btn" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport = port.lireRegistre(nreg_lect_swt, swt);    // lecture swt
        else
        {
            std::cout << "Erreur du port nreg_lect_swt" << std::endl;
            exit(1);
        }

        if(statutport)
            statutport =
              port.lireRegistre(nreg_lect_cmpt_t, compteur_temps);    // lecture compteur temps
        else
        {
            std::cout << "Erreur du port nreg_lect_cmpt_t" << std::endl;
            exit(1);
        }


        if(statutport)
            statutport = port.lireRegistre(nreg_lect_can0, echconv[0]);    // lecture canal 0
        else
        {
            std::cout << "Erreur du port nreg_lect_can0" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport = port.lireRegistre(nreg_lect_can1, echconv[1]);    // lecture canal 1
        else
        {
            std::cout << "Erreur du port nreg_lect_can1" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport = port.lireRegistre(nreg_lect_can2, echconv[2]);    // lecture canal 2
        else
        {
            std::cout << "Erreur du port nreg_lect_can2" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport = port.lireRegistre(nreg_lect_can3, echconv[3]);    // lecture canal 3
        else
        {
            std::cout << "Erreur du port nreg_lect_can3" << std::endl;
            exit(1);
        }
        if(!statutport)
        {
            std::cout << "Erreur du port dans la boucle" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport =
              port.ecrireRegistre(nreg_ecri_led, swt);    // re-envoyer lecture swt sur port LED
        else
        {
            std::cout << "Erreur du port nreg_ecri_led" << std::endl;
            exit(1);
        }
        // compose valeur pour affichage VV0N
        canal_a_afficher        = (swt & 0x3);
        indice_canal_a_afficher = (int)canal_a_afficher;


        if(statutport)
            statutport = port.ecrireRegistre(
              nreg_ecri_aff7sg0,
              canal_a_afficher);    // envoyer numero canal_a_afficher afficheur 7 segments
        else
        {
            std::cout << "Erreur du port nreg_ecri_aff7sg0" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport = port.ecrireRegistre(
              nreg_ecri_aff7sg1,
              echconv[indice_canal_a_afficher]);    // envoyer valeur echconv[] afficheur 7 segments
        else
        {
            std::cout << "Erreur du port nreg_ecri_aff7sg1" << std::endl;
            exit(1);
        }
        if(statutport)
            statutport = port.ecrireRegistre(
              nreg_ecri_aff7dot, stat_btn & 0xFF);    // envoyer vecteur  bouton (sans le statut)
        else
        {
            std::cout << "Erreur du port nreg_ecri_aff7dot" << std::endl;
            exit(1);
        }

        std::cout << "      " << (int)stat_btn << ",   " << (int)swt << ",     "
                  << (int)compteur_temps;
        indice_canal_a_afficher = 0;
        for(indice_canal_a_afficher; indice_canal_a_afficher < 4; indice_canal_a_afficher++)
            if((int)echconv[indice_canal_a_afficher] == 0)
                std::cout << ",     0x00";
            else
                std::cout << ",    " << (int)echconv[indice_canal_a_afficher];
        std::cout << std::endl;
        Sleep(delai_boucle);    // attente x ms sans occuper de CPU ...
    }

    std::cout << std::endl << "--------------------------------------------------" << std::endl;


    std::cout << "--fin. " << std::endl;
    return 0;    // fin normale
}
