#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "globaldef.h"
#include <vector>


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr size_t MONO   = 1;
constexpr size_t STEREO = 2;


/*****************************************************************************/
/* Type definition --------------------------------------------------------- */
#pragma region Sample format selection
#if 1
#define PA_SAMPLE_TYPE paFloat32    // NOLINT
using SAMPLE                    = float;
constexpr SAMPLE SAMPLE_SILENCE = 0.0f;
#define PRINTF_S_FORMAT "%.8f"    // NOLINT
#else
#define PA_SAMPLE_TYPE  paInt16
using SAMPLE                    = int16_t;
constexpr SAMPLE SAMPLE_SILENCE = 0;
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
    ~Recording()                = default;
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
#pragma region Operators
    [[nodiscard]] INLINE const SAMPLE& operator[](size_t index) const
    {
        return m_samples[index];
    }

    [[nodiscard]] INLINE SAMPLE& operator[](size_t index)
    {
        return const_cast<SAMPLE&>(
          const_cast<const Recording*>(this)->operator[](index));    // NOLINT
    }
#pragma endregion

/* --------------------------------- */
#pragma region Accessors
    [[nodiscard]] INLINE const std::vector<SAMPLE>& getSamples() const
    {
        return m_samples;
    }
    [[nodiscard]] INLINE size_t getSampleRate() const
    {
        return m_sampleRate;
    }

    [[nodiscard]] INLINE size_t getNumSamples() const
    {
        return m_samples.size();
    }

    [[nodiscard]] INLINE float getNumSeconds() const
    {
        return getNumSamples() / static_cast<float>(getNumChannels() * getSampleRate());
    }

    [[nodiscard]] INLINE size_t getFramesPerBuffer() const
    {
        return m_framesPerBuffer;
    }

    [[nodiscard]] INLINE size_t getMaxFrameIndex() const
    {
        return getNumSamples() / getNumChannels();
    }

    [[nodiscard]] INLINE size_t getNumChannels() const
    {
        return m_numChannels;
    }


    [[nodiscard]] INLINE bool isValid() const
    {
        return !m_samples.empty();
    }


    [[nodiscard]] INLINE std::vector<SAMPLE>::const_iterator begin() const
    {
        return m_samples.cbegin();
    }
    [[nodiscard]] INLINE std::vector<SAMPLE>::const_iterator end() const
    {
        return m_samples.cend();
    }


    void clear()
    {
        m_samples.clear();
        m_framesPerBuffer = 0;
        m_numChannels     = 0;
        m_sampleRate      = 0;
    }


#pragma endregion
};


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
