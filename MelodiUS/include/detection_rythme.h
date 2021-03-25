#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "recording.h"


/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
std::vector<Recording> analyse_rythme(const Recording& rec);
void analyse_note(std::vector<size_t> debuts, std::vector<size_t> fins, size_t recordingLength);


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
