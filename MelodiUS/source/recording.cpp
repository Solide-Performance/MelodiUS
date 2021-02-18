/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "recording.h"


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */

/* --------------------------------- */
#pragma region Constructors
Recording::Recording(SAMPLE* samplesBegin,
                     SAMPLE* samplesEnd,
                     size_t  sampleRate,
                     size_t  framesPerBuffer,
                     size_t  numChannels)
: m_samples{samplesBegin, samplesEnd},
  m_sampleRate{sampleRate},
  m_framesPerBuffer{framesPerBuffer},
  m_numChannels{numChannels}
{
}

#pragma endregion


/* --------------------------------- */
#pragma region Operators
SAMPLE Recording::operator[](size_t index)
{
    return m_samples[index];
}
#pragma endregion

/* --------------------------------- */
#pragma region Accessors
const std::vector<SAMPLE>& Recording::getSamples() const
{
    return m_samples;
}
size_t Recording::getSampleRate() const
{
    return m_sampleRate;
}

size_t Recording::getNumberOfSamples() const
{
    return m_samples.size();
}

size_t Recording::getFramesPerBuffer() const
{
    return m_framesPerBuffer;
}
size_t Recording::getNumChannels() const
{
    return m_numChannels;
}
#pragma endregion

/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
