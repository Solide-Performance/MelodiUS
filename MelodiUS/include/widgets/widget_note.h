#pragma once

#include "globaldef.h"
#include "note.h"

class NoteWidget
{
private:
    Note m_note;

    QLabel* m_noteHead    = nullptr;
    QLabel* m_noteTail    = nullptr;
    QLabel* m_noteTailEnd = nullptr;
    QLabel* m_noteSharp   = nullptr;
    int64_t m_tailLength;

    int m_x;
    int m_y;


public:
    NoteWidget() = delete;
    NoteWidget(QWidget* parent, Note note, int x)
    : m_note{note}, m_x{x}, m_y{LookupNoteHeight(note.noteValue)}
    {
        /* Setup note head*/
        m_noteHead         = new QLabel(parent);
        QPixmap headPixmap = LookupNoteHead(note.noteType);
        m_noteHead->setPixmap(headPixmap);
        m_noteHead->setMask(headPixmap.mask());
        m_noteHead->setGeometry(QRect{m_x, m_y, headPixmap.width(), headPixmap.height()});
    }
    NoteType getNoteType()
    {
        return m_note.getNoteType();
    }
    NoteValue getNoteValue()
    {
        return m_note.getNoteValue();
    }
    void show()
    {
        if(m_noteHead)
        {
            m_noteHead->show();
        }
        if(m_noteSharp)
        {
            m_noteSharp->show();
        }
        if(m_noteTail)
        {
            m_noteTail->show();
        }
        if(m_noteTailEnd)
        {
            m_noteTailEnd->show();
        }
    }

    void hide()
    {
        if(m_noteHead)
        {
            m_noteHead->hide();
        }
        if(m_noteSharp)
        {
            m_noteSharp->hide();
        }
        if(m_noteTail)
        {
            m_noteTail->hide();
        }
        if(m_noteTailEnd)
        {
            m_noteTailEnd->hide();
        }
    }

private:
    static int LookupNoteHeight(NoteValue note)
    {
        static constexpr int64_t multiplier = 5;
        static constexpr int64_t offset     = 19;
        static constexpr std::array<int64_t, static_cast<int64_t>(NoteValue::UNKNOWN)> lookupTable{
          0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  8,  8,
          9,  9,  10, 10, 11, 12, 12, 13, 13, 14, 15, 16, 17, 17, 18,
          18, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 27};

        return multiplier * (lookupTable[static_cast<int64_t>(note)] + offset);
    }

    static QPixmap LookupNoteHead(NoteType note)
    {
        static const std::array<QString, static_cast<int64_t>(NoteValue::UNKNOWN)> lookupTable{
          "images/ronde.png",
          "images/blancheHead.png",
          "images/noireHead.png",
          "images/noireHead.png", /* Les croches */
          "images/noireHead.png",
          "images/pause.png",
          "images/demiPause.png",
          "images/silence.png",
          "images/demiSilence.png",
          "images/quartSilence.png",
        };

        return QPixmap{lookupTable[static_cast<int64_t>(note)]};
    }
};