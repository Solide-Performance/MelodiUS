#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "globaldef.h"
#include <vector>


/*****************************************************************************/
/* Type definition --------------------------------------------------------- */
#pragma region Sample format selection
#if 1
#define PA_SAMPLE_TYPE paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#else
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#endif
#pragma endregion


/*****************************************************************************/
/* Class ------------------------------------------------------------------- */
class Recording
{
private:
    std::vector<SAMPLE> m_samples{};
    size_t              m_sampleRate      = 0;
    size_t              m_framesPerBuffer = 0;
    size_t              m_numChannels     = 0;

public:
    /* --------------------------------- */
    /* Constructors                      */
    Recording()                 = default;
    Recording(const Recording&) = default;
    Recording(Recording&&)      = default;
    Recording& operator=(const Recording&) = default;
    Recording& operator=(Recording&&) = default;

    template<typename InputIterator>
    Recording(const InputIterator samplesBegin,
              const InputIterator samplesEnd,
              size_t              sampleRate,
              size_t              framesPerBuffer,
              size_t              numChannels)
    : m_samples{samplesBegin, samplesEnd},
      m_sampleRate{sampleRate},
      m_framesPerBuffer{framesPerBuffer},
      m_numChannels{numChannels}
    {
    }


    /* --------------------------------- */
    /* Operators                         */
    [[nodiscard]] const SAMPLE& operator[](size_t index) const;
    [[nodiscard]] SAMPLE&       operator[](size_t index);

    /* --------------------------------- */
    /* Accessors                         */
    [[nodiscard]] const std::vector<SAMPLE>& getSamples() const;

    [[nodiscard]] size_t getSampleRate() const;
    [[nodiscard]] size_t getNumSamples() const;
    [[nodiscard]] float  getNumSeconds() const;
    [[nodiscard]] size_t getFramesPerBuffer() const;
    [[nodiscard]] size_t getMaxFrameIndex() const;
    [[nodiscard]] size_t getNumChannels() const;
    [[nodiscard]] bool   isValid() const;

    [[nodiscard]] std::vector<SAMPLE>::const_iterator begin() const;
    [[nodiscard]] std::vector<SAMPLE>::const_iterator end() const;
};


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
