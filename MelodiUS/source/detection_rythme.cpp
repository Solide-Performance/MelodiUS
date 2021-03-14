/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "detection_rythme.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr double epsilon       = 0.005;
constexpr size_t MARGE_moment  = 1000;
constexpr size_t MARGE_attaque = 200;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
int analyse_rythme(const Recording& rec)
{
    const std::vector<float>& tableau = rec.getSamples();
    size_t                    taille  = tableau.size();

    std::vector<float>  derive(taille);
    std::vector<float>  volume(taille);
    float               maximum = 0;
    std::vector<float>  derive_double(taille);
    std::vector<bool>   attaque(taille);
    std::vector<size_t> distance(taille);


    for(size_t i = 0; i < taille - 1; i++)
    {
        derive[i] = tableau[i + 1] - tableau[i];
    }

    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive[i], 0.0f, epsilon) && tableau[i] > 0)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            maximum = tableau[i];
        }
        volume[i] = maximum;
    }

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive_double[i] = volume[i + 1] - volume[i];
    }

    // const int AVANT=20;
    float volmax = *std::max_element(volume.cbegin(), volume.cend());
    // float derive_doublemax = *std::max_element(derive_double.cbegin(), derive_double.cend());
    // std::cout << derive_doublemax << std::endl;
    for(size_t i = 0; i < taille - 1; i++)
    {
        // float pente = std::accumulate(derive_double.begin()+i-AVANT,
        // derive_double.begin()+i,0.0f)/AVANT; std::cout << pente << std::endl;
        if(COMPARE_FLOATS(derive_double[i], 0.0f, epsilon)
           && volume[i] > 0.5 * volmax /*&& pente>derive_doublemax*0.3*/)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            attaque[i] = true;
        }
        else
        {
            attaque[i] = false;
        }
    }


    size_t note = 0;
    size_t j    = 0;
    for(size_t i = 0; i < taille - 1; i++)
    {
        if(attaque[i] == 1)
        {
            distance[j] = i - note;
            note        = i;
            j++;
        }
    }
    size_t distanceMax = *std::max_element(distance.begin(), distance.end());
    distance.erase(std::remove_if(distance.begin(),
                                  distance.end(),
                                  [&](size_t a) {
                                      return a < distanceMax * 0.05;
                                  }),
                   distance.end());

    for(size_t dist : distance)
    {
        std::cout << dist << '\n';
    }
    std::vector<size_t> index_debut(distance.size());
    std::cout << std::endl;
    for(size_t i = 0; i < distance.size(); i++)
    {
        index_debut[i] = std::accumulate(distance.begin(), distance.begin() + i + 1, 0);
    }

    for(size_t debut : index_debut)
    {
        std::cout << debut << '\n';
    }

    std::cout << std::endl;
    std::vector<size_t> index_fin(distance.size());
    for(size_t i = 0; i < index_debut.size() - 1; i++)
    {
        size_t compteur       = index_debut[i];
        float  volume_attaque = std::accumulate(
          &volume[index_debut[i] - MARGE_attaque], &volume[index_debut[i] + MARGE_attaque], 0.0f);
        std::cout << volume_attaque << std::endl;
        for(; compteur < index_debut[i + 1]; compteur++)
        {

            float volume_moment = std::accumulate(
              &volume[compteur - MARGE_moment], &volume[compteur + MARGE_moment], 0.0f);

            if(volume_moment < 0.3 * volume_attaque)
            {
                break;
            }
        }
        index_fin[i] = compteur;
    }
    float volume_attaque =
      std::accumulate(&volume[index_debut[index_debut.size() - 1] - MARGE_attaque],
                      &volume[index_debut[index_debut.size() - 1] + MARGE_attaque],
                      0.0f);
    std::cout << volume_attaque << std::endl;
    size_t compteur = index_debut[index_debut.size() - 1];
    for(; compteur < volume.size(); compteur++)
    {

        float volume_moment =
          std::accumulate(&volume[compteur - MARGE_moment], &volume[compteur + MARGE_moment], 0.0f);

        if(volume_moment < 0.3 * volume_attaque)
        {
            break;
        }
    }
    index_fin[index_debut.size() - 1] = compteur;

    for(size_t fin : index_fin)
    {
        std::cout << fin << '\n';
    }
    return 0;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
