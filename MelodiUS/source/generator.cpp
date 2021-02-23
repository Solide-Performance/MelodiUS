/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "generator.h"
#include "portaudio.h"
#include <cmath>


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
Recording Generate_Sine(size_t freq,
                        size_t numSeconds,
                        size_t sampleRate,
                        size_t framesPerBuffer,
                        size_t numChannels,
                        float  amplitude)
{
    size_t size   = numSeconds * sampleRate * numChannels;
    double cycles = static_cast<double>(numSeconds) / (1. / static_cast<double>(freq));

    SAMPLE* data = (SAMPLE*)malloc(size * sizeof(SAMPLE));

    /* initialise sinusoidal wavetable */
    for(size_t i = 0; i < size; i++)
    {
        data[i] = (SAMPLE)(amplitude * sin((cycles / size) * 3.14159265358979 * 2. * i));
    }

    Recording rec{&data[0], &data[size - 1], sampleRate, framesPerBuffer, numChannels};

    return rec;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
