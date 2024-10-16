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
#ifndef LINUX_
#include "portaudio/portaudio.h"
#include <conio.h>    // Used for _kbhit example, can be removed
#endif

#include "readwrite_wav.h"
#include "recorder.h"

#include <cstdio>
#include <cstdlib>
#include <future>
#include <iostream>
#include <thread>


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define CALL_ERROR_HANDLER() errorHandler(err, &data.recordedSamples)    // NOLINT


/*****************************************************************************/
/* Static function declarations -------------------------------------------- */
#ifndef LINUX_
static void errorHandler(PaError err, SAMPLE** dataBlock);
static int  recordCallback(const void*                     inputBuffer,
                           void*                           outputBuffer,
                           unsigned long                   framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags           statusFlags,
                           void*                           userData);
#endif
static void recorderStopHandler();
static bool keyboardStopListener();


/*****************************************************************************/
/* Static variables -------------------------------------------------------- */
static size_t                g_numChannels = static_cast<size_t>(-1);
static bool                  g_stopFlag    = false;
static std::function<bool()> g_policy      = keyboardStopListener;


/*****************************************************************************/
/* Non-static function definitions ----------------------------------------- */
Recording Record(size_t numSeconds, size_t sampleRate, size_t framesPerBuffer, size_t numChannels)
{
#ifdef LINUX_
    return Recording{};
#else
    PaStreamParameters inputParameters;
    PaStream*          stream = nullptr;
    PaError            err    = paNoError;
    paTestData         data{};

    g_numChannels = numChannels;

    size_t totalFrames = data.maxFrameIndex = numSeconds * sampleRate;

    data.frameIndex   = 0;
    size_t numSamples = totalFrames * numChannels;
    size_t numBytes   = numSamples * sizeof(SAMPLE);
    data.recordedSamples =
      static_cast<SAMPLE*>(malloc(numBytes)); /* From now on, recordedSamples is initialised. */
    if(data.recordedSamples == nullptr)
    {
        std::cout << "Could not allocate record array." << std::endl;
        CALL_ERROR_HANDLER();
    }
    for(size_t i = 0; i < numSamples; i++)
    {
        data.recordedSamples[i] = 0;
    }

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if(inputParameters.device == paNoDevice)
    {
        std::cerr << "Error: No default input device." << std::endl;
        CALL_ERROR_HANDLER();
        throw recorderException{};
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
    std::cout << "\n=== Now recording!! Please speak into the microphone. ===" << std::endl;

    /* Clear input buffer */
    std::fflush(stdin);

    /* Start listening for ' ' to be pressed.
     * If pressed, stop recording audio */
    std::thread stopperThread{recorderStopHandler};
    while((err = Pa_IsStreamActive(stream)) == 1)
    {
        Pa_Sleep(1000);    // NOLINT
    }
    if(err < 0)
    {
        CALL_ERROR_HANDLER();
    }
    std::cout << "Recording complete (" << data.frameIndex << ')' << std::endl;

    /* Clear input buffer */
    std::fflush(stdin);
    g_stopFlag = true;
    stopperThread.join();
    g_stopFlag = false;

    err = Pa_CloseStream(stream);
    if(err != paNoError)
    {
        CALL_ERROR_HANDLER();
    }

    Recording recording{&data.recordedSamples[0],
                        &data.recordedSamples[data.frameIndex * numChannels],
                        sampleRate,
                        framesPerBuffer,
                        numChannels};

    g_numChannels = static_cast<size_t>(-1);
    free(data.recordedSamples);

    return recording;
#endif
}

void Recording_SetStopPolicy(const std::function<bool()>& newPolicy)
{
    if(newPolicy)
    {
        g_policy = newPolicy;
    }
    else
    {
        g_policy = keyboardStopListener;
    }
}

/*****************************************************************************/
/* Static function definitions --------------------------------------------- */
static void recorderStopHandler()
{
    /* Stopflag is used to kill this thread, as well as to kill the listener thread */
    while(g_stopFlag != true)
    {
        bool evaluation = g_policy();
        if(evaluation)
        {
            g_stopFlag = true;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
static bool keyboardStopListener()
{
#ifndef LINUX_
    return _kbhit() != 0;
#else
    return false;
#endif;
}

#ifndef LINUX_
static void errorHandler(PaError err, SAMPLE** dataBlock)
{
    // Pa_Terminate();

    if(dataBlock != nullptr)
    {
        free(*dataBlock);
        *dataBlock = nullptr;
    }
    if(err != paNoError)
    {
        std::cerr << "An error occured while using the portaudio stream\n";
        std::cerr << "Error number: " << err << '\n';
        std::cerr << "Error message: " << Pa_GetErrorText(err) << std::endl;

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
    paTestData*   data         = static_cast<paTestData*>(userData);
    const SAMPLE* rptr         = static_cast<const SAMPLE*>(inputBuffer);
    SAMPLE*       wptr         = &data->recordedSamples[data->frameIndex * g_numChannels];
    size_t        framesToCalc = 0;
    int           finished;
    size_t        framesLeft = data->maxFrameIndex - data->frameIndex;

    (void)outputBuffer; /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if(g_stopFlag == true)
    {
        return paComplete;
    }

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

#endif
#pragma endregion


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
