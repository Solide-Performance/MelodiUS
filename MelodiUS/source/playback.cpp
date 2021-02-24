/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "playback.h"
#include "portaudio.h"
#include "recorder.h"

#include <iostream>


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define CALL_ERROR_HANDLER() errorHandler(err)    // NOLINT


/*****************************************************************************/
/* Type definitions -------------------------------------------------------- */
struct PlaybackStruct
{
    Recording& rec;
    size_t     index = 0;
};


/*****************************************************************************/
/* Static function declarations -------------------------------------------- */
static void errorHandler(PaError err);
static int  playCallback(const void*                     inputBuffer,
                         void*                           outputBuffer,
                         unsigned long                   framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags           statusFlags,
                         void*                           userData);


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
void Playback(const Recording& rec)
{
    PaStreamParameters outputParameters;
    PaStream*          stream;
    PaError            err = paNoError;

    /* Playback recorded data.  -------------------------------------------- */
    err = Pa_Initialize();
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if(outputParameters.device == paNoDevice)
    {
        std::cerr << "Error: No default output device." << std::endl;
    }
    outputParameters.channelCount = 2; /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    std::cout << "\n=== Now playing back. ===" << std::endl;

    PlaybackStruct ps{const_cast<Recording&>(rec)};

    err = Pa_OpenStream(
      &stream,
      nullptr, /* no input */
      &outputParameters,
      static_cast<double>(rec.getSampleRate()),
      static_cast<unsigned long>(rec.getFramesPerBuffer()),
      paClipOff, /* we won't output out of range samples so don't bother clipping them */
      playCallback,
      &ps);
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }

    if(stream != nullptr)
    {
        err = Pa_StartStream(stream);
        if(err != paNoError)
        {
            CALL_ERROR_HANDLER();
        }

        std::cout << "Waiting for playback to finish." << std::endl;

        while((err = Pa_IsStreamActive(stream)) == 1)
        {
            Pa_Sleep(100);  // NOLINT
        }
        if(err < 0)
        {
            CALL_ERROR_HANDLER();
        }

        err = Pa_CloseStream(stream);
        if(err != paNoError)
        {
            CALL_ERROR_HANDLER();
        }

        std::cout << "Done." << std::endl;
    }
}


/*****************************************************************************/
/* Static function definitions --------------------------------------------- */
/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
#pragma region
static int playCallback(const void*                     inputBuffer,
                        void*                           outputBuffer,
                        unsigned long                   framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags           statusFlags,
                        void*                           userData)
{
    PlaybackStruct& ps = *static_cast<PlaybackStruct*>(userData);

    if(ps.rec.getNumSamples() == ps.index * ps.rec.getNumChannels())
    {
        return paComplete;
    }

    SAMPLE* rptr       = &ps.rec[ps.index * ps.rec.getNumChannels()];
    SAMPLE* wptr       = static_cast<SAMPLE*>(outputBuffer);
    size_t  framesLeft = ps.rec.getMaxFrameIndex() - ps.index;

    (void)inputBuffer; /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if(framesLeft < framesPerBuffer)
    {
        size_t i = 0;
        /* final buffer... */
        for(; i < framesLeft; i++)
        {
            *wptr++ = *rptr++; /* left */    // NOLINT
            if(ps.rec.getNumChannels() == 2)
            {
                *wptr++ = *rptr++; /* right */    // NOLINT
            }
        }
        for(; i < framesPerBuffer; i++)
        {
            *wptr++ = 0; /* left */    // NOLINT
            if(ps.rec.getNumChannels() == 2)
            {
                *wptr++ = 0; /* right */    // NOLINT
            }
        }
        ps.index += framesLeft;
        return paComplete;
    }
    else
    {
        for(size_t i = 0; i < framesPerBuffer; i++)
        {
            *wptr++ = *rptr++; /* left */    // NOLINT
            if(ps.rec.getNumChannels() == 2)
            {
                *wptr++ = *rptr++; /* right */    // NOLINT
            }
        }
        ps.index += framesPerBuffer;
        return paContinue;
    }
}
#pragma endregion

static void errorHandler(PaError err)
{
    Pa_Terminate();

    if(err != paNoError)
    {
        std::cerr << "An error occured while using the portaudio stream\n"
                  << "Error number: " << static_cast<int>(err) << '\n'
                  << "Error message: " << Pa_GetErrorText(err) << std::endl;

        throw recorderException();
    }
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
