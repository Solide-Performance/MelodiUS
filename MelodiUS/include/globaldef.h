#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <numbers>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include <gcem/gcem.hpp>

#pragma region Qt Includes
#include <QDebug>
#include <QtCore/QTimer>
#include <QtGlobal>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMenu>
#include <QtWidgets/QVBoxLayout>



#if QT_VERSION >= 0x060000
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif
#pragma endregion



/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define sizeof_array(x)    static_cast<size_t>(sizeof(x) / sizeof((x)[0]))    // NOLINT
#define LABEL_TO_STRING(x) #x                                                 // NOLINT


template<typename T>
constexpr void SAFE_DELETE(T** x)
{
    if(x && *x)
    {
        try
        {
            delete *x;
            *x = nullptr;
        }
        catch(...)
        {
        }
    }
}


/* Epsilon is a margin between the two values */
template<typename T>
constexpr bool COMPARE_VALUES(const T f1, const T f2, const T epsilon = 0.0005)
{
    /* Due to floating point representation imprecision, two floating-point values should never be
     * compared together directly. This function can be used for integer values as well, as a simple
     * margin. */
    return std::abs(f1 - f2) <= epsilon;
}


template<typename floating>
constexpr floating db_to_lin(floating dB)
{
    static_assert(std::is_floating_point<floating>::value,
                  "Argument must be a floating point type");

    return gcem::pow(10.0, dB / 20.0);
}

template<typename floating>
constexpr floating lin_to_db(floating lin)
{
    static_assert(std::is_floating_point<floating>::value,
                  "Argument must be a floating point type");

    /* gcem::log is base e, we need to rebase it. */
    return 20 * (gcem::log(lin) / gcem::log(10.0));
}


/*****************************************************************************/
/* Utility ----------------------------------------------------------------- */
/* clang-format off */
static auto EMPTY_FUNCTION = []{};
/* clang-format on */


/*****************************************************************************/
/* Compiler stuff ---------------------------------------------------------- */
#ifndef LINUX_
#define INLINE __forceinline
#else
#define INLINE
#endif


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
