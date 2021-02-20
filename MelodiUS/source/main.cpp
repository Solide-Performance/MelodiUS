/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "recorder.h"
#include "recording.h"

/*****************************************************************************/
/* Entry point ------------------------------------------------------------- */
int main()
{
    Recording recordedBonMatin = Record();

    SaveToWav("bon matin.wav", recordedBonMatin);

    Recording recordedBonMatin2 = LoadFromWav("bon matin.wav");
    SaveToWav("bon matin2.wav", recordedBonMatin2);

    return 1;
}

/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
