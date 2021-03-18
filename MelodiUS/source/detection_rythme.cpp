/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "detection_rythme.h"
#include "globaldef.h"

#include "fft.h"


#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifndef LINUX_
#include <immintrin.h>
#include <xmmintrin.h>
#endif


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr double epsilon       = 0.005;
constexpr size_t SAMPLE_CUTOFF = 3000;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
int analyse_rythme(const Recording& rec)
{
    const size_t       dt      = rec.getSampleRate() / 3;
    std::vector<float> tableau = rec.getSamples();
    size_t             taille  = tableau.size();

    std::vector<float>  derive(taille);
    std::vector<float>  volume(taille);
    std::vector<float>  volume_plat(taille);
    std::vector<float>  derive_double(taille);
    std::vector<int>  debut_note(taille);
    std::vector<int>  fin_note(taille);
    float               maximum = 0;
    const float marge_bruit=0.0005;
    const float marge_volume=0.005;
    const float marge_note

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive[i] = tableau[i + 1] - tableau[i];
    }

    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive[i], 0.0f, epsilon) && tableau[i] > /*marge_volume*/ )
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            maximum = tableau[i];
        }
        volume[i] = maximum;
    }

    for(size_t i = 0; i< taille; i++)
    {
        if(volume[i]<marge_bruit)
        {
            volume[i]=0;
        }
    }
    
    float  volume_moyen = 0.f;
    for(int i=0; i<taille; i+=dt)
    {
        volume_moyen=0;
        
    
        for(int j=0 ; j < std::min(i + dt, volume.size()); j++)
        {
                volume_moyen += volume[j];
        }
        volume_moyen /= dt;
        std::fill(&volume_plat[i], &volume_plat[i+dt], volume_moyen);

    }
    int compteur_debut=0;
    for(size_t i = 0; i < taille - 1; i++)
    {
        derive_double[i] = volume_plat[i + 1] - volume_plat[i];
        if(derive_double[i]>marge_note)
        {
            debut_note[compteur_fin]=i;
        }
    }
    
    
    
    
    
    
    
    
    return 0;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
