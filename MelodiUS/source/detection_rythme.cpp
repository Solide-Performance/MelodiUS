/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "detection_rythme.h"
#include "globaldef.h"

#include "fft.h"
#include "tuning.h"

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
std::vector<Recording> analyse_rythme(const Recording& rec)
{
    const size_t       dt      = rec.getSampleRate() / 3;
    std::vector<float> tableau = rec.getSamples();
    size_t             taille  = tableau.size();

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

    float volmax = *std::max_element(volume.cbegin(), volume.cend());
    float volmin = /* 0.0316*/ 0.1 * volmax;
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


    // Genocide

    for(auto itTab = tableau.begin(), const itVol = volume.begin();
        itVol < volume.end() && itTab < tableau.end();
        itTab++, itVol++)
    {
        if(*itVol < volmin)
        {
            *itTab = 0.f;
        }
    }

    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive_double[i], 0.0f, epsilon) /*&& pente>derive_doublemax*0.3*/)
        {
            if(volume[i] > 0.5 * volmax)
            {
                attaque[i] = true;
            }
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


    std::vector<size_t> index_debut(distance.size());
    for(size_t i = 0; i < distance.size(); i++)
    {
        index_debut[i] = std::accumulate(distance.begin(), distance.begin() + i + 1, 0);
    }

    std::vector<size_t> index_fin(distance.size());
    // std::vector<float>  tousLesVolumes(volume.size(), 0.f);
    for(size_t i = 0; i < index_debut.size(); i++)
    {
        float  volume_attaque = 0.0f;
        size_t compteur       = index_debut[i];
        size_t max = i == index_debut.size() - 1 ? volume.size() - 1 : index_debut[i + 1];
        for(; compteur < max; compteur += dt)
        {
            float  volumeMoyen = 0.f;
            size_t j           = compteur;
            for(; j < std::min(compteur + dt, volume.size()); j++)
            {
                volumeMoyen += volume[j];
            }
            volumeMoyen /= dt;

            // std::fill(&tousLesVolumes[compteur], &tousLesVolumes[j], volumeMoyen);

            if(compteur == index_debut[i])
            {
                volume_attaque = volumeMoyen;
            }
            else if(volumeMoyen <= volume_attaque * 0.33)
            {
                break;
            }
        }
        index_fin[i] = std::min(compteur, max);
    }

    /*for(size_t debut : index_debut)
    {
        std::cout << debut << '\n';
    }
    for(size_t fin : index_fin)
    {
        std::cout << fin << '\n';
    }*/

    for(size_t i = 0; i < index_debut.size(); i++)
    {
        size_t sampleLength = index_fin[i] - index_debut[i];
        if (sampleLength < SAMPLE_CUTOFF)
        {
            if(i < index_debut.size() - 1)
            {
                // Removes begin of next note
                index_debut.erase(index_debut.begin() + i + 1);
                index_fin.erase(index_fin.begin() + i);
            }
            else
            {
                // Remove current note
                index_debut.erase(index_debut.begin() + i);
                index_fin.erase(index_fin.begin() + i);
            }
        }
    }


    /*std::ofstream f("fichier.txt");
    for(size_t i = 0; i < taille; i++)
    {
        f << i << '\t' << volume[i] << '\t' << tousLesVolumes[i] << '\t' << attaque[i] << '\t'
          << defense[i] << '\n';
    }
    f.close();*/

    std::cout << std::endl << "Frequencies:" << std::endl;

    std::vector<Recording> notes(index_debut.size());
    for(size_t i = 0; i < notes.size(); i++)
    {
        const float* beginIt = &tableau[index_debut[i]];
        const float* endIt   = &tableau[index_fin[i]];

        notes[i] = Recording{
          beginIt, endIt, rec.getSampleRate(), rec.getFramesPerBuffer(), rec.getNumChannels()};

        double freq = FindFrequency(notes[i]);
        std::cout << "Note " << i + 1 << " : " << freq << "Hz (" << FindNoteFromFreq(freq)
                  << ")\tSamples: " << index_debut[i] << " to " << index_fin[i] << "("
                  << index_fin[i] - index_debut[i] << ")" << std::endl;
    }

    return notes;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
