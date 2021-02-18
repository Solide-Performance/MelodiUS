#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
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
    std::vector<SAMPLE> m_samples;
    size_t              m_sampleRate      = -1;
    size_t              m_framesPerBuffer = -1;
    size_t              m_numChannels     = -1;

public:
    /* --------------------------------- */
    /* Constructors                      */
    Recording()                 = default;
    Recording(const Recording&) = default;
    Recording(Recording&&)      = default;

    Recording(SAMPLE* samplesBegin,
              SAMPLE* samplesEnd,
              size_t  sampleRate,
              size_t  framesPerBuffer,
              size_t  numChannels);

    /* --------------------------------- */
    /* Operators                         */
    SAMPLE operator[](size_t index);

    /* --------------------------------- */
    /* Accessors                         */
    const std::vector<SAMPLE>& getSamples() const;
    size_t                     getSampleRate() const;
    size_t                     getNumberOfSamples() const;
    size_t                     getFramesPerBuffer() const;
    size_t                     getNumChannels() const;
};


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
