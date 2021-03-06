#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#ifndef LINUX_
#include "CommunicationFPGA/CommunicationFPGA.h"
#endif

#include "globaldef.h"
#include <thread>
#include <vector>


/*****************************************************************************/
/* Defines ----------------------------------------------------------------- */
#define PHONEME_INDEX(phoneme) static_cast<uint8_t>(phoneme)


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr std::chrono::milliseconds ADC_MEASURE_DELAY = std::chrono::milliseconds{50};


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
/* Function declarations - ------------------------------------------------- */


void Init();
void DeInit();
void StartListener();

/* --------------------------------- */
/* Accessors                         */
[[nodiscard]] bool                   isOk();
[[nodiscard]] bool                   isConnected();
[[nodiscard]] std::string            ErrorMsg();
[[nodiscard]] std::array<uint8_t, 4> GetADC();
[[nodiscard]] uint8_t                GetADC(size_t channel);
[[nodiscard]] Phoneme                GetCurrentPhoneme();
void                                 Wait();
void SetPhonemeCallback(Phoneme number, std::function<void()> callback);
void UpdatePhonemeThreshold(std::array<std::array<int, 4>, 4> newThreshold);
void UpdatePhonemeThreshold(Phoneme phoneme, std::array<int, 4> newThreshold);
void SetFlag(bool voiceFlag);
[[nodiscard]] std::array<std::array<int, 4>, 4> GetPhonemeThresholds();
void                                            SetPhonemeCallbackEnabled(bool enabled);

void WriteLED(uint8_t val);

void SaveCalibrationFile();
bool LoadCalibrationFile();



/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
}    // namespace FPGA
