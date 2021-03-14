/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "detection_rythme.h"
#include "globaldef.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

#ifndef LINUX_
#include <immintrin.h>
#include <xmmintrin.h>
#endif


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr double  epsilon       = 0.005;
constexpr int64_t MARGE_moment  = 1000;
constexpr size_t  MARGE_attaque = 200;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
int analyse_rythme(const Recording& rec)
{
    const std::vector<float>& tableau = rec.getSamples();
    size_t                    taille  = tableau.size();

    std::vector<float>  volume(taille);
    std::vector<size_t> distance(16); /* Allocates 16 notes, will grow if needed */

    float maximum = 0;
#pragma omp parallel for
    for(size_t i = 0; i < taille - 1; i++)
    {
        float derive = tableau[i + 1] - tableau[i];

        if(COMPARE_FLOATS(derive, 0.0f, epsilon) && tableau[i] > 0)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            maximum = tableau[i];
        }
        volume[i] = maximum;
    }


    std::vector<float> derive_double(taille);
    float              volmax = *std::max_element(volume.cbegin(), volume.cend());
#pragma omp parallel for
    for(size_t i = 0, note = 0; i < taille - 1; i++)
    {
        float deriv_double = volume[i + 1] - volume[i];
        derive_double[i]   = deriv_double;

        if(COMPARE_FLOATS(deriv_double, 0.0f, epsilon) && volume[i] > 0.5 * volmax)
        {    // comparaison avec marge d'erreur, utilise la fonction de pascal
            distance.push_back(i - note);
            note = i;
        }
    }

    size_t distanceMax = *std::max_element(distance.begin(), distance.end());
    distance.erase(std::remove_if(distance.begin(),
                                  distance.end(),
                                  [&](size_t a) {
                                      return a < distanceMax * 0.05;
                                  }),
                   distance.end());
    size_t notesQty = distance.size();

    // for(size_t dist : distance)
    //{
    //    std::cout << dist << '\n';
    //}
    // std::cout << std::endl;

    std::vector<size_t> index_debut(notesQty);
#pragma omp parallel for
    for(size_t i = 0; i < notesQty; i++)
    {
        index_debut[i] = std::accumulate(distance.begin(), distance.begin() + i + 1, 0);
    }

    // for(size_t debut : index_debut)
    //{
    //    std::cout << debut << '\n';
    //}
    // std::cout << std::endl;

    std::vector<size_t>      index_fin(notesQty);
    std::vector<std::thread> threadPool{};
    threadPool.reserve(notesQty);

    auto lambda = [&](size_t i, int64_t limit) {
        size_t  debut    = index_debut[i];
        int64_t compteur = debut;

        float volume_attaque =
          std::accumulate(&volume[debut - MARGE_attaque], &volume[debut + MARGE_attaque], 0.0f);

        // std::cout << volume_attaque << std::endl;

#pragma omp parallel for
        for(; compteur < limit; compteur++)
        {
#ifndef LINUX_
            // https://ravikiranb.com/tutorials/mac-with-simd/
            // https://stackoverflow.com/a/32992310/10827197
            // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_add_ss
            // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_load_p
            // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_hadd_ps
            // https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_extract_ps

            auto begin = &volume[std::max(compteur - MARGE_moment, 0ll)];

            const size_t N = std::min(size_t(compteur + MARGE_moment), volume.size() - 1)
                             - std::max(compteur - MARGE_moment, 0ll);

            __m128 mm_volume_moment = {0.0f, 0.0f, 0.0f, 0.0f};
            float  volume_moment    = 0.0f;

            /* Main SIMD loop */
            size_t i = 0;
            for(; i < (N & ~(4 - 1)); i += 4)
            {
                mm_volume_moment = _mm_add_ss(mm_volume_moment, _mm_load_ps(begin + i));
            }

            /* Pack 128-bits info into a single register by adding them */
            mm_volume_moment = _mm_hadd_ps(mm_volume_moment, mm_volume_moment);
            mm_volume_moment = _mm_hadd_ps(mm_volume_moment, mm_volume_moment);

            /* Get SIMD register value */
            volume_moment = _mm_extract_ps(mm_volume_moment, 0);

            /* add up remaining single values until all elements are covered */
            for(; i < N; i++)
            {
                volume_moment += begin[i];
            }

#else
            float volume_moment =
              std::accumulate(&volume[std::max(compteur - MARGE_moment, 0ll)],
                              &volume[std::min(size_t(compteur + MARGE_moment), volume.size() - 1)],
                              0.0f);
#endif

            if(volume_moment < 0.3f * volume_attaque)
            {
                break;
            }
        }
        index_fin[i] = compteur;
    };
#pragma omp parallel for
    for(size_t i = 0; i < notesQty - 1; i++)
    {
        threadPool.emplace_back(lambda, i, index_debut[i + 1]);
    }
    threadPool.emplace_back(lambda, notesQty - 1, volume.size());
    for(std::thread& t : threadPool)
    {
        t.join();
    }

    for(size_t fin : index_fin)
    {
        std::cout << fin << '\n';
    }
    return 0;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
