#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#define sizeof_array(x)    static_cast<size_t>(sizeof(x) / sizeof((x)[0]))    // NOLINT
#define LABEL_TO_STRING(x) #x                                                 // NOLINT

template<typename floating>
constexpr bool COMPARE_FLOATS(floating f1, floating f2)
{
    static_assert(std::is_floating_point<floating>::value, "Argument must be a floating point type");

    constexpr double EPSILON = 0.0005; /* Margin for float comparison */

    /* Due to floating point representation imprecision, two floating-point values should never be
     * compared together */
    return std::abs(f1 - f2) <= EPSILON;
}




/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
