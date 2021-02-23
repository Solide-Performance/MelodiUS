/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "recording.h"


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */

/* --------------------------------- */
#pragma region Constructors
Recording::Recording(const SAMPLE* samplesBegin,
                     const SAMPLE* samplesEnd,
                     size_t        sampleRate,
                     size_t        framesPerBuffer,
                     size_t        numChannels)
: m_samples{samplesBegin, samplesEnd},
  m_sampleRate{sampleRate},
  m_framesPerBuffer{framesPerBuffer},
  m_numChannels{numChannels}
{
}

#pragma endregion


/* --------------------------------- */
#pragma region Operators
const SAMPLE& Recording::operator[](size_t index) const
{
    return m_samples[index];
}

SAMPLE& Recording::operator[](size_t index)
{
    return const_cast<SAMPLE&>(const_cast<const Recording*>(this)->operator[](index));
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

size_t Recording::getNumSamples() const
{
    return m_samples.size();
}

float Recording::getNumSeconds() const
{
    return getNumSamples() / static_cast<float>(getNumChannels() * getSampleRate());
}

size_t Recording::getFramesPerBuffer() const
{
    return m_framesPerBuffer;
}

size_t Recording::getMaxFrameIndex() const
{
    return getNumSamples() / getNumChannels();
}

size_t Recording::getNumChannels() const
{
    return m_numChannels;
}


bool Recording::isValid() const
{
    return !m_samples.empty();
}


std::vector<SAMPLE>::const_iterator Recording::begin() const
{
    return m_samples.cbegin();
}
std::vector<SAMPLE>::const_iterator Recording::end() const
{
    return m_samples.cend();
}
#pragma endregion

/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
