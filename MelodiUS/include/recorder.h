#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "globaldef.h"
#include "recording.h"


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr size_t SAMPLE_RATE       = 44100;
constexpr size_t FRAMES_PER_BUFFER = 512;
constexpr size_t NUM_CHANNELS      = 2;
constexpr size_t NUM_SECONDS       = 60;

/* #define DITHER_FLAG     (paDitherOff) */
constexpr bool DITHER_FLAG = false;

/* clang-format off */
static std::function<bool()> EMPTY_STOP_POLICY{};
static std::function<bool()> TRUE_STOP_POLICY{[]{ return true; }};
/* clang-format on */


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

void Recording_SetStopPolicy(const std::function<bool()>& newPolicy);

[[nodiscard]] std::vector<short> Samples_FloatToShort(const std::vector<float>& inVec);
[[nodiscard]] std::vector<float> Samples_ShortToFloat(const std::vector<short>& inVec);


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
