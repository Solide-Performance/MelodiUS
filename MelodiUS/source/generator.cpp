/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "generator.h"

#include <cmath>
#include <cstdlib>


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
constexpr double pi = 3.1415926535897932384626433;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
Recording Generate_Sine(size_t freq,
                        size_t numSeconds,
                        size_t numChannels,
                        size_t sampleRate,
                        size_t framesPerBuffer,
                        float  amplitude)
{
    size_t size   = numSeconds * sampleRate * numChannels;
    double cycles = static_cast<double>(numSeconds) / (1. / static_cast<double>(freq));

    std::vector<SAMPLE> data(size);

    /* initialise sinusoidal wavetable */
    for(size_t i = 0; i < size; i++)
    {
        data[i] = static_cast<SAMPLE>(amplitude * sin((cycles / size) * pi * 2. * i));
    }

    Recording rec{data.begin(), data.end(), sampleRate, framesPerBuffer, numChannels};

    return rec;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
