/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fft.h"
#include "generator.h"
#include "globaldef.h"
#include "playback.h"
#include "readwrite_wav.h"
#include "recorder.h"
#include "recording.h"

#include "fpga.h"

#include "portaudio.h"

#include <iostream>
#include <string>
#include <string_view>


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
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
int main()
{
    /* portaudio init */
    setupPortaudio();

    /* CommunicationFPGA init */
    setupFPGA();

    /* Menu display and command handling */
    menuHandler();

    /* Close portaudio & FPGA*/
    Pa_Terminate();
    FPGA::DeInit();
    return 0;
}


/*****************************************************************************/
/* Function definitions ---------------------------------------------------- */
void menuHandler()
{
    Recording rec;

    WAV_Reader C{"C.wav"};
    WAV_Reader D{"D.wav"};
    WAV_Reader D0{"D0.wav"};
    WAV_Reader E{"E.wav"};
    WAV_Reader E_maybe{"E_maybe.wav"};
    WAV_Reader E2{"E2.wav"};
    WAV_Reader G{"G.wav"};
    WAV_Reader G_maybe{"G_maybe.wav"};

    while(true)
    {
        std::cout << std::endl;
        std::cout << "\n ----- MelodiUS ----- \n";
        std::cout << "1 - Start recording\n";
        std::cout << "2 - Generate sinus wave\n";
        std::cout << "3 - Playback recorded audio\n";
        std::cout << "4 - Save recorded .wav file\n";
        std::cout << "5 - Load & Playback .wav file\n";
        std::cout << "6 - Find main frequency of signal\n";
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

                std::cout << "Number of channels" << std::endl;
                size_t numChannels = 0;
                std::cin >> numChannels;

                if(seconds > SECONDS_MIN && seconds < SECONDS_MAX && freq > FREQ_MIN
                   && freq < FREQ_MAX && numChannels >= MONO && numChannels <= STEREO)
                {
                    rec = Generate_Sine(freq, seconds, numChannels);
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
                SaveToWav(filename, rec);
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
                    Playback(rec);
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
                    size_t freq = FindFrequency(rec);
                    std::cout << "Main frequency of signal: " << freq << std::endl;
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

    FPGA::WriteLED(0xFF);
    if(!FPGA::isOk())
    {
        std::cerr << "FPGA Connection Failed: " << FPGA::errorMsg() << std::endl;
        // throw std::exception();
    }
    else
    {
        std::cout << "FPGA Connection Successful" << std::endl;
    }
}


void setupPortaudio()
{
    PaError err = Pa_Initialize();
    if(err != paNoError)
    {
        std::cerr << "An error occured while using the portaudio stream\n"
                  << "Error number: " << static_cast<int>(err) << '\n'
                  << "Error message: " << Pa_GetErrorText(err) << std::endl;
        throw std::exception();
    }
    else
    {
        // Clears screen, from:
        // https://stackoverflow.com/a/33450696/10827197
        system("@cls||clear");
        std::cout << "Portaudio Initialized" << std::endl;
    }
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
