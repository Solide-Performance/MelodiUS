#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "globaldef.h"
#include "recorder.h"
#include "recording.h"


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr double SINE_AMPLITUDE = 0.8;


/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
[[nodiscard]] Recording Generate_Sine(size_t freq,
                                      double numSeconds      = NUM_SECONDS,
                                      size_t sampleRate      = SAMPLE_RATE,
                                      size_t numChannels     = NUM_CHANNELS,
                                      size_t framesPerBuffer = FRAMES_PER_BUFFER,
                                      float  amplitude       = SINE_AMPLITUDE);


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
