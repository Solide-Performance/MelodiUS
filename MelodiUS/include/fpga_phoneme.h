#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#ifndef LINUX_
#include "CommunicationFPGA/CommunicationFPGA.h"
#endif

#include "globaldef.h"
#include <array>
#include <string>
#include <thread>
#include <vector>


/*****************************************************************************/
/* Enums ------------------------------------------------------------------- */
enum class Phoneme : uint8_t
{
    a = 0,
    ey,
    ae,
    i,
    UNKNOWN
};


namespace FPGA
{
enum Registers : int
{
    /* Ports, these values might change */
    BUTTON       = 0,
    SWITCH       = 1,
    TIME_COUNTER = 2,
    ADC0         = 3,
    ADC1         = 4,
    ADC2         = 5,
    ADC3         = 6,
    AN0          = 7,
    AN1          = 8,
    DOT          = 9,
    LED          = 10,
};


/*****************************************************************************/
/* Function declarations - -------------------------------------------------- */


void Init();
void DeInit();
void StartListener();

/* --------------------------------- */
/* Accessors                         */
[[nodiscard]] bool                   isOk();
[[nodiscard]] std::string            ErrorMsg();
[[nodiscard]] std::array<uint8_t, 4> GetADC();
[[nodiscard]] uint8_t                GetADC(size_t channel);
[[nodiscard]] Phoneme                GetCurrentPhoneme();
void SetPhonemeCallback(Phoneme number, std::function<void()> callback);

void WriteLED(uint8_t val);



/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
}    // namespace FPGA