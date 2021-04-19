#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fpga_phoneme.h"
#include "globaldef.h"

/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
static constexpr QColor COLOR_A{255, 0, 0};    /* Red */
static constexpr QColor COLOR_EY{0, 204, 255}; /* Aqua */
static constexpr QColor COLOR_AE{51, 204, 51}; /* Dark Green */
static constexpr QColor COLOR_I{0, 0, 255};    /* Dark Blue */


/*****************************************************************************/
/* Class definition -------------------------------------------------------- */
class LevelledProgressBar : public QProgressBar
{
public:
    LevelledProgressBar(QWidget*        parent,
                        int             max         = 255,
                        int             min         = 0,
                        Qt::Orientation orientation = Qt::Vertical)
    : QProgressBar{parent},
      m_levels{QFrame{parent}, QFrame{parent}, QFrame{parent}, QFrame{parent}},
      m_levelVals{0},
      m_maxVal{max},
      m_oldPos{0, 0, 0, 0}
    {
        int levelVal = (max - min) / 2;
        m_levelVals  = {levelVal, levelVal, levelVal, levelVal};

        setMaximum(max);
        setMinimum(min);
        setValue(levelVal);
        setOrientation(orientation);
        setTextVisible(true);

        QPalette colorA{};
        colorA.setColor(QPalette::ColorRole::WindowText, COLOR_A);
        m_levels[0].setFrameShape(QFrame::HLine);
        m_levels[0].setFrameShadow(QFrame::Plain);
        m_levels[0].setPalette(colorA);

        QPalette colorEY{};
        colorEY.setColor(QPalette::ColorRole::WindowText, COLOR_EY);
        m_levels[1].setFrameShape(QFrame::HLine);
        m_levels[1].setFrameShadow(QFrame::Plain);
        m_levels[1].setPalette(colorEY);

        QPalette colorAE{};
        colorAE.setColor(QPalette::ColorRole::WindowText, COLOR_AE);
        m_levels[2].setFrameShape(QFrame::HLine);
        m_levels[2].setFrameShadow(QFrame::Plain);
        m_levels[2].setPalette(colorAE);


        QPalette colorI{};
        colorI.setColor(QPalette::ColorRole::WindowText, COLOR_I);
        m_levels[3].setFrameShape(QFrame::HLine);
        m_levels[3].setFrameShadow(QFrame::Plain);
        m_levels[3].setPalette(colorI);

        RepositionLevels(true);
    }

    void ChangeVal(const std::array<int, 4> newVal)
    {
        m_levelVals = newVal;
        RepositionLevels(true);
    }

    void paintEvent(QPaintEvent* event) override
    {
        QProgressBar::paintEvent(event);

        RepositionLevels();
    }


private:
    void RepositionLevels(bool forceUpdate = false)
    {
        QRect position = geometry();

        if(position == m_oldPos && forceUpdate == false)
        {
            return;
        }
        m_oldPos = position;

        for(size_t i = 0; i < m_levels.size(); i++)
        {

            /* clang-format off */
            int   frameHeight = position.height() -
                                static_cast<double>(m_levelVals[i]) / static_cast<double>(m_maxVal) *
                                position.height();

            m_levels[i].setGeometry(position.left() - 1,
                                    position.top() + frameHeight - 1,
                                    position.width() + 2,
                                    1);
            /* clang-format on */
        }
    }

    std::array<QFrame, 4> m_levels;
    std::array<int, 4>    m_levelVals;
    int                   m_maxVal;
    QRect                 m_oldPos;
};
