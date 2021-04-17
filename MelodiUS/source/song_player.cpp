#include "song_player.h"
#include "generator.h"
#include "playback.h"
#include "recording.h"
#include "tuning.h"

double LengthFromTempo(NoteType noteType, double bpm)
{
    double bps = bpm / 60.;
    switch(noteType)
    {
        case NoteType::Ronde:
            return bps * 4.;
            break;
        case NoteType::Blanche:
            return bps * 2.;
            break;
        case NoteType::Noire:
            return bps;
            break;
        case NoteType::Croche:
            return bps / 2.;
            break;
        case NoteType::DoubleCroche:
            return bps / 4.;
            break;

        case NoteType::UNKNOWN:
            [[fallthrough]];
        default:
            return 0.;
            break;
    }
}

void Play_Song(const std::vector<Note>& notes, double bpm)
{
    for(Note note : notes)
    {
        double duration = LengthFromTempo(note.noteType, bpm);
        double freq = FindFreqFromNote(note.noteValue);
        Recording playedNote = Generate_Sine(freq, duration);
        Playback(playedNote);
    }
}
