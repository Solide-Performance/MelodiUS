#pragma once
#include <array>

#include <QtGlobal>
#if QT_VERSION >= 0x060000
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif
#include <QtCore/QVariant>
#include <QtGui/QBitmap>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollBar>

class Portee
{
public:
    Portee() = delete;
    Portee(const Portee& other)
    : Cle{(QGroupBox*)other.Cle.parent()},
      lines{QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()},
            QFrame{(QGroupBox*)other.Cle.parent()}}
    {
        for(int i = 0; i <= lines.size() - 1; i++)
        {
            lines[i].setGeometry(other.lines[i].geometry());
            lines[i].setFrameShape(other.lines[i].frameShape());
            lines[i].setFrameShadow(other.lines[i].frameShadow());
            lines[i].show();
        }
        Cle.setGeometry(other.Cle.geometry());
        Cle.setPixmap(other.Cle.pixmap());
        Cle.show();
        Cle.lower();
    }

    Portee(int n, QGroupBox* GroupBox)
    : Cle(GroupBox),
      lines{QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox},
            QFrame{GroupBox}}
    {
        int decalage = (150 * n);
        ///=== LIGNE HORIZONTALE ========//
        for(int i = 0; i < 5; i++)
        {
            lines[i].setGeometry(QRect(20, 130 + (20 * i) + decalage, 938, 21));
            lines[i].setFrameShape(QFrame::HLine);
            lines[i].setFrameShadow(QFrame::Plain);
        }
        ///=== LIGNE VERTICALE ========//
        for(int i = 6; i < 10; i++)
        {
            lines[i].setGeometry(QRect(237 * (i - 5), 140 + decalage, 20, 81));
            lines[i].setFrameShape(QFrame::VLine);
            lines[i].setFrameShadow(QFrame::Plain);
        }
        lines[5].setGeometry(QRect(10, 140 + decalage, 20, 81));    // debut
        lines[5].setFrameShape(QFrame::VLine);
        lines[5].setFrameShadow(QFrame::Plain);

        Cle.setGeometry(QRect(10, 130 + decalage, 51, 101));
        Cle.setPixmap(QPixmap(QString::fromUtf8("images/cle40x80T.png")));
        Cle.lower();
    };
    

    std::array<QFrame, 10> lines;
    QLabel                 Cle;
};
