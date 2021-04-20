#pragma once
#include "globaldef.h"

class Portee
{
public:
    Portee() = delete;
    Portee(const Portee& other)
    : Cle{(QWidget*)other.Cle.parent()},
      lines{QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()},
            QFrame{(QWidget*)other.Cle.parent()}}
    {
        for(int i = 0; i <= lines.size() - 1; i++)
        {
            lines[i].setGeometry(other.lines[i].geometry());
            lines[i].setFrameShape(other.lines[i].frameShape());
            lines[i].setFrameShadow(other.lines[i].frameShadow());
            lines[i].show();
        }
        Cle.setGeometry(other.Cle.geometry());

#if QT_VERSION >= 0x060000
        Cle.setPixmap(other.Cle.pixmap());
#else
        Cle.setPixmap(*other.Cle.pixmap());
#endif

        Cle.show();
        Cle.lower();
    }

    Portee(int n, QWidget* parent)
    : Cle(parent),
      lines{QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent},
            QFrame{parent}}
    {
        int decalage = (350 * n);
        ///=== LIGNE HORIZONTALE ========//
        for(int i = 0; i < 5; i++)
        {
            lines[i].setGeometry(QRect(20, 160 + (20 * i) + decalage, 938, 21));
            lines[i].setFrameShape(QFrame::HLine);
            lines[i].setFrameShadow(QFrame::Plain);
        }
        ///=== LIGNE VERTICALE ========//
        for(int i = 6; i < 10; i++)
        {
            lines[i].setGeometry(QRect(215.75 * (i - 5)+85, 170 + decalage, 20, 81));
            lines[i].setFrameShape(QFrame::VLine);
            lines[i].setFrameShadow(QFrame::Plain);
        }
        lines[5].setGeometry(QRect(10, 170 + decalage, 20, 81));    // debut
        lines[5].setFrameShape(QFrame::VLine);
        lines[5].setFrameShadow(QFrame::Plain);

        Cle.setGeometry(QRect(10, 155 + decalage, 70, 115));
        Cle.setPixmap(QPixmap(QString::fromUtf8("images/clef_sol.png")).scaled(70, 115, Qt::KeepAspectRatio));
        Cle.lower();
    };


    std::array<QFrame, 10> lines;
    QLabel                 Cle;
};
