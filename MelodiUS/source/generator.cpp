/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "generator.h"
#include "portaudio.h"
#include <cmath>


#include "playback.h"
/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
#define TABLE_SIZE 200


/*****************************************************************************/
/* Type definitions -------------------------------------------------------- */
typedef struct
{
    float sine[TABLE_SIZE];
    int   phase;
} testStruct;


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
Recording Generate_Sine(size_t freq,
                        size_t numSeconds,
                        size_t sampleRate,
                        size_t framesPerBuffer,
                        size_t numChannels,
                        float amplitude)
{
    PaStreamParameters outputParameters;
    PaStream*          stream;
    PaError            err;
    testStruct         data;
    int                i;
    printf("PortAudio Test: output MONO sine wave. SR = %d, BufSize = %d\n",
           SAMPLE_RATE,
           FRAMES_PER_BUFFER);
    /* initialise sinusoidal wavetable */
    for(i = 0; i < TABLE_SIZE; i++)
    {
        data.sine[i] =
          (float)(amplitude * sin(((double)i / (double)TABLE_SIZE) * 3.14159265358979 * 2.));
    }
    data.phase = 0;

    std::vector<SAMPLE> sineArray{&data.sine[0], &data.sine[TABLE_SIZE - 1]};
    sineArray.reserve(128 * sineArray.size());
    for (int i = 0; i < 7; i++)
    {
        std::copy_n(sineArray.begin(), sineArray.size(), std::back_inserter(sineArray));
    }

    Recording rec{sineArray.begin(),
                  sineArray.end(),
                  sampleRate,
                  framesPerBuffer,
                  numChannels};

    Playback(rec);

    return rec;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
