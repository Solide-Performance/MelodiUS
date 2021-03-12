#include "detection_rythme.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

constexpr double epsilon = 0.005;

/* Function definitions ---------------------------------------------------- */
std::vector<float> Generate_Sine(size_t freq,
                                 size_t numSeconds,
                                 size_t sampleRate,
                                 size_t framesPerBuffer,
                                 size_t numChannels,
                                 float  amplitude)
{
    (void)framesPerBuffer;

    size_t size   = numSeconds * sampleRate * numChannels;
    double cycles = static_cast<double>(numSeconds) / (1. / static_cast<double>(freq));

    std::vector<float> data(size);

    /* initialise sinusoidal wavetable */
    for(size_t i = 0; i < size; i++)
    {
        data[i] = static_cast<float>(amplitude * sin((cycles / size) * 3.1415 * 2. * i));
    }

    return data;
}


int analyse_rythme(const Recording& rec)
{
    const std::vector<float>& tableau = rec.getSamples();

    // std::vector<float> tableau = Generate_Sine(10, 1, 100, 1, 1, 2);

    size_t             taille = tableau.size();
    std::vector<float> derive(taille);
    std::vector<float> volume(taille);
    float              maximum = 0;
    std::vector<float> derive_double(taille);
    std::vector<bool>  attaque(taille);
    std::vector<int>   distance(taille);

    int note = 0;
    int j    = 0;

    for(size_t i = 0; i < taille; i++)
    {
        // std::cout << tableau[i] << std::endl;
    }
    // std::cout << std::endl;

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive[i] = tableau[i + 1] - tableau[i];
        // std::cout << derive[i] << std::endl;
    }
    // std::cout << std::endl;



    std::ofstream s("volume.txt");
    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive[i], 0.0f, epsilon) && tableau[i] > 0)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            maximum = tableau[i];
        }
        volume[i] = maximum;
        s << volume[i] << '\n';
        // std::cout << volume[i] << std::endl;
    }
    s << std::endl;
    s.close();
    // std::cout << std::endl;

    s.open("derive_double.txt");
    for(size_t i = 0; i < taille - 1; i++)
    {
        derive_double[i] = volume[i + 1] - volume[i];
        s << derive_double[i] << '\n';
    }
    s << std::endl;
    s.close();

    float volmax = *std::max_element(volume.cbegin(), volume.cend());
    s.open("attaques.txt");
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
        s << attaque[i] << '\n';
        // std::cout << attaque[i] << std::endl;
    }
    s << std::endl;
    // std::cout << std::endl;

    for(size_t i = 0; i < taille - 1; i++)
    {
        if(attaque[i] == 1)    // Lire a partir de la sampling rate?
        {
            distance[j] = i - note;
            note        = i;
            j++;
        }
    }
    int distanceMax = *std::max_element(distance.begin(), distance.end());
    distance.erase(std::remove_if(distance.begin(),
                                  distance.end(),
                                  [&](int a) {
                                      return a < distanceMax * 0.05;
                                  }),
                   distance.end());

    for(int dist : distance)
    {
        std::cout << dist << '\n';
    }
    std::cout << std::endl;

    s.close();
    return 0;
}
