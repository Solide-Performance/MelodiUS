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
#include <cstdio>
#include <cstdlib>

#include "portaudio.h"
#include "readwrite_wav.h"
#include "recorder.h"


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define CALL_ERROR_HANDLER() errorHandler(err, &data.recordedSamples)    // NOLINT



/*****************************************************************************/
/* Static variables -------------------------------------------------------- */
static size_t g_numChannels = static_cast<size_t>(-1);


/*****************************************************************************/
/* Static function declarations -------------------------------------------- */
static int  recordCallback(const void*                     inputBuffer,
                           void*                           outputBuffer,
                           unsigned long                   framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags           statusFlags,
                           void*                           userData);
static void errorHandler(PaError err, SAMPLE** dataBlock);


/*****************************************************************************/
/* Non-static function definitions ----------------------------------------- */
Recording Record(size_t numSeconds, size_t sampleRate, size_t framesPerBuffer, size_t numChannels)
{
    PaStreamParameters inputParameters;
    PaStream*          stream;
    PaError            err = paNoError;
    paTestData         data{};
    SAMPLE             max, val;
    double             average;

    g_numChannels = numChannels;

    size_t totalFrames = data.maxFrameIndex =
      numSeconds * sampleRate; /* Record for a few seconds. */

    data.frameIndex   = 0;
    size_t numSamples = totalFrames * numChannels;
    size_t numBytes   = numSamples * sizeof(SAMPLE);
    data.recordedSamples =
      static_cast<SAMPLE*>(malloc(numBytes)); /* From now on, recordedSamples is initialised. */
    if(data.recordedSamples == nullptr)
    {
        printf("Could not allocate record array.\n");
        CALL_ERROR_HANDLER();
    }
    for(size_t i = 0; i < numSamples; i++)
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
      static_cast<double>(sampleRate),
      static_cast<unsigned long>(framesPerBuffer),
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
        printf("index = %zd\n", data.frameIndex);
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
    for(int i = 0; i < numSamples; i++)
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

    Recording recording{&data.recordedSamples[0],
                        &data.recordedSamples[numSamples],
                        sampleRate,
                        framesPerBuffer,
                        numChannels};

    g_numChannels = static_cast<size_t>(-1);
    free(data.recordedSamples);

    return recording;
}

void SaveToWav(std::string_view filename, const Recording& recording)
{
    std::vector<short> shortData = Samples_FloatToShort(recording.getSamples());

    WAV_Writer writer{
      filename,
      static_cast<unsigned long>(recording.getSampleRate() * recording.getNumChannels()),
      1};

    writer.Write(shortData.data(), shortData.size());
}

Recording LoadFromWav(std::string_view filename)
{
    WAV_Reader reader{filename};
    reader.Read();

    std::vector<float> floatData = Samples_ShortToFloat(reader.get_Data());

    // @todo
    // HARDCODED '2' & '1' !!!!!!! TO REMOVE
    return {&floatData.front(), &floatData.back(), (size_t)(reader.get_FrameRate() / 2), 1, 2};
}


std::vector<short> Samples_FloatToShort(const std::vector<float>& inVec)
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

std::vector<float> Samples_ShortToFloat(const std::vector<short>& inVec)
{
    std::vector<float> floatData(inVec.size());

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
    size_t        framesToCalc;
    int           finished;
    size_t        framesLeft = data->maxFrameIndex - data->frameIndex;

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
        for(size_t i = 0; i < framesToCalc; i++)
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
        for(size_t i = 0; i < framesToCalc; i++)
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

#pragma endregion


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
