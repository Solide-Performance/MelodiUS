#pragma once

#include "globaldef.h"
#include "note.h"
#include <iostream>

class NoteWidget
{
private:
    Note m_note;

    QLabel* m_noteHead    = nullptr;
    QLabel* m_noteTail    = nullptr;
    QLabel* m_noteTailEnd = nullptr;
    QLabel* m_noteSharp   = nullptr;
    QLabel* m_legato      = nullptr;
    int64_t m_tailLength;

    int    m_x;
    int    m_y;
    int theLastOfX;

public:
    NoteWidget() = delete;
    NoteWidget(QWidget* parent, Note note, int x, int ligne, int theLastOfX )
    : m_note{note},
      m_x{x},
      m_y{LookupNoteHeight(note.noteValue) + (350 * ligne)},
      theLastOfX{theLastOfX}
    {
        /* Setup note head*/
        m_noteHead         = new QLabel(parent);
        m_legato           = new QLabel(parent);
        QPixmap headPixmap = LookupNoteHead(note.noteType).scaled(30, 65, Qt::KeepAspectRatio);
        if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45)
        {
            headPixmap = LookupNoteHeadrotated(note.noteType).scaled(30, 65, Qt::KeepAspectRatio);
        }
        if(note.getNoteType() == NoteType::Ronde)
        {
            m_y += 3;
        }
        m_noteHead->setPixmap(headPixmap);
        m_noteHead->setMask(headPixmap.mask());
        m_noteHead->setGeometry(QRect{m_x, m_y, headPixmap.width(), headPixmap.height()});

        if(note.liee)
        {
            
            if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45)
            {
                QPixmap h = QPixmap("images/legato.png");
                h = h.scaled(m_x - theLastOfX, h.height(), Qt::IgnoreAspectRatio);
                m_legato->setPixmap(h);
                m_legato->setGeometry(QRect{theLastOfX+10, m_y -25,m_x-theLastOfX, h.height()});
                m_legato->show();
                std::cout <<m_y << std::endl;
                
            }
            else
            {
                QPixmap h = QPixmap("images/legato.png");
                h         = h.transformed(QTransform().rotate(180));
                h         = h.scaled(m_x - theLastOfX, h.height(), Qt::IgnoreAspectRatio);
                m_legato->setPixmap(h);
                m_legato->setGeometry(QRect{theLastOfX+10, m_y + 55, m_x - theLastOfX, h.height()});
                m_legato->show();
            }
        }
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
    int getX()
    {
        return m_x;
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

        static constexpr std::array<int64_t, static_cast<int64_t>(NoteValue::UNKNOWN)> lookupTable{
          235, 225, 225, 215, 215, 205, 205, 195, 185, 185, 175, 175, 165, 155, 155,
          145, 145, 135, 135, 170, 160, 160, 150, 150, 140, 130, 130, 120, 120, 110,
          110, 100, 90,  90,  80,  80,  70,  60,  60,  50,  50,  40,  40,  30,  20};

        return lookupTable[static_cast<int64_t>(note)] + 30;
    }

    static QPixmap LookupNoteHead(NoteType note)
    {
        static const std::array<QString, static_cast<int64_t>(NoteValue::UNKNOWN)> lookupTable{
          "images/ronde.png",
          "images/blanche.png",
          "images/noire.png",
          "images/croche.png", /* Les croches */
          "images/noireHead.png",
          "images/pause.png",
          "images/demi_pause.png",
          "images/silence.png",
          "images/demi_silence.png",
          "images/quart_silence.png",
        };

        return QPixmap{lookupTable[static_cast<int64_t>(note)]};
    }
    static QPixmap LookupNoteHeadrotated(NoteType note)
    {
        static const std::array<QString, static_cast<int64_t>(NoteValue::UNKNOWN)> lookupTable{
          "images/ronde_bas.png",
          "images/blanche_bas.png",
          "images/noire_bas.png",
          "images/croche_bas.png", /* Les croches */
          "images/noireHead_bas.png",
          "images/pause.png",
          "images/demi_pause.png",
          "images/silence.png",
          "images/demi_silence.png",
          "images/quart_silence.png",
        };

        return QPixmap{lookupTable[static_cast<int64_t>(note)]};
    }
};