#pragma once

#include "globaldef.h"
#include "note.h"

class NoteWidget
{
private:
    Note m_note;

    QLabel* m_noteHead  = nullptr;
    QLabel* m_noteSharp = nullptr;
    int64_t m_tailLength;

    int m_x;
    int m_y;

    bool liee;

public:
    NoteWidget() = delete;
    NoteWidget(QWidget* parent, Note note, int x, int ligne)
    : m_note{note}, m_x{x}, m_y{LookupNoteHeight() + (150 * ligne)}
    {
        /* Setup note head*/
        m_noteHead         = new QLabel(parent);
        QPixmap headPixmap = LookupNoteHead().scaled(30, 65, Qt::KeepAspectRatio);
        m_noteHead->setPixmap(headPixmap);
        m_noteHead->setMask(headPixmap.mask());
        m_noteHead->setGeometry(QRect{m_x, m_y, headPixmap.width(), headPixmap.height()});
        m_noteHead->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
    }
    ~NoteWidget()
    {
        clear();
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
    }
    void lieeNote(bool val)
    {
        liee = val;
    }
    void clear()
    {
        if(m_noteHead)
        {
            m_noteHead->close();
            m_noteHead = nullptr;
        }
        if(m_noteSharp)
        {
            m_noteSharp->close();
            m_noteSharp = nullptr;
        }
    }

private:
    int LookupNoteHeight()
    {
        if(m_note.noteValue == NoteValue::UNKNOWN)
        {
            // HAUTEUR DE LA PAUSE
            // A CHANGER POUR UNE VRAIE VALEUR
            // @TODO
            return 100;
        }
        else
        {
            static constexpr int64_t multiplier = 5;
            static constexpr int64_t offset     = 19;
            static constexpr std::array<int64_t, static_cast<int64_t>(NoteValue::UNKNOWN)>
              lookupTable{0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  8,  8,
                          9,  9,  10, 10, 11, 12, 12, 13, 13, 14, 15, 16, 17, 17, 18,
                          18, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 27};

            return multiplier * (lookupTable[static_cast<int64_t>(m_note.noteValue)] + offset);
        }
    }

    QPixmap LookupNoteHead()
    {
        /* Check if pause or note */
        if(m_note.noteValue == NoteValue::UNKNOWN)
        {
            static const std::array<QString, static_cast<int64_t>(NoteType::UNKNOWN)> lookupTable{
              "images/pause.png",
              "images/demi_pause.png",
              "images/silence.png",
              "images/demi_silence.png",
              "images/quart_silence.png",
            };
            return QPixmap{lookupTable[static_cast<int64_t>(m_note.noteType)]};
        }
        else
        {
            static const std::array<QString, static_cast<int64_t>(NoteType::UNKNOWN)> lookupTable{
              "images/ronde.png",
              "images/blanche.png",
              "images/noire.png",
              "images/croche.png", /* Les croches */
              "images/noireHead.png"};
            return QPixmap{lookupTable[static_cast<int64_t>(m_note.noteType)]};
        }
    }
};