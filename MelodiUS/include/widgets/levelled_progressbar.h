#pragma once
#include "globaldef.h"


class LevelledProgressBar : public QProgressBar
{
public:
    LevelledProgressBar(QWidget*        parent,
                        int             max         = 255,
                        int             min         = 0,
                        Qt::Orientation orientation = Qt::Vertical)
    : QProgressBar{parent},
      m_level{parent},
      m_levelVal{(max - min) / 2},
      m_maxVal{max},
      m_oldPos{0, 0, 0, 0}
    {
        setMaximum(max);
        setMinimum(min);
        setValue(m_levelVal);
        setOrientation(orientation);
        setTextVisible(true);

        m_level.setFrameShape(QFrame::HLine);
        m_level.setFrameShadow(QFrame::Plain);
        RepositionLevel();
    }

    void ChangeVal(int newVal)
    {
        m_levelVal = newVal;
        RepositionLevel();
    }

    void paintEvent(QPaintEvent* event) override
    {
        QProgressBar::paintEvent(event);

        RepositionLevel();
    }


private:
    void RepositionLevel()
    {
        QRect position = geometry();

        if(position == m_oldPos)
        {
            return;
        }
        m_oldPos = position;

        /* clang-format off */
        int   frameHeight = position.height() -
                            static_cast<double>(m_levelVal) / static_cast<double>(m_maxVal) *
                            position.height();

        m_level.setGeometry(position.left() - 1,
                            position.top() + frameHeight,
                            position.width() + 2,
                            21);
        /* clang-format on */
    }

    QFrame m_level;
    QRect  m_oldPos;
    int    m_levelVal;
    int    m_maxVal;
};
