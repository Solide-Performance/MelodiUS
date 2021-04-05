/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "recording.h"


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */

/* --------------------------------- */
#pragma region Operators
const SAMPLE& Recording::operator[](size_t index) const
{
    return m_samples[index];
}

SAMPLE& Recording::operator[](size_t index)
{
    return const_cast<SAMPLE&>(const_cast<const Recording*>(this)->operator[](index));    // NOLINT
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


void Recording::clear()
{
    m_samples.clear();
    m_framesPerBuffer = 0;
    m_numChannels     = 0;
    m_sampleRate      = 0;
}


#pragma endregion


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
