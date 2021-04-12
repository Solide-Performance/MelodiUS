/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "detection_rythme.h"
#include "fft.h"
#include "globaldef.h"
#include "note.h"
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
/* Function declarations --------------------------------------------------- */
void correct_fuckaroos(std::vector<size_t>&    debuts,
                       std::vector<size_t>&    fins,
                       std::vector<NoteValue>& notes,
                       std::vector<Recording>& recs,
                       size_t                  dt);

std::vector<Note> analyse_note(const std::vector<size_t>&    debuts,
                               const std::vector<size_t>&    fins,
                               size_t                        recordingLength,
                               const std::vector<NoteValue>& valeur_note);


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
std::vector<Recording> analyse_rythme(const Recording& rec)
{
    const size_t       dt            = rec.getSampleRate() / 9;
    size_t             sample_cutoff = rec.getSampleRate() / 5;
    std::vector<float> tableau       = rec.getSamples();
    size_t             taille        = tableau.size();

    std::vector<float>  derive(taille);
    std::vector<float>  volume(taille);
    std::vector<float>  volume_plat(taille);
    std::vector<float>  derive_double(taille);
    std::vector<size_t> debut_note{};
    std::vector<size_t> fin_note{};
    float               maximum     = 0.f;
    const float         marge_bruit = 0.0005f;
    const float         marge_note  = 0.003f;

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive[i] = tableau[i + 1] - tableau[i];
    }

    float volmax = *std::max_element(volume.cbegin(), volume.cend());
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

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive_double[i] = volume_plat[i + 1] - volume_plat[i];
        if(derive_double[i] > marge_note)
        {
            debut_note.push_back(i);
        }
    }
    if(debut_note.size() == 0)
    {
        return {{}};
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


    /*std::ofstream debugFile{"debug.txt"};
    for(int i = 0; i < volume.size(); i++)
    {
        debugFile << i << '\t';
        debugFile << volume[i] << '\t';
        debugFile << volume_plat[i] << '\t';

        if(std::find(debut_note.begin(), debut_note.end(), i) != debut_note.end())
        {
            debugFile << 1 << '\t';
        }
        else
        {
            debugFile << 0 << '\t';
        }
        if(std::find(fin_note.begin(), fin_note.end(), i) != fin_note.end())
        {
            debugFile << 1 << '\t';
        }
        else
        {
            debugFile << 0 << '\t';
        }
        debugFile << std::endl;
    }
    debugFile.close();*/



    std::cout << std::endl;
    std::vector<NoteValue> valeur_note(debut_note.size());
    std::vector<Recording> notes(debut_note.size());

    for(size_t i = 0; i < notes.size(); i++)
    {
        const float* beginIt = &tableau[debut_note[i]];
        const float* endIt   = &tableau[fin_note[i]];

        notes[i] = Recording{
          beginIt, endIt, rec.getSampleRate(), rec.getFramesPerBuffer(), rec.getNumChannels()};

        double freq = FindFrequency(notes[i]);

        auto [str, val] = FindNoteFromFreq(freq);
        valeur_note[i]  = val;
    }
    std::cout << std::endl << std::endl;


    correct_fuckaroos(debut_note, fin_note, valeur_note, notes, dt);

    for(size_t i = 0; i < notes.size(); i++)
    {
        double freq = FindFrequency(notes[i]);

        auto [str, val] = FindNoteFromFreq(freq);
        std::cout << "Note " << i + 1 << " : " << freq << "Hz (" << str
                  << ")\tSamples: " << debut_note[i] << " to " << fin_note[i] << "("
                  << fin_note[i] - debut_note[i] << ")" << std::endl;
    }

    analyse_note(debut_note, fin_note, volume_plat.size(), valeur_note);

    return notes;
}

void correct_fuckaroos(std::vector<size_t>&    debuts,
                       std::vector<size_t>&    fins,
                       std::vector<NoteValue>& notes,
                       std::vector<Recording>& recs,
                       size_t                  dt)
{
    std::vector<size_t> durees;
    for(size_t i = 0; i < debuts.size(); i++)
    {
        size_t duree = fins[i] - debuts[i];

        if(duree <= 2 * dt)
        {
            if(i != 0 && i != notes.size() - 1)
            {
                if(debuts[i] == fins[i - 1] && fins[i] == debuts[i + 1])
                {
                    if(fins[i - 1] - debuts[i - 1] < fins[i + 1] - debuts[i + 1])
                    {
                        fins.erase(fins.begin() + i - 1);
                        debuts.erase(debuts.begin() + i);
                        notes.erase(notes.begin() + i);
                        recs.erase(recs.begin() + i);
                        continue;
                    }
                    else
                    {
                        fins.erase(fins.begin() + i);
                        debuts.erase(debuts.begin() + i + 1);
                        notes.erase(notes.begin() + i);
                        recs.erase(recs.begin() + i);
                        continue;
                    }
                }
                else if(debuts[i] == fins[i - 1])
                {
                    fins.erase(fins.begin() + i - 1);
                    debuts.erase(debuts.begin() + i);
                    notes.erase(notes.begin() + i);
                    recs.erase(recs.begin() + i);
                    continue;
                }
                else if(fins[i] == debuts[i + 1])
                {
                    fins.erase(fins.begin() + i);
                    debuts.erase(debuts.begin() + i + 1);
                    notes.erase(notes.begin() + i);
                    recs.erase(recs.begin() + i);
                    continue;
                }
            }
        }
    }
}

std::vector<Note> analyse_note(const std::vector<size_t>&    debuts,
                               const std::vector<size_t>&    fins,
                               size_t                        recordingLength,
                               const std::vector<NoteValue>& valeur_note)
{
    std::vector<int64_t> liste_duree;
    std::vector<int64_t> liste_ratios;

    /* Garder la duree des notes et des silences.
     * Chaque note est associee a un silence (meme s'il fait 0 de longueur). */
    for(int64_t i = 0; i < debuts.size(); i++)
    {
        int64_t noteLength = fins[i] - debuts[i];

        int64_t maxSilenceLength = i + 1 >= debuts.size() ? recordingLength : debuts[i + 1];
        int64_t silenceLength    = maxSilenceLength - fins[i];

        liste_duree.push_back(noteLength);
        liste_duree.push_back(silenceLength);
    }

    /* Create groups of note lengths */
    const int64_t maxLength =
      *std::max_element(liste_duree.begin(), liste_duree.end(), std::less());

    const std::vector<int64_t> fractions = {
      maxLength, maxLength / 2, maxLength / 4, maxLength / 8, maxLength / 16, 0};

    for(int64_t noteLength : liste_duree)
    {
        /* Set current min value to infinity */
        int64_t currentMin = std::numeric_limits<int64_t>::max();

        for(int64_t groupLength : fractions)
        {
            /* Update current minimum value */
            int64_t val = std::abs((int64_t)groupLength - (int64_t)noteLength);
            currentMin  = std::min(currentMin, val);
        }

        /* Find where the min was */
        for(int i = 0; i < fractions.size(); i++)
        {
            int64_t currentFraction = fractions[i];
            int64_t upperVal        = currentMin + noteLength;
            int64_t lowerVal        = abs(currentMin - noteLength);
            if(currentFraction == upperVal || currentFraction == lowerVal)
            {
                /* Append index of ratio */
                liste_ratios.push_back(i);
                break;
            }
        }
    }

    /* Get lowest layer of fraction */
    /* clang-format off */
    int64_t max_ratio = *std::max_element(liste_ratios.begin(),
                                          liste_ratios.end(),
                                          [rejected = fractions.size() - 1](int64_t greatest, int64_t val)
                                          {
                                              return val == rejected ? false : greatest < val;
                                          });
    /* clang-format on */

    std::vector<Note> liste_symbole;
    if(max_ratio == 0)
    {
        int j = 0;
        for(int i = 0; i < liste_ratios.size(); i++)
        {
            if(i % 2 == 0)
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, valeur_note[j++]));
                }
            }
            else
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Silence, NoteValue::UNKNOWN));
                }
            }
        }
    }
    else if(max_ratio == 1)
    {
        int j = 0;
        for(int i = 0; i < liste_ratios.size(); i++)
        {
            if(i % 2 == 0)
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, valeur_note[j++]));
                }
            }
            else
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Silence, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::DemiSilence, NoteValue::UNKNOWN));
                }
            }
        }
    }
    else if(max_ratio == 2)
    {
        int j = 0;
        for(int i = 0; i < liste_ratios.size(); i++)
        {
            if(i % 2 == 0)
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Blanche, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, valeur_note[j++]));
                }
            }
            else
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::DemiPause, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Silence, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::DemiSilence, NoteValue::UNKNOWN));
                }
            }
        }
    }
    else if(max_ratio == 3)
    {
        int j = 0;
        for(int i = 0; i < liste_ratios.size(); i++)
        {
            if(i % 2 == 0)
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Ronde, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Blanche, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 3)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, valeur_note[j++]));
                }
            }
            else
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Pause, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::DemiPause, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Silence, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 3)
                {
                    liste_symbole.push_back(Note(NoteType::DemiSilence, NoteValue::UNKNOWN));
                }
            }
        }
    }
    else if(max_ratio == 4)
    {
        int j = 0;
        for(int i = 0; i < liste_ratios.size(); i++)
        {
            if(i % 2 == 0)
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Ronde, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Blanche, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 3)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, valeur_note[j++]));
                }
                else if(liste_ratios[i] == 4)
                {
                    liste_symbole.push_back(Note(NoteType::DoubleCroche, valeur_note[j++]));
                }
            }
            else
            {

                if(liste_ratios[i] == 0)
                {
                    liste_symbole.push_back(Note(NoteType::Pause, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::DemiPause, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Silence, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 3)
                {
                    liste_symbole.push_back(Note(NoteType::DemiSilence, NoteValue::UNKNOWN));
                }
                else if(liste_ratios[i] == 4)
                {
                    liste_symbole.push_back(Note(NoteType::QuartSilence, NoteValue::UNKNOWN));
                }
            }
        }
    }
    // easter egg
    else
    {
        std::cout << "you just broke the fourth wall" << std::endl;
    }
    /* clang-format off */
    std::cout << max_ratio << std::endl;
    for(int64_t i = 0; i < liste_duree.size(); i++)
    {
        int64_t duree = liste_duree[i];
        int64_t ratio = liste_ratios[i];
        std::cout << duree << '\t' << ratio << '\n';
    }
    for(int64_t i = 0; i < liste_symbole.size(); i++)
    {
        std::cout << (int)liste_symbole[i].noteType << '\t' << (int)liste_symbole[i].noteValue << '\n';
    }
    std::cout << std::endl;

    return liste_symbole;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
