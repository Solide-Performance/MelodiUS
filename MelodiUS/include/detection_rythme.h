#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "recording.h"
#include "note.h"

/*****************************************************************************/
/* Function declarations --------------------------------------------------- */
std::vector<Recording> analyse_rythme(const Recording& rec);
void                   analyse_note(std::vector<size_t>    debuts,
                                    std::vector<size_t>    fins,
                                    size_t                 recordingLength,
                                    std::vector<NoteValue> valeur_note);


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
