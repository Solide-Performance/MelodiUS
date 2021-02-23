/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "playback.h"
#include "portaudio.h"
#include "recorder.h"


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define CALL_ERROR_HANDLER() errorHandler(err)


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
    paTestData         data;

    data.maxFrameIndex   = rec.getNumSeconds() * rec.getSampleRate();
    data.frameIndex      = 0;
    data.recordedSamples = (SAMPLE*)malloc(
      static_cast<size_t>(rec.getNumSeconds()) * rec.getSampleRate() * rec.getNumChannels()
      * sizeof(SAMPLE));
    if(data.recordedSamples == nullptr)
    {
        printf("Could not allocate record array.\n");
        CALL_ERROR_HANDLER();
    }
    for(int i = 0; i < rec.getNumSamples(); i++)
    {
        data.recordedSamples[i] = rec[i];
    }

    /* Playback recorded data.  -------------------------------------------- */
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if(outputParameters.device == paNoDevice)
    {
        fprintf(stderr, "Error: No default output device.\n");
    }
    outputParameters.channelCount = 2; /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency =
      Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    printf("\n=== Now playing back. ===\n");
    fflush(stdout);

    err = Pa_OpenStream(
      &stream,
      nullptr, /* no input */
      &outputParameters,
      rec.getSampleRate(),
      rec.getFramesPerBuffer(),
      paClipOff, /* we won't output out of range samples so don't bother clipping them */
      playCallback,
      &data);
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }

    if(stream)
    {
        err = Pa_StartStream(stream);
        if(err != paNoError)
        {
            CALL_ERROR_HANDLER();
        }

        printf("Waiting for playback to finish.\n");
        fflush(stdout);

        while((err = Pa_IsStreamActive(stream)) == 1)
        {
            Pa_Sleep(100);
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

        printf("Done.\n");
        fflush(stdout);
    }
}


/*****************************************************************************/
/* Static function definitions --------------------------------------------- */
/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
#pragma region
int g_numChannels = 2;

static int playCallback(const void*                     inputBuffer,
                        void*                           outputBuffer,
                        unsigned long                   framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags           statusFlags,
                        void*                           userData)
{
    paTestData*  data = (paTestData*)userData;
    SAMPLE*      rptr = &data->recordedSamples[data->frameIndex * g_numChannels];
    SAMPLE*      wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int          finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

    (void)inputBuffer; /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if(framesLeft < framesPerBuffer)
    {
        /* final buffer... */
        for(i = 0; i < framesLeft; i++)
        {
            *wptr++ = *rptr++; /* left */
            if(g_numChannels == 2)
            {
                *wptr++ = *rptr++; /* right */
            }
        }
        for(; i < framesPerBuffer; i++)
        {
            *wptr++ = 0; /* left */
            if(g_numChannels == 2)
            {
                *wptr++ = 0; /* right */
            }
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for(i = 0; i < framesPerBuffer; i++)
        {
            *wptr++ = *rptr++; /* left */
            if(g_numChannels == 2)
            {
                *wptr++ = *rptr++; /* right */
            }
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}
#pragma endregion

static void errorHandler(PaError err)
{
    Pa_Terminate();

    if(err != paNoError)
    {
        fprintf(stderr, "An error occured while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        // err = 1; /* Always return 0 or 1, but no other return codes. */

        throw recorderException();
    }
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
