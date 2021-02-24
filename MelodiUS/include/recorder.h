#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "globaldef.h"
#include "recording.h"


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
/* #define SAMPLE_RATE  (17932)     // Test failure to open with this value. */
#define SAMPLE_RATE       (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_CHANNELS      (2)
#define NUM_SECONDS       (4)

/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG (0)


/*****************************************************************************/
/* Type definitions -------------------------------------------------------- */
using recorderException = std::exception;
struct paTestData
{
    size_t  frameIndex; /* Index into sample array. */
    size_t  maxFrameIndex;
    SAMPLE* recordedSamples;
};


/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
[[nodiscard]] Recording Record(size_t numSeconds      = NUM_SECONDS,
                               size_t sampleRate      = SAMPLE_RATE,
                               size_t framesPerBuffer = FRAMES_PER_BUFFER,
                               size_t numChannels     = NUM_CHANNELS);

void                    SaveToWav(std::string_view filename, const Recording& recording);
[[nodiscard]] Recording LoadFromWav(std::string_view filename);

[[nodiscard]] std::vector<short> Samples_FloatToShort(const std::vector<float> inVec);
[[nodiscard]] std::vector<float> Samples_ShortToFloat(const std::vector<short> inVec);


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
