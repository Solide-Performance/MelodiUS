#pragma once

#include "globaldef.h"
#include "note.h"
#include <iostream>

class NoteWidget
{
private:
    Note m_note;

    QLabel* m_noteHead        = nullptr;
    QLabel* m_legato          = nullptr;
    QLabel* m_diese           = nullptr;
    QFrame* m_ledger          = nullptr;
    QWidget* m_ligneDeuxCroche = nullptr;
    QLabel* m_ziglouigloui    = nullptr;
    int64_t m_tailLength;

    int       m_x;
    int       m_y;
    int       theLastOfX;
    int       theLastOfY;
    NoteValue theLastNoteValue;

    bool deuxCroche;

public:
    NoteWidget() = delete;
    NoteWidget(QWidget*  parent,
               Note      note,
               int       x,
               int       ligne,
               int       theLastOfX,
               int       theLastOfy,
               NoteValue lastNoteValue)
    : m_note{note},
      m_x{x},
      m_y{LookupNoteHeight() + (350 * ligne)},
      theLastOfX{theLastOfX},
      theLastOfY{theLastOfy},
      theLastNoteValue{lastNoteValue},
      deuxCroche{note.deuxCroche}
    {
        if(note.getNoteValue() == NoteValue::UNKNOWN)
        {
            if(note.getNoteType() == NoteType::Pause)
            {
                m_y = 187 + (350 * ligne);
            }
            else if(note.getNoteType() == NoteType::DemiPause)
            {
                m_y = 199 + (350 * ligne);
            }
        }
        /* Setup note head*/
        m_noteHead = new QLabel(parent);
        m_noteHead->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
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
        if(note.noteType == NoteType::Croche)
        {
            if(deuxCroche&&note.deuxiemeDeuxCroche)
            {
                m_ligneDeuxCroche = new QFrame(parent);
                m_ligneDeuxCroche->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
                if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45
                   && (int)theLastNoteValue >= 19 && (int)theLastNoteValue != 45)
                {
                    QPoint p1(theLastOfX, theLastOfY + 65);
                    QPoint p2(m_x, m_y + 65);
                    QPainter h(parent);
                    h.begin(parent);
                    h.setPen(Qt::black);
                    h.drawLine(p1, p2);
                    std::cout << h.isActive();
                }
                else if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45
                        && (int)theLastNoteValue < 19)
                {
                    QPoint p1(theLastOfX, theLastOfY + 5);
                    QPoint p2(m_x, m_y + 65);
                    QPainter h(parent);
                    h.begin(parent);
                    h.setPen(Qt::black);
                    h.drawLine(p1, p2);
                    std::cout << h.isActive();
                    
                }
                else if((int)note.getNoteValue() < 19 && (int)theLastNoteValue != 45
                        && (int)theLastNoteValue >= 19)
                {
                    QPoint p1(theLastOfX, theLastOfY + 65);
                    QPoint p2(m_x, m_y + 5);
                    QPainter h(parent);
                    h.begin(parent);
                    h.setPen(Qt::black);
                    h.drawLine(p1, p2);
                    std::cout << h.isActive();
                }
                else
                {
                    QPoint p1(theLastOfX, theLastOfY + 5);
                    QPoint p2(m_x, m_y + 5);
                    QPainter h(parent);
                    h.begin(parent);
                    h.setPen(Qt::black);
                    h.drawLine(p1, p2);
                    std::cout << h.isActive();
                    
                }
                
            }else if(!deuxCroche)
            {
                if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45)
                {
                    m_ziglouigloui = new QLabel(parent);
                    QPixmap z      = QPixmap("images/Ziglouigloui_bas.png");
                    m_ziglouigloui->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
                    z = z.scaled(20, 50, Qt::KeepAspectRatio);
                    m_ziglouigloui->setPixmap(z);
                    m_ziglouigloui->setGeometry(QRect{m_x + 3, m_y + 22, z.width(), z.height()});
                    m_ziglouigloui->show();
                }
                else
                {
                    m_ziglouigloui = new QLabel(parent);
                    QPixmap z = QPixmap("images/Ziglouigloui.png");
                    m_ziglouigloui->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
                    z = z.scaled(20, 50, Qt::KeepAspectRatio);
                    m_ziglouigloui->setPixmap(z);
                    m_ziglouigloui->setGeometry(QRect{m_x+22,m_y-3,z.width(),z.height()});
                    m_ziglouigloui->show();
                }
            }
        }
        if(note.liee && note.getNoteValue() != NoteValue::UNKNOWN)
        {
            m_legato = new QLabel(parent);
            m_legato->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
            if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45)
            {
                QPixmap h = QPixmap("images/legato.png");
                h         = h.scaled(m_x - theLastOfX, h.height(), Qt::IgnoreAspectRatio);
                m_legato->setPixmap(h);
                m_legato->setGeometry(
                  QRect{theLastOfX + 10, m_y - 25, m_x - theLastOfX, h.height()});
                m_legato->show();
                std::cout << m_y << std::endl;
            }
            else
            {
                QPixmap h = QPixmap("images/legato.png");
                h         = h.transformed(QTransform().rotate(180));
                h         = h.scaled(m_x - theLastOfX, h.height(), Qt::IgnoreAspectRatio);
                m_legato->setPixmap(h);
                m_legato->setGeometry(
                  QRect{theLastOfX + 10, m_y + 55, m_x - theLastOfX, h.height()});
                m_legato->show();
            }
        }
        if(note.isSharp())
        {
            m_diese = new QLabel(parent);
            m_diese->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
            QPixmap d = QPixmap("images/diese.png");
            d         = d.scaled(20, 25, Qt::IgnoreAspectRatio);
            m_diese->setPixmap(d);
            if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45)
            {
                m_diese->setGeometry(QRect{m_x - 20, m_y, d.width(), d.height()});
            }
            else
            {
                m_diese->setGeometry(QRect{m_x - 20, m_y + 40, d.width(), d.height()});
            }
            m_diese->show();
        }
        int ledger = LookupNoteLedger(note.getNoteValue());
        switch(ledger)
        {
            case 0:
                break;
            case 1:
                m_ledger = new QFrame(parent);
                m_ledger->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
                if((int)note.getNoteValue() >= 19 && (int)note.getNoteValue() != 45)
                {
                    m_ledger->setGeometry(QRect(m_x - 5, m_y - 40, 40, 100));
                    m_ledger->setFrameShape(QFrame::HLine);
                    m_ledger->setFrameShadow(QFrame::Plain);
                    m_ledger->show();
                }
                else
                {
                    m_ledger->setGeometry(QRect(m_x - 5, m_y + 7, 40, 100));
                    m_ledger->setFrameShape(QFrame::HLine);
                    m_ledger->setFrameShadow(QFrame::Plain);
                    m_ledger->show();
                }
                break;
            case 2:
                m_ledger = new QFrame(parent);
                m_ledger->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
                m_ledger->setGeometry(QRect(m_x - 5, m_y + 14, 40, 100));
                m_ledger->setFrameShape(QFrame::HLine);
                m_ledger->setFrameShadow(QFrame::Plain);
                m_ledger->show();
                break;
            case 3:
                m_ledger = new QFrame(parent);
                m_ledger->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
                m_ledger->setGeometry(QRect(m_x - 5, m_y - 47, 40, 100));
                m_ledger->setFrameShape(QFrame::HLine);
                m_ledger->setFrameShadow(QFrame::Plain);
                m_ledger->show();
                break;
            default:
                break;
        }
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
    }
    int getX()
    {
        return m_x;
    }
    int getY()
    {
        return m_y;
    }
    void hide()
    {
        if(m_noteHead)
        {
            m_noteHead->hide();
        }
    }
    void clear()
    {
        if(m_noteHead)
        {
            m_noteHead->close();
            m_noteHead = nullptr;
        }
        if(m_diese)
        {
            m_diese->close();
            m_diese = nullptr;
        }
        if(m_legato)
        {
            m_legato->close();
            m_legato = nullptr;
        }
        if(m_ledger)
        {
            m_ledger->close();
            m_ledger = nullptr;
        }
        if(m_ligneDeuxCroche)
        {
            m_ligneDeuxCroche->close();
            m_ligneDeuxCroche = nullptr;
        }
    }

private:
    int LookupNoteHeight()
    {
        static constexpr std::array<int64_t, static_cast<int64_t>(NoteValue::UNKNOWN) + 1>
          lookupTable{235, 225, 225, 215, 215, 205, 205, 195, 185, 185, 175, 175,
                      165, 155, 155, 145, 145, 135, 135, 170, 160, 160, 150, 150,
                      140, 130, 130, 120, 120, 110, 110, 100, 90,  90,  80,  80,
                      70,  60,  60,  50,  50,  40,  40,  30,  20,  150};

        return lookupTable[static_cast<int64_t>(m_note.noteValue)] + 30;
    }
    static int LookupNoteLedger(NoteValue note)
    {
        static constexpr std::array<int64_t, static_cast<int64_t>(NoteValue::UNKNOWN) + 1>
          lookupTable{2, 1, 1, 2, 2, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 1, 1, 3, 1, 1, 3, 3, 1, 3, 3, 1, 1, 3, 3, 1, 3, 0};
        return lookupTable[static_cast<int64_t>(note)];
    }
    static QPixmap LookupNoteHead(NoteType note)
    {
        static const std::array<QString, static_cast<int64_t>(NoteValue::UNKNOWN)> lookupTable{
          "images/ronde.png",
          "images/blanche.png",
          "images/noire.png",
          "images/noire.png", /* Les croches */
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
          "images/noire_bas.png", /* Les croches */
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