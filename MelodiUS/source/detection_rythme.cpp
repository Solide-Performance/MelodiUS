/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "detection_rythme.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
constexpr double epsilon = 0.005;


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

    float volmax = *std::max_element(volume.cbegin(), volume.cend());
    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive_double[i], 0.0f, epsilon) && volume[i] > 0.5 * volmax)
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
                                  [&](int a) {
                                      return a < distanceMax * 0.05;
                                  }),
                   distance.end());

    for(size_t dist : distance)
    {
        std::cout << dist << '\n';
    }
    std::cout << std::endl;

    return 0;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
