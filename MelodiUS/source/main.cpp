/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "playback.h"
#include "readwrite_wav.h"
#include "recorder.h"
#include "recording.h"

#include <iostream>
#include <string>
#include <string_view>


/*****************************************************************************/
/* Entry point ------------------------------------------------------------- */
int main()
{
    Recording rec;


    while(true)
    {
        std::cout << std::endl;
        std::cout << "\n ----- MelodiUS ----- \n";
        std::cout << "1 - Start recording\n";
        std::cout << "2 - Generate sinus wave\n";
        std::cout << "3 - Playback recorded audio\n";
        std::cout << "4 - Save recorded .wav file\n";
        std::cout << "5 - Load & Playback .wav file\n";
        std::cout << "0 - Exit" << std::endl;

        int menuVal = 0;
        std::cin >> menuVal;

        switch(menuVal)
        {
            /* Start recording */
            case 1:
            {
                std::cout << " - Recording - \nSeconds:" << std::endl;
                int seconds = 0;
                std::cin >> seconds;

                if(seconds > 0 && seconds < 60)
                {
                    rec = Record(seconds);
                }
                break;
            }

            /* Generate sinus wave */
            case 2:
                // TODO
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

            /* Exit */
            case 0:
                [[fallthrough]];
            default:
                return 0;
        }
    }

    return 1;
}

/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
