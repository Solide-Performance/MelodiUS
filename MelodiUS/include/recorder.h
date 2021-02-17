#pragma once

/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include <stdexcept>
#include <vector>


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


#pragma region Sample format selection
#if 1
#define PA_SAMPLE_TYPE paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif
#pragma endregion


/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
std::vector<SAMPLE> Record(size_t numSeconds      = NUM_SECONDS,
                           size_t sampleRate      = SAMPLE_RATE,
                           size_t framesPerBuffer = FRAMES_PER_BUFFER,
                           size_t numChannels     = NUM_CHANNELS);

void SaveToWav(const char*                filename,
               const std::vector<SAMPLE>& records,
               size_t                     sampleRate = SAMPLE_RATE);


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
