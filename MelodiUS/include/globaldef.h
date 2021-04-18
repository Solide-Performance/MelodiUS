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
#include <QFileDialog>
#include <QtCore/QTimer>
#include <QtGlobal>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
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
#include <QtWidgets/QVBoxLayout>


#if QT_VERSION >= 0x060000
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif
#pragma endregion




/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
#ifdef LINUX_
constexpr uint8_t CHAR_BIT = 8;
#endif


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define sizeof_array(x)    static_cast<size_t>(sizeof(x) / sizeof((x)[0]))    // NOLINT
#define LABEL_TO_STRING(x) #x    // NOLINT


template<typename T>
constexpr void SAFE_DELETE(T** x)
{
    if(x && *x)
    {
        delete *x;
        *x = nullptr;
    }
}


/* Epsilon is a margin between the two floating point values */
template<typename floating>
constexpr bool COMPARE_FLOATS(floating f1, floating f2, double epsilon = 0.0005)
{
    static_assert(std::is_floating_point<floating>::value,
                  "Argument must be a floating point type");

    /* Due to floating point representation imprecision, two floating-point values should never be
     * compared together */
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

    return 20 * (gcem::log(lin) / gcem::log(10.0));
}


/*****************************************************************************/
/* Utility ----------------------------------------------------------------- */
static auto EMPTY_FUNCTION = []() {
};


/*****************************************************************************/
/* Compiler stuff ---------------------------------------------------------- */
#ifndef LINUX_
#define INLINE __forceinline
#else
#define INLINE
#endif


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
