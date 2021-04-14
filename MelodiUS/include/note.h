#pragma once
#include <array>

#pragma region Note type and value enums
enum class NoteType
{
    Ronde = 0,
    Blanche,
    Noire,
    Croche,
    DoubleCroche,
    Pause,
    DemiPause,
    Silence,
    DemiSilence,
    QuartSilence,
    UNKNOWN
};

enum class NoteValue : int64_t
{
    E2 = 0,
    F2,
    Fs2,
    G2,
    Gs2,
    A2,
    As2,
    B2,
    C3,
    Cs3,
    D3,
    Ds3,
    E3,
    F3,
    Fs3,
    G3,
    Gs3,
    A3,
    As3,
    B3,
    C4,
    Cs4,
    D4,
    Ds4,
    E4,
    F4,
    Fs4,
    G4,
    Gs4,
    A4,
    As4,
    B4,
    C5,
    Cs5,
    D5,
    Ds5,
    E5,
    F5,
    Fs5,
    G5,
    Gs5,
    A5,
    As5,
    B5,
    C6,
    UNKNOWN
};
#pragma endregion


class Note
{
public:
    const NoteType  noteType;
    const NoteValue noteValue;
    bool            liee;

    constexpr Note() : noteType{NoteType::UNKNOWN}, noteValue{NoteValue::UNKNOWN}
    {
        liee = false;
    }
    constexpr Note(NoteType nt, NoteValue nv,bool lieee) : noteType{nt}, noteValue{nv}, liee{lieee}
    {
    }
    constexpr Note(NoteType nt, NoteValue nv) : noteType{nt}, noteValue{nv}
    {
        liee = false;
    }
    NoteType getNoteType()
    {
        return noteType;
    }
    NoteValue getNoteValue()
    {
        return noteValue;
    }
    double getNoteSum()
    {
        if(noteType == NoteType::Ronde)
        {
            return 4;
        }
        else if(noteType == NoteType::Blanche)
        {
            return 2;
        }
        else if(noteType == NoteType::Noire)
        {
            return 1;
        }
        else if(noteType == NoteType::Croche)
        {
            return 0.5;
        }
        else if(noteType == NoteType::DoubleCroche)
        {
            return 0.25;
        }
        else if(noteType == NoteType::Pause)
        {
            return 4;
        }
        else if(noteType == NoteType::DemiPause)
        {
            return 2;
        }
        else if(noteType == NoteType::Silence)
        {
            return 1;
        }
        else if(noteType == NoteType::DemiSilence)
        {
            return 0.5;
        }
        else if(noteType == NoteType::QuartSilence)
        {
            return 0.25;
        }
    }
    bool isLiee()
    {
        return liee;
    }
    void setLiee(bool val)
    {
        liee = val;
    }
    bool isSharp()
    {
        static std::array<bool, static_cast<int64_t>(NoteValue::UNKNOWN) + 1> lookupTable{
          false, false, true, false, true, false, true, false, false, true, false, true,
          false, false, true, false, true, false, true, false, false, true, false, true,
          false, false, true, false, true, false, true, false, false, true, false, true,
          false, false, true, false, true, false, true, false, false, false};

        return lookupTable[static_cast<int64_t>(noteValue)];
    }
};

