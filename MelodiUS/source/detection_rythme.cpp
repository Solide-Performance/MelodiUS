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
constexpr double epsilon = 0.005;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
std::vector<Recording> analyse_rythme(const Recording& rec)
{
    const size_t       dt            = rec.getSampleRate() / 20;
    const size_t       sample_cutoff = rec.getSampleRate() / 5;
    std::vector<float> tableau       = rec.getSamples();
    size_t             taille        = tableau.size();

    std::vector<float>  derive(taille);
    std::vector<float>  volume(taille);
    std::vector<float>  volume_plat(taille);
    std::vector<float>  derive_double(taille);
    std::vector<size_t> debut_note{};
    std::vector<size_t> fin_note{};
    float               maximum      = 0;
    const float         marge_bruit  = 0.0005;
    const float         marge_volume = 0.005;
    const float         marge_note   = 0.05f;

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive[i] = tableau[i + 1] - tableau[i];
    }

    float volmax = *std::max_element(volume.cbegin(), volume.cend());
    float volmin = /* 0.0316*/ 0.1 * volmax;
    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive[i], 0.0f, epsilon) && tableau[i] > 0 /*marge_volume*/)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            maximum = tableau[i];
        }
        volume[i] = maximum;
    }

    for(size_t i = 0; i < taille - 1; i++)
    {
        if(volume[i] < marge_bruit)
        {
            volume[i] = 0;
        }
    }

    for(int i = 0; i < taille; i += dt)
    {
        float volume_moyen = 0.f;

        size_t max = std::min(i + dt, volume.size() - 1);
        for(int j = i; j < max; j++)
        {
            volume_moyen += volume[j];
        }
        volume_moyen /= dt;
        std::fill(&volume_plat[i], &volume_plat[max], volume_moyen);
    }

    int compteur_debut = 0;
    for(size_t i = 0; i < taille - 1; i++)
    {
        derive_double[i] = volume_plat[i + 1] - volume_plat[i];
        if(derive_double[i] > marge_note)
        {
            debut_note.push_back(i);
        }
    }

    for(size_t i = 0; i < debut_note.size() - 1; i++)
    {
        size_t sampleLength = debut_note[i + 1] - debut_note[i];
        if(sampleLength < sample_cutoff)
        {
            // Removes note
            debut_note.erase(debut_note.begin() + i);
        }
    }

    for(size_t note = 0; note < debut_note.size(); note++)
    {
        float valeurAttaque = volume_plat[debut_note[note]];

        size_t max = note == debut_note.size() - 1 ? volume.size() - 1 : debut_note[note + 1];
        size_t i   = debut_note[note];
        for(; i < max; i += dt)
        {
            float currentValue = volume_plat[i];
            if(currentValue < 0.3f * valeurAttaque)
            {
                break;
            }
        }
        fin_note.push_back(std::min(i, volume_plat.size() - 1));

    }

    /*std::ofstream f{"test.txt"};
    for(int i = 0; i < taille; i++)
    {
        bool valDebut = false;
        bool valFin   = false;
        if(std::find(debut_note.begin(), debut_note.end(), i) != debut_note.end())
        {
            valDebut = true;
        }
        if(std::find(fin_note.begin(), fin_note.end(), i) != fin_note.end())
        {
            valFin = true;
        }
        f << i << '\t' << volume[i] << '\t' << volume_plat[i] << '\t' << derive_double[i] << '\t'
          << valDebut << '\t' << valFin << '\n';
    }
    f.close();*/


    /*for(size_t i = 0; i < debut_note.size(); i++)
    {
        if(debut_note[i] != 0)
        {
            std::cout << debut_note[i] << " | " << fin_note[i] << "\t | "
                      << (fin_note[i] - debut_note[i]) << '\n';
        }
    }*/

    std::vector<Recording> notes(debut_note.size());
    for(size_t i = 0; i < notes.size(); i++)
    {
        const float* beginIt = &tableau[debut_note[i]];
        const float* endIt   = &tableau[fin_note[i]];

        notes[i] = Recording{
          beginIt, endIt, rec.getSampleRate(), rec.getFramesPerBuffer(), rec.getNumChannels()};

        double freq = FindFrequency(notes[i]);
        std::cout << "Note " << i + 1 << " : " << freq << "Hz (" << FindNoteFromFreq(freq)
                  << ")\tSamples: " << debut_note[i] << " to " << fin_note[i] << "("
                  << fin_note[i] - debut_note[i] << ")" << std::endl;
    }
    std::cout << std::endl;

    return notes;

}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
