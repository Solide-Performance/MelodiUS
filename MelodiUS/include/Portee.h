#pragma once
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

class Portee
{
public:
    Portee(int n, QGroupBox* GroupBox)
    : Cle(GroupBox),
      line(GroupBox),
      line_2(GroupBox),
      line_3(GroupBox),
      line_4(GroupBox),
      line_5(GroupBox),
      line_6(GroupBox),
      line_7(GroupBox),
      line_8(GroupBox),
      line_9(GroupBox),
      line_10(GroupBox)
    {
        ///=== LIGNE HORIZONTALE ========//
        
        line.setGeometry(QRect(20, 130 + (150 * n), 950, 21));
        line.setFrameShape(QFrame::HLine);
        line.setFrameShadow(QFrame::Plain);

        line_2.setGeometry(QRect(20, 150 + (150 * n), 950, 21));
        line_2.setFrameShape(QFrame::HLine);
        line_2.setFrameShadow(QFrame::Plain);

        line_3.setGeometry(QRect(20, 170 + (150 * n), 950, 21));
        line_3.setFrameShape(QFrame::HLine);
        line_3.setFrameShadow(QFrame::Plain);

        line_4.setGeometry(QRect(20, 190 + (150 * n), 950, 21));
        line_4.setFrameShape(QFrame::HLine);
        line_4.setFrameShadow(QFrame::Plain);

        line_5.setGeometry(QRect(20, 210 + (150 * n), 950, 21));
        line_5.setFrameShape(QFrame::HLine);
        line_5.setFrameShadow(QFrame::Plain);
        ///=== LIGNE VERTICALE ========//

        line_6.setGeometry(QRect(10, 140 + (150 * n), 20, 81));    // debut
        line_6.setFrameShape(QFrame::VLine);
        line_6.setFrameShadow(QFrame::Plain);

        line_7.setGeometry(QRect(237, 140 + (150 * n), 20, 81));
        line_7.setFrameShape(QFrame::VLine);
        line_7.setFrameShadow(QFrame::Plain);

        line_8.setGeometry(QRect(237 * 2, 140 + (150 * n), 20, 81));
        line_8.setFrameShape(QFrame::VLine);
        line_8.setFrameShadow(QFrame::Plain);

        line_10.setGeometry(QRect(237 * 3, 140 + (150 * n), 20, 81));
        line_10.setFrameShape(QFrame::VLine);
        line_10.setFrameShadow(QFrame::Plain);

        line_9.setGeometry(QRect(960, 140 + (150 * n), 20, 81));
        line_9.setFrameShape(QFrame::VLine);
        line_9.setFrameShadow(QFrame::Plain);

        Cle.setGeometry(QRect(10, 130 +(150 * n), 51, 101));
        Cle.setPixmap(QPixmap(QString::fromUtf8("images/cle40x80T.png")));
    };
    QFrame line;
    QFrame line_2;
    QFrame line_3;
    QFrame line_4;
    QFrame line_5;
    QFrame line_6;
    QFrame line_7;
    QFrame line_8;
    QFrame line_9;
    QFrame line_10;
    QLabel Cle;
};
