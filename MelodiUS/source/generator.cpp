/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "generator.h"

#include <cmath>


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
Recording Generate_Sine(size_t freq,
                        double numSeconds,
                        size_t sampleRate,
                        size_t numChannels,
                        size_t framesPerBuffer,
                        float  amplitude)
{
    assert(numChannels >= MONO && numChannels <= STEREO);

    size_t size   = std::ceil(numSeconds) * sampleRate * numChannels;
    double cycles = numSeconds / (1. / static_cast<double>(freq));

    std::vector<SAMPLE> data(size);

    /* initialise sinusoidal wavetable */
    for(size_t i = 0; i < size / numChannels; i += numChannels)
    {
        SAMPLE sample = amplitude * double(sin((cycles / size) * std::numbers::pi * 2. * i));

        for(size_t chann = 0; chann < numChannels; chann++)
        {
            data[i + chann] = sample;
        }
    }

    Recording rec{data.begin(), data.end(), sampleRate, framesPerBuffer, numChannels};

    return rec;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
