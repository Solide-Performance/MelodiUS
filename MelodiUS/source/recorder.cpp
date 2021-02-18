/*
 * $Id$
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "portaudio.h"
#include "write_wav.h"

#include "recorder.h"


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
#define EPSILON 0.0005


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define CALL_ERROR_HANDLER()   errorHandler(err, &data.recordedSamples)
#define COMPARE_FLOATS(f1, f2) (std::abs(f1 - f2) <= EPSILON)


/*****************************************************************************/
/* Type definitions -------------------------------------------------------- */
typedef struct
{
    int     frameIndex; /* Index into sample array. */
    int     maxFrameIndex;
    SAMPLE* recordedSamples;
} paTestData;


/*****************************************************************************/
/* Static variables -------------------------------------------------------- */
static size_t g_numChannels = -1;


/*****************************************************************************/
/* Static function declarations -------------------------------------------- */
static int  playCallback(const void*                     inputBuffer,
                         void*                           outputBuffer,
                         unsigned long                   framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags           statusFlags,
                         void*                           userData);
static int  recordCallback(const void*                     inputBuffer,
                           void*                           outputBuffer,
                           unsigned long                   framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags           statusFlags,
                           void*                           userData);
static void errorHandler(PaError err, SAMPLE** dataBlock);


/*****************************************************************************/
/* Non-static function definitions ----------------------------------------- */
std::vector<SAMPLE> Record(size_t numSeconds,
                           size_t sampleRate,
                           size_t framesPerBuffer,
                           size_t numChannels)
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream*          stream;
    PaError            err = paNoError;
    paTestData         data;
    int                i;
    int                totalFrames;
    int                numSamples;
    int                numBytes;
    SAMPLE             max, val;
    double             average;

    g_numChannels = numChannels;

    printf("patest_record.c\n");
    fflush(stdout);

    data.maxFrameIndex = totalFrames = numSeconds * sampleRate; /* Record for a few seconds. */
    data.frameIndex                  = 0;
    numSamples                       = totalFrames * numChannels;
    numBytes                         = numSamples * sizeof(SAMPLE);
    data.recordedSamples =
      (SAMPLE*)malloc(numBytes); /* From now on, recordedSamples is initialised. */
    if(data.recordedSamples == nullptr)
    {
        printf("Could not allocate record array.\n");
        CALL_ERROR_HANDLER();
    }
    for(i = 0; i < numSamples; i++)
    {
        data.recordedSamples[i] = 0;
    }

    err = Pa_Initialize();
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if(inputParameters.device == paNoDevice)
    {
        fprintf(stderr, "Error: No default input device.\n");
        CALL_ERROR_HANDLER();
    }
    inputParameters.channelCount = 2; /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency =
      Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
      &stream,
      &inputParameters,
      nullptr, /* &outputParameters, */
      sampleRate,
      framesPerBuffer,
      paClipOff, /* we won't output out of range samples so don't bother clipping them */
      recordCallback,
      &data);
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }

    err = Pa_StartStream(stream);
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }
    printf("\n=== Now recording!! Please speak into the microphone. ===\n");
    fflush(stdout);

    while((err = Pa_IsStreamActive(stream)) == 1)
    {
        Pa_Sleep(1000);
        printf("index = %d\n", data.frameIndex);
        fflush(stdout);
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

    /* Measure maximum peak amplitude. */
    max     = 0;
    average = 0.0;
    for(i = 0; i < numSamples; i++)
    {
        val = data.recordedSamples[i];
        val = std::abs(val);
        if(val > max)
        {
            max = val;
        }
        average += val;
    }

    average = average / (double)numSamples;

    printf("sample max amplitude = " PRINTF_S_FORMAT "\n", max);
    printf("sample average = %lf\n", average);

    /* Playback recorded data.  -------------------------------------------- */
    data.frameIndex = 0;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if(outputParameters.device == paNoDevice)
    {
        fprintf(stderr, "Error: No default output device.\n");
        CALL_ERROR_HANDLER();
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
      sampleRate,
      framesPerBuffer,
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

    std::vector<SAMPLE> samples{&data.recordedSamples[0],
                                &data.recordedSamples[data.maxFrameIndex]};

    g_numChannels = -1;
    free(data.recordedSamples);

    return samples;
}

#include <algorithm>
void SaveToWav(const char* filename, const std::vector<SAMPLE>& records, size_t sampleRate)
{
    WAV_Writer writer;

    std::vector<short> shortData         = Samples_FloatToShort(records);

    // 2x sample rate for some reason, might be causing a bug
    int result = Audio_WAV_OpenWriter(&writer, "Bon matin.wav", sampleRate * 2, 1);
    if(result < 0)
    {
        errorHandler(result, nullptr);
    }
    result = Audio_WAV_WriteShorts(&writer, shortData.data(), shortData.size());
    if(result < 0)
    {
        errorHandler(result, nullptr);
    }

    result = Audio_WAV_CloseWriter(&writer);
    if(result < 0)
    {
        errorHandler(result, nullptr);
    }

#if 0    // Default example
    {
        FILE* fid;
        fid = fopen("recorded.raw", "wb");
        if(fid == nullptr)
        {
            printf("Could not open file.");
        }
        else
        {
            fwrite(data.recordedSamples, numChannels * sizeof(SAMPLE), totalFrames, fid);
            fclose(fid);
            printf("Wrote data to 'recorded.raw'\n");
        }
    }
#endif
}


std::vector<short> Samples_FloatToShort(const std::vector<float> inVec)
{
    std::vector<short> shortData = std::vector<short>(inVec.size());

    // https://stackoverflow.com/a/56213245/10827197
    for(int i = 0; i < inVec.size(); i++)
    {
        float floatData = inVec[i] * 32767;
        shortData[i]    = (short)floatData;
    }

    return shortData;
}

std::vector<float> Samples_ShortToFloat(const std::vector<short> inVec)
{
    std::vector<float> floatData = std::vector<float>(inVec.size());

    for(int i = 0; i < inVec.size(); i++)
    {
        float shortData = (float)inVec[i] / 32767;
        floatData[i]    = shortData;
    }

    return floatData;
}


/*****************************************************************************/
/* Static function definitions --------------------------------------------- */
static void errorHandler(PaError err, SAMPLE** dataBlock)
{
    Pa_Terminate();

    if(dataBlock != nullptr)
    {
        free(*dataBlock);
        *dataBlock = nullptr;
    }
    if(err != paNoError)
    {
        fprintf(stderr, "An error occured while using the portaudio stream\n");
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
        // err = 1; /* Always return 0 or 1, but no other return codes. */

        throw recorderException();
    }
}

#pragma region Callback functions
/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int recordCallback(const void*                     inputBuffer,
                          void*                           outputBuffer,
                          unsigned long                   framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags           statusFlags,
                          void*                           userData)
{
    paTestData*   data = (paTestData*)userData;
    const SAMPLE* rptr = (const SAMPLE*)inputBuffer;
    SAMPLE*       wptr = &data->recordedSamples[data->frameIndex * g_numChannels];
    long          framesToCalc;
    long          i;
    int           finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void)outputBuffer; /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if(framesLeft < framesPerBuffer)
    {
        framesToCalc = framesLeft;
        finished     = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished     = paContinue;
    }

    if(inputBuffer == nullptr)
    {
        for(i = 0; i < framesToCalc; i++)
        {
            *wptr++ = SAMPLE_SILENCE; /* left */
            if(g_numChannels == 2)
            {
                *wptr++ = SAMPLE_SILENCE; /* right */
            }
        }
    }
    else
    {
        for(i = 0; i < framesToCalc; i++)
        {
            *wptr++ = *rptr++; /* left */
            if(g_numChannels == 2)
            {
                *wptr++ = *rptr++; /* right */
            }
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
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


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
