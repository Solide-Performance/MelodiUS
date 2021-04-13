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
/* Defines ----------------------------------------------------------------- */
#define READ_TWICE true


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr double epsilon = 0.005;


/*****************************************************************************/
/* Types ------------------------------------------------------------------- */
struct NotesPacket
{
    std::vector<size_t>      debut_note;
    std::vector<size_t>      fin_note;
    std::vector<NoteValue>   notes;
    std::vector<std::string> noteNames;
};


/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
void filePrinter(const std::vector<float>& volume,
                 const std::vector<float>& volume_plat,
                 const NotesPacket&        np);

void correct_fuckaroos(NotesPacket& np, size_t dt);

std::vector<Note> analyse_note(const NotesPacket& np, size_t recordingLength);

NotesPacket analyse_rythme_impl(std::vector<float> volume_plat,
                                const Recording&   rec,
                                size_t             dt,
                                size_t             sampleCutoff,
                                float              marge_note);


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
std::vector<Recording> analyse_rythme(const Recording& rec)
{
    const size_t dt            = rec.getSampleRate() / 9;
    size_t       sample_cutoff = rec.getSampleRate() / 5;
    size_t       taille        = rec.getNumSamples();

    std::vector<float> derive(taille);
    std::vector<float> volume(taille);
    std::vector<float> volume_plat(taille);
    float              maximum     = 0.f;
    const float        marge_bruit = 0.0005f;
    const float        marge_note  = 0.003f;

    for(size_t i = 0; i < taille - 1; i++)
    {
        derive[i] = rec[i + 1] - rec[i];
    }

    float volmax = *std::max_element(volume.cbegin(), volume.cend());
    for(size_t i = 0; i < taille - 1; i++)
    {
        if(COMPARE_FLOATS(derive[i], 0.0f, epsilon) && rec[i] > 0 /*marge_volume*/)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            maximum = rec[i];
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

    NotesPacket np1 = analyse_rythme_impl(volume_plat, rec, dt, sample_cutoff, marge_note);

#if !READ_TWICE
    NotesPacket np = np1;
#else
    NotesPacket np2 = analyse_rythme_impl(volume_plat, rec, dt, sample_cutoff, marge_note * 0.6);

    NotesPacket np;

    for(size_t i = 0; i < np1.notes.size(); i++)
    {
        size_t debut = np1.debut_note[i];
        size_t fin   = np1.fin_note[i];

        /* clang-format off */
        auto debutMatch = std::find_if(np2.debut_note.begin(), np2.debut_note.end(),
                                       [dt, debut](size_t pos)
                                       {
                                            return (pos - dt * 2 <= debut) && (pos + dt * 2 >= debut);
                                       });
        auto finMatch = std::find_if(np2.fin_note.begin(), np2.fin_note.end(),
                                     [dt, fin](size_t pos)
                                     {
                                          return (pos - dt * 2 <= fin) && (pos + dt * 2 >= fin);
                                     });

        if (debutMatch == np2.debut_note.end())
        {
            debutMatch = np2.debut_note.begin() + i;
        }
        size_t distanceDebut = debutMatch - np2.debut_note.begin();
        if (finMatch == np2.fin_note.end())
        {
            finMatch = np2.fin_note.begin() + distanceDebut;
        }
        size_t distanceFin = finMatch - np2.fin_note.begin();
        /* clang-format on */

        size_t diff = distanceFin - distanceDebut;
        if(diff > 0)
        {
            bool isNoteSame = true;
            for(size_t j = distanceDebut; j <= distanceFin; j++)
            {
                if(np1.notes[i] == np2.notes[j])
                {
                    continue;
                }
                else
                {
                    isNoteSame = false;
                    break;
                }
            }

            if(isNoteSame == true)
            {
                np.notes.push_back(np1.notes[i]);
                np.noteNames.push_back(np1.noteNames[i]);
                np.debut_note.push_back(np1.debut_note[i]);
                np.fin_note.push_back(np1.fin_note[i]);
            }
            else
            {
                np.notes.insert(np.notes.end(),
                                np2.notes.begin() + distanceDebut,
                                np2.notes.begin() + distanceFin + 1);
                np.noteNames.insert(np.noteNames.end(),
                                    np2.noteNames.begin() + distanceDebut,
                                    np2.noteNames.begin() + distanceFin + 1);
                np.debut_note.insert(np.debut_note.end(),
                                     np2.debut_note.begin() + distanceDebut,
                                     np2.debut_note.begin() + distanceFin + 1);
                np.fin_note.insert(np.fin_note.end(),
                                   np2.fin_note.begin() + distanceDebut,
                                   np2.fin_note.begin() + distanceFin + 1);
            }
        }
        else
        {
            np.notes.push_back(np1.notes[i]);
            np.noteNames.push_back(np1.noteNames[i]);
            np.debut_note.push_back(np1.debut_note[i]);
            np.fin_note.push_back(np1.fin_note[i]);
        }
    }
#endif


    std::cout << std::endl;
    for(size_t i = 0; i < np.notes.size(); i++)
    {
        std::cout << "Note " << i + 1 << " : "
                  << "(" << np.noteNames[i] << ")\tSamples: " << np.debut_note[i] << " to "
                  << np.fin_note[i] << "(" << np.fin_note[i] - np.debut_note[i] << ")" << std::endl;
    }

    // analyse_note(np, volume_plat.size());

    return {{}};
}

NotesPacket analyse_rythme_impl(std::vector<float> volume_plat,
                                const Recording&   rec,
                                size_t             dt,
                                size_t             sampleCutoff,
                                float              marge_note)
{
    std::vector<float> derive_double(volume_plat.size());
    NotesPacket        np;


    for(size_t i = 0; i < volume_plat.size() - 1; i++)
    {
        derive_double[i] = volume_plat[i + 1] - volume_plat[i];
        if(derive_double[i] > marge_note)
        {
            np.debut_note.push_back(i);
        }
    }
    if(np.debut_note.size() == 0)
    {
        return {};
    }
    for(size_t i = 0; i < np.debut_note.size() - 1; i++)
    {
        size_t sampleLength = np.debut_note[i + 1] - np.debut_note[i];
        if(sampleLength < sampleCutoff)
        {
            // Removes note
            np.debut_note.erase(np.debut_note.begin() + i);
        }
    }

    for(size_t note = 0; note < np.debut_note.size(); note++)
    {
        float valeurAttaque = volume_plat[np.debut_note[note]];

        size_t max =
          note == np.debut_note.size() - 1 ? volume_plat.size() - 1 : np.debut_note[note + 1];
        size_t i = np.debut_note[note];
        for(; i < max; i += dt)
        {
            float currentValue = volume_plat[i];
            if(currentValue < 0.3f * valeurAttaque)
            {
                break;
            }
        }
        np.fin_note.push_back(std::min(i, volume_plat.size() - 1));
    }



    np.notes.reserve(np.debut_note.size());

    for(size_t i = 0; i < np.debut_note.size(); i++)
    {
        const float* beginIt = &rec[np.debut_note[i]];
        const float* endIt   = &rec[np.fin_note[i]];

        Recording tempRec{
          beginIt, endIt, rec.getSampleRate(), rec.getFramesPerBuffer(), rec.getNumChannels()};

        double freq = FindFrequency(tempRec);

        auto [str, val] = FindNoteFromFreq(freq);
        np.notes.push_back(val);
        np.noteNames.push_back(str);
    }

    correct_fuckaroos(np, dt);

    return np;
}

void correct_fuckaroos(NotesPacket& np, size_t dt)
{
    std::vector<size_t> durees;
    for(size_t i = 0; i < np.debut_note.size(); i++)
    {
        size_t duree = np.fin_note[i] - np.debut_note[i];

        if(duree <= 2 * dt)
        {
            if(i != 0 && i != np.notes.size() - 1)
            {
                if(np.debut_note[i] == np.fin_note[i - 1] && np.fin_note[i] == np.debut_note[i + 1])
                {
                    if(np.fin_note[i - 1] - np.debut_note[i - 1]
                       < np.fin_note[i + 1] - np.debut_note[i + 1])
                    {
                        np.fin_note.erase(np.fin_note.begin() + i - 1);
                        np.debut_note.erase(np.debut_note.begin() + i);
                        np.notes.erase(np.notes.begin() + i);
                        np.noteNames.erase(np.noteNames.begin() + i);
                        continue;
                    }
                    else
                    {
                        np.fin_note.erase(np.fin_note.begin() + i);
                        np.debut_note.erase(np.debut_note.begin() + i + 1);
                        np.notes.erase(np.notes.begin() + i);
                        np.noteNames.erase(np.noteNames.begin() + i);
                        continue;
                    }
                }
                else if(np.debut_note[i] == np.fin_note[i - 1])
                {
                    np.fin_note.erase(np.fin_note.begin() + i - 1);
                    np.debut_note.erase(np.debut_note.begin() + i);
                    np.notes.erase(np.notes.begin() + i);
                    np.noteNames.erase(np.noteNames.begin() + i);
                    continue;
                }
                else if(np.fin_note[i] == np.debut_note[i + 1])
                {
                    np.fin_note.erase(np.fin_note.begin() + i);
                    np.debut_note.erase(np.debut_note.begin() + i + 1);
                    np.notes.erase(np.notes.begin() + i);
                    np.noteNames.erase(np.noteNames.begin() + i);
                    continue;
                }
            }
        }
    }
}

std::vector<Note> analyse_note(const NotesPacket& np, size_t recordingLength)
{
    std::vector<int64_t> liste_duree;
    std::vector<int64_t> liste_ratios;

    /* Garder la duree des notes et des silences.
     * Chaque note est associee a un silence (meme s'il fait 0 de longueur). */
    for(int64_t i = 0; i < np.debut_note.size(); i++)
    {
        int64_t noteLength = np.fin_note[i] - np.debut_note[i];

        int64_t maxSilenceLength =
          i + 1 >= np.debut_note.size() ? recordingLength : np.debut_note[i + 1];
        int64_t silenceLength = maxSilenceLength - np.fin_note[i];

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
                    liste_symbole.push_back(Note(NoteType::Noire, np.notes[j++]));
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
                    liste_symbole.push_back(Note(NoteType::Noire, np.notes[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, np.notes[j++]));
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
                    liste_symbole.push_back(Note(NoteType::Blanche, np.notes[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, np.notes[j++]));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, np.notes[j++]));
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
                    liste_symbole.push_back(Note(NoteType::Ronde, np.notes[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Blanche, np.notes[j++]));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, np.notes[j++]));
                }
                else if(liste_ratios[i] == 3)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, np.notes[j++]));
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
                    liste_symbole.push_back(Note(NoteType::Ronde, np.notes[j++]));
                }
                else if(liste_ratios[i] == 1)
                {
                    liste_symbole.push_back(Note(NoteType::Blanche, np.notes[j++]));
                }
                else if(liste_ratios[i] == 2)
                {
                    liste_symbole.push_back(Note(NoteType::Noire, np.notes[j++]));
                }
                else if(liste_ratios[i] == 3)
                {
                    liste_symbole.push_back(Note(NoteType::Croche, np.notes[j++]));
                }
                else if(liste_ratios[i] == 4)
                {
                    liste_symbole.push_back(Note(NoteType::DoubleCroche, np.notes[j++]));
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


void filePrinter(const std::vector<float>& volume, const std::vector<float>& volume_plat, const NotesPacket& np)
{
    std::ofstream debugFile{"debug.txt"};
    for(int i = 0; i < volume.size(); i++)
    {
        debugFile << i << '\t';
        debugFile << volume[i] << '\t';
        debugFile << volume_plat[i] << '\t';

        if(std::find(np.debut_note.begin(), np.debut_note.end(), i) != np.debut_note.end())
        {
            debugFile << 1 << '\t';
        }
        else
        {
            debugFile << 0 << '\t';
        }
        if(std::find(np.fin_note.begin(), np.fin_note.end(), i) != np.fin_note.end())
        {
            debugFile << 1 << '\t';
        }
        else
        {
            debugFile << 0 << '\t';
        }
        debugFile << std::endl;
    }
    debugFile.close();
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
