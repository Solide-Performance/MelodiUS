﻿/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "benchmark.h"
#include "detection_rythme.h"
#include "fft.h"
#include "fpga_phoneme.h"
#include "generator.h"
#include "globaldef.h"
#include "gui.h"
#include "playback.h"
#include "readwrite_wav.h"
#include "recorder.h"
#include "recording.h"
#include "tuning.h"

#ifndef LINUX_
#include "portaudio/portaudio.h"
#endif

#include <filesystem>
#include <iostream>
#include <string>
#include <thread>


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr size_t SECONDS_MAX = 60;
constexpr size_t SECONDS_MIN = 0;
constexpr size_t FREQ_MAX    = 20000;
constexpr size_t FREQ_MIN    = 50;


/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
void menuHandler();
void setupPortaudio();
void setupFPGA();


/*****************************************************************************/
/* Entry point ------------------------------------------------------------- */
int main(int argc, char* argv[])
{
    /* Invoke GUI */
    // std::thread gui{mainOfGui, argc, argv};

    /* portaudio init */
    std::thread portAudioInitThread(setupPortaudio);

    /* CommunicationFPGA init */
    setupFPGA();

    /* Menu display and command handling */
    menuHandler();

/* Close portaudio & FPGA*/
#ifndef LINUX_
    portAudioInitThread.join();
    Pa_Terminate();
    FPGA::DeInit();
#endif

    // gui.join();
    return 0;
}


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
void menuHandler()
{
    Recording rec;

    // std::string path = "tests/test_gamme/";
    // for(const std::filesystem::directory_entry& entry :
    //    std::filesystem::recursive_directory_iterator(path))
    //{
    //    std::cout << entry.path() << ": ";
    //    try
    //    {
    //        rec = LoadFromWav(entry.path().generic_string());
    //        if(rec.isValid())
    //        {
    //            /*double           freq = FindFrequency(rec);
    //            auto [NoteName, NoteVal] = FindNoteFromFreq(freq);
    //            std::cout << NoteName << " (" << freq << ")" << std::endl;*/
    //            analyse_rythme(rec);
    //        }
    //        else
    //        {
    //            std::cout << "Must read valid audio" << std::endl;
    //        }
    //    }
    //    catch(const std::exception& ex)
    //    {
    //        std::cout << "Could not read .wav file" << ex.what() << std::endl;
    //    }
    //}
    // while(1)
    //{
    //}

    while(true)
    {
        std::cout << std::endl;
        std::cout << "\n ----- MelodiUS ----- \n";
        std::cout << "1 - Start recording\n";
        std::cout << "2 - Generate sinus wave\n";
        std::cout << "3 - Playback recorded audio\n";
        std::cout << "4 - Save .wav file\n";
        std::cout << "5 - Load .wav file\n";
        std::cout << "6 - Rythm analysis\n";
        std::cout << "7 - Find main frequency of signal\n";
        std::cout << "0 - Exit" << std::endl;

        int menuVal = 0;
        std::cin >> menuVal;

        switch(menuVal)
        {
            /* Start recording */
            case 1:
            {
                std::cout << " - Recording - \nSeconds:" << std::endl;
                size_t seconds = 0;
                std::cin >> seconds;

                if(seconds > SECONDS_MIN && seconds < SECONDS_MAX)
                {
                    rec = Record(seconds);
                }
                break;
            }

            /* Generate sinus wave */
            case 2:
            {
                std::cout << " - Generating sine wave - \nSeconds:" << std::endl;
                size_t seconds = 0;
                std::cin >> seconds;

                std::cout << "Frequency:" << std::endl;
                size_t freq = 0;
                std::cin >> freq;

                std::cout << "Sample rate (0 for default 44100):" << std::endl;
                size_t sampleRate = 0;
                std::cin >> sampleRate;
                if(sampleRate == 0)
                    sampleRate = SAMPLE_RATE;

                std::cout << "Number of channels (0 for default 2-channels stereo):" << std::endl;
                size_t numChannels = 0;
                std::cin >> numChannels;
                if(numChannels == 0)
                    numChannels = STEREO;

                if(seconds > SECONDS_MIN && seconds < SECONDS_MAX && freq > FREQ_MIN
                   && freq < FREQ_MAX && numChannels >= MONO && numChannels <= STEREO)
                {
                    rec = Generate_Sine(freq, seconds, sampleRate, numChannels);
                }
            }
            break;

            /* Playback recorded audio */
            case 3:
            {
                std::cout << " - Playback - " << std::endl;
                if(rec.isValid())
                {
                    Playback(rec);
                }
                else
                {
                    std::cout << "Must record valid audio" << std::endl;
                }
                break;
            }

            /* Save recorded .wav file */
            case 4:
            {
                std::cout << " - Saving recording - \nFilename:" << std::endl;
                std::string filename{};
                std::cin >> filename;

                if(rec.isValid())
                {
                    SaveToWav(filename, rec);
                }
                else
                {
                    std::cout << "Must record valid audio" << std::endl;
                }
                break;
            }

            /* Load and Playback .wav file */
            case 5:
            {
                std::cout << " - Load & Playback - \nFilename:" << std::endl;
                std::string filename{};
                std::cin >> filename;
                rec.clear();

                try
                {
                    rec = LoadFromWav(filename);
                }
                catch(const std::exception& ex)
                {
                    std::cout << "Could not read .wav file" << ex.what() << std::endl;
                }

                if(rec.isValid())
                {
                    // Playback(rec);
                }
                else
                {
                    std::cout << "Must read valid audio" << std::endl;
                }
                break;
            }

            case 6:
            {
                if(rec.isValid())
                {
                    analyse_rythme(rec);
                }
                else
                {
                    std::cout << "Must read valid audio" << std::endl;
                }

                break;
            }

            case 7:
            {
                if(rec.isValid())
                {
                    int freq                  = static_cast<int>(FindFrequency(rec));
                    auto [NoteName, NoteType] = FindNoteFromFreq(freq);
                    std::cout << "Main frequency of " << NoteName << " signal: " << freq
                              << std::endl;
                }
                else
                {
                    std::cout << "Must record valid audio" << std::endl;
                }

                break;
            }

            /* Exit */
            case 0:
                [[fallthrough]];
            default:
                return;
        }
    }
}

void setupFPGA()
{
    FPGA::Init();

    // clang-format off
    FPGA::setPhonemeCallback(Phoneme::a,  []{std::cout << "Phoneme A\n"  << std::endl;});
    FPGA::setPhonemeCallback(Phoneme::ey, []{std::cout << "Phoneme EY\n" << std::endl;});
    FPGA::setPhonemeCallback(Phoneme::ae, []{std::cout << "Phoneme AE\n" << std::endl;});
    FPGA::setPhonemeCallback(Phoneme::i,  []{std::cout << "Phoneme I\n"  << std::endl;});
    // clang-format on

    FPGA::WriteLED(0xFF);
    if(!FPGA::isOk())
    {
        std::cerr << "FPGA Connection Failed: " << FPGA::errorMsg() << std::endl;
        FPGA::DeInit();
        // throw std::exception();
    }
    else
    {
        std::cout << "FPGA Connection Successful" << std::endl;
    }

    FPGA::StartListener();
}


void setupPortaudio()
{
#ifndef LINUX_
    /* Initialise portaudio */
    PaError err = Pa_Initialize();

    /* Check for errors */
    if(err != paNoError)
    {
        std::cerr << "An error occured while using the portaudio stream\n"
                  << "Error number: " << static_cast<int>(err) << '\n'
                  << "Error message: " << Pa_GetErrorText(err) << std::endl;
        throw std::exception();
    }


    std::cout << "Portaudio Initialized successfully" << std::endl;
#endif
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
